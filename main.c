#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "hardware/timer.h"
#include "leds.h"
#include "notes.h"
#include "buzzer.h"
#include "pio_matrix.pio.h"

#define SAMPLE_RATE 8000     // Taxa de amostragem (8kHz para economizar memória)
#define SAMPLES 512          // Número de amostras para FFT (potência de 2)
#define MIC_PIN 28           // Pino do microfone
#define FREQ_RESOLUTION ((float)SAMPLE_RATE / SAMPLES)  // Resolução em frequência

volatile uint nota_selecionada = 0;
volatile bool detectar_audio = false; // Iniciar em modo demo
volatile bool modo_demo_ativo = true; // Flag para controlar modo demo inicial
volatile uint demo_contador = 0;     // Contador para controlar duração do demo
volatile bool nota_mudou = true;     // Flag para indicar mudança de nota
struct repeating_timer timer;
float amostras_audio[SAMPLES];
float magnitudes[SAMPLES / 2];

PIO pio;
uint sm;

void PIO_setup(PIO *pio, uint *sm);
bool timer_callback(struct repeating_timer *t);
void fft(float *real, float *imag, int n);
float encontrar_frequencia_dominante();
const char* detectar_nota_por_frequencia(float freq);
void capturar_audio();
void processar_audio();

int main()
{
    stdio_init_all();
    
    // Inicializar ADC para o microfone
    adc_init();
    adc_gpio_init(MIC_PIN);
    adc_select_input(2); // ADC2 para o pino 28
    
    PIO_setup(&pio, &sm);
    
    // Inicializar buzzers
    initialization_buzzers(BUZZER_A, BUZZER_B);
    
    // Validar matriz de notas
    validar_matriz_notas();
    
    add_repeating_timer_ms(2500, timer_callback, NULL, &timer); // Mudado para 2.5 segundos

    printf("🎤 Detector de Notas Musicais Iniciado!\n");
    printf("📊 Taxa de amostragem: %d Hz\n", SAMPLE_RATE);
    printf("🔢 Amostras por análise: %d\n", SAMPLES);
    printf("📈 Resolução de frequência: %.2f Hz\n", FREQ_RESOLUTION);
    printf("🎭 Iniciando modo demonstração (cada nota por 2.5s)...\n\n");
    
    // Tocar a primeira nota imediatamente
    printf("🎵 Iniciando com nota: %s\n", notas_musicais[nota_selecionada].nome);
    demo_note(pio, sm, notas_musicais[nota_selecionada].nome);
    nota_mudou = false;

    while (true)
    {
        if (detectar_audio)
        {
            // Capturar e processar áudio
            capturar_audio();
            processar_audio();
        }
        else if (modo_demo_ativo && nota_mudou)
        {
            // Modo demonstração - exibir e tocar notas sequencialmente
            printf("🎼 Executando demo para: %s\n", notas_musicais[nota_selecionada].nome);
            
            // Limpar LEDs antes de mostrar nova nota
            clear_all_leds(pio, sm);
            sleep_ms(100);
            
            // Executar demo da nota atual
            demo_note(pio, sm, notas_musicais[nota_selecionada].nome);
            nota_mudou = false; // Resetar flag para evitar repetição
        }
        
        sleep_ms(50); // Pausa menor para responsividade
    }
}

void PIO_setup(PIO *pio, uint *sm)
{
    // configurações da PIO
    *pio = pio0;
    uint offset = pio_add_program(*pio, &pio_matrix_program);
    *sm = pio_claim_unused_sm(*pio, true);
    pio_matrix_program_init(*pio, *sm, offset, LED_PIN);
}

bool timer_callback(struct repeating_timer *t)
{
    if (modo_demo_ativo)
    {
        // Modo demonstração - muda para a próxima nota a cada 2.5 segundos
        nota_selecionada = (nota_selecionada + 1) % 12;
        nota_mudou = true; // Sinalizar que a nota mudou
        demo_contador++;
        
        printf("⏰ Timer: Mudando para nota %d (%s)\n", 
               nota_selecionada, notas_musicais[nota_selecionada].nome);
        
        // Após 30 segundos (1 ciclo completo de notas com 2.5s cada), ativar detecção de áudio
        if (demo_contador >= 12)
        {
            modo_demo_ativo = false;
            detectar_audio = true;
            printf("\n🎤 Modo demo finalizado! Iniciando detecção de áudio...\n");
            printf("🎵 Cante ou toque uma nota musical!\n\n");
        }
    }
    
    return true;
}

void capturar_audio()
{
    absolute_time_t inicio = get_absolute_time();
    float soma = 0;
    
    // Capturar amostras do microfone
    for (int i = 0; i < SAMPLES; i++)
    {
        // Ler valor do ADC (0-4095) e converter para -1.0 a 1.0
        uint16_t adc_value = adc_read();
        amostras_audio[i] = (adc_value / 2048.0f) - 1.0f;
        soma += amostras_audio[i];
        
        // Aguardar próxima amostra baseado na taxa de amostragem
        busy_wait_until(inicio + (i + 1) * (1000000 / SAMPLE_RATE));
    }
    
    // Remover componente DC (média)
    float media = soma / SAMPLES;
    for (int i = 0; i < SAMPLES; i++)
    {
        amostras_audio[i] -= media;
    }
}

void processar_audio()
{
    // Preparar arrays para FFT
    float real[SAMPLES], imag[SAMPLES];
    
    // Copiar amostras reais e zerar imaginárias
    for (int i = 0; i < SAMPLES; i++)
    {
        real[i] = amostras_audio[i];
        imag[i] = 0.0f;
    }
    
    // Executar FFT
    fft(real, imag, SAMPLES);
    
    // Calcular magnitudes
    for (int i = 0; i < SAMPLES / 2; i++)
    {
        magnitudes[i] = sqrt(real[i] * real[i] + imag[i] * imag[i]);
    }
    
    // Encontrar frequência dominante
    float freq_dominante = encontrar_frequencia_dominante();
    
    if (freq_dominante > 50.0f && freq_dominante < 2000.0f) // Filtrar frequências válidas
    {
        const char* nota_detectada = detectar_nota_por_frequencia(freq_dominante);
        
        // Exibir resultado
        printf("🎵 Freq: %.1f Hz -> Nota: %s\n", freq_dominante, nota_detectada);
        
        // Exibir na matriz LED
        draw_note(pio, sm, nota_detectada);
    }
}

float encontrar_frequencia_dominante()
{
    float max_magnitude = 0;
    int freq_index = 1; // Começar do índice 1 para evitar DC
    
    // Procurar pico de magnitude no espectro
    for (int i = 1; i < SAMPLES / 2; i++)
    {
        if (magnitudes[i] > max_magnitude)
        {
            max_magnitude = magnitudes[i];
            freq_index = i;
        }
    }
    
    // Interpolação parabólica para maior precisão
    if (freq_index > 1 && freq_index < (SAMPLES / 2 - 1))
    {
        float y1 = magnitudes[freq_index - 1];
        float y2 = magnitudes[freq_index];
        float y3 = magnitudes[freq_index + 1];
        
        float ajuste = (y1 - y3) / (2.0f * (y1 - 2.0f * y2 + y3));
        float freq_interpolada = (freq_index + ajuste) * FREQ_RESOLUTION;
        
        return freq_interpolada;
    }
    
    return freq_index * FREQ_RESOLUTION;
}

const char* detectar_nota_por_frequencia(float freq)
{
    const char* melhor_nota = "?";
    float menor_diferenca = 10000.0f;
    
    // Comparar com frequências das notas (várias oitavas)
    for (int nota = 0; nota < 12; nota++)
    {
        // Verificar em diferentes oitavas (A1 a A7)
        for (int oitava = 1; oitava <= 7; oitava++)
        {
            float freq_nota = get_nota_frequencia(notas_musicais[nota].nome);
            
            // Calcular frequência para a oitava atual
            // A4 = 440Hz está na 4ª oitava, então ajustamos relativament
            float freq_oitava = freq_nota * pow(2, oitava - 4);
            
            float diferenca = fabs(freq - freq_oitava);
            
            if (diferenca < menor_diferenca)
            {
                menor_diferenca = diferenca;
                melhor_nota = notas_musicais[nota].nome;
            }
        }
    }
    
    return melhor_nota;
}

void fft(float *real, float *imag, int n)
{
    int i, j, k, m;
    int M = 0;
    
    // Calcular log2(n)
    for (int temp = n; temp > 1; temp >>= 1)
        M++;

    // Bit-reversal permutation
    for (i = 0; i < n; i++)
    {
        int bit_reverso = 0;
        for (j = 0; j < M; j++)
            if (i & (1 << j))
                bit_reverso |= (1 << (M - 1 - j));

        if (bit_reverso > i)
        {
            float temp_real = real[i];
            float temp_imag = imag[i];
            real[i] = real[bit_reverso];
            imag[i] = imag[bit_reverso];
            real[bit_reverso] = temp_real;
            imag[bit_reverso] = temp_imag;
        }
    }

    // FFT Cooley-Tukey
    for (i = 1; i <= M; i++)
    {
        int passo = 1 << i;
        int metade = passo >> 1;
        float angulo = -M_PI / metade;
        float w_real = cosf(angulo);
        float w_imag = sinf(angulo);

        for (j = 0; j < n; j += passo)
        {
            float wr = 1.0f, wi = 0.0f;
            for (k = 0; k < metade; k++)
            {
                float t_real = wr * real[j + k + metade] - wi * imag[j + k + metade];
                float t_imag = wr * imag[j + k + metade] + wi * real[j + k + metade];
                
                real[j + k + metade] = real[j + k] - t_real;
                imag[j + k + metade] = imag[j + k] - t_imag;
                real[j + k] += t_real;
                imag[j + k] += t_imag;
                
                float temp_w = wr;
                wr = wr * w_real - wi * w_imag;
                wi = temp_w * w_imag + wi * w_real;
            }
        }
    }
}