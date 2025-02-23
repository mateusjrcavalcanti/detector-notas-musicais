#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "libs/notas.h"
#include "libs/ssd1306.h"
#include "libs/leds.h"
#include "libs/animation.h"
#include "libs/buzzer.h"
#include "pio_matrix.pio.h"
#include "pico/bootrom.h"

#define SAMPLE_RATE 44100
#define SAMPLES 1024
#define MIC_PIN 28
#define VX_PIN 27
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define BUTTON_A 5
#define BUTTON_B 6

float magnitudes[SAMPLES / 2];
ssd1306_t ssd;
uint last_interrupt_a = 0;
uint last_interrupt_b = 0;
uint DEBOUNCE_MS = 200;
bool buzzer_enabled = false;

enum TELAS
{
    MENU,
    DETECTAR,
    TOCAR
};

uint tela_atual = MENU;
float freq_dominante = 0;
char *nota = "##";
volatile uint nota_selecionada = 0;
struct repeating_timer timer;

PIO pio;
uint sm;

void fft(float *real, float *imag, int n);
const char *detectar_nota(float freq);
float interpolar_pico(int k, float *magnitudes);
void buzzer_pwm(uint gpio, uint16_t frequency, uint16_t duration_ms);
void setup_display();
void setup_button(uint gpio);
void gpio_irq_handler(uint gpio, uint32_t events);
void enable_interrupt();
void menu();
void display_character(char ch);
void PIO_setup(PIO *pio, uint *sm);
bool joystick_callback(struct repeating_timer *t);
bool i2c_recovery();
int main()
{
    stdio_init_all();
    adc_init();
    adc_gpio_init(MIC_PIN);
    adc_gpio_init(VX_PIN);
    setup_display();
    PIO_setup(&pio, &sm);
    add_repeating_timer_ms(100, joystick_callback, NULL, &timer);
    initialization_buzzers(BUZZER_A, BUZZER_B);
    float real[SAMPLES], imag[SAMPLES];

    while (true)
    {
        menu();
        adc_select_input(2);
        absolute_time_t inicio = get_absolute_time();
        float soma = 0;
        setup_button(BUTTON_A);
        setup_button(BUTTON_B);
        enable_interrupt();

        for (int i = 0; i < SAMPLES; i++)
        {
            real[i] = (adc_read() / 2048.0) - 1.0;
            soma += real[i];
            imag[i] = 0;
            busy_wait_until(inicio + (i + 1) * (1000000 / SAMPLE_RATE));
        }

        float media = soma / SAMPLES;
        for (int i = 0; i < SAMPLES; i++)
        {
            real[i] -= media;
        }

        fft(real, imag, SAMPLES);

        float max_magnitude = 0;
        int freq_index = 0;
        for (int i = 1; i < SAMPLES / 2; i++)
        {
            magnitudes[i] = sqrt(real[i] * real[i] + imag[i] * imag[i]);
            if (magnitudes[i] > max_magnitude)
            {
                max_magnitude = magnitudes[i];
                freq_index = i;
            }
        }

        freq_dominante = interpolar_pico(freq_index, magnitudes) * SAMPLE_RATE / SAMPLES;
        nota = detectar_nota(freq_dominante);
        printf("🎶 Frequência: %.2f Hz - Nota: %s\n", freq_dominante, nota);

        if (tela_atual == TOCAR && buzzer_enabled)
        {
            buzzer_pwm(BUZZER_A, (uint16_t)frequencias_base[60 + nota_selecionada], 500);
            printf("🎶 Tocando: %s (%.2f Hz)\n", notas[nota_selecionada], frequencias_base[60 + nota_selecionada]);

            buzzer_enabled = false;
        }

        sleep_ms(500);
    }
}

void fft(float *real, float *imag, int n)
{
    int i, j, k, m;
    int M = 0;
    for (int temp = n; temp > 1; temp >>= 1)
        M++;

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

    for (i = 1; i <= M; i++)
    {
        int passo = 1 << i;
        int metade = passo >> 1;
        float angulo = -M_PI / metade;
        float w_real = cos(angulo);
        float w_imag = sin(angulo);

        for (j = 0; j < n; j += passo)
        {
            float wr = 1.0, wi = 0.0;
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

const char *detectar_nota(float freq)
{
    int i, nota_index = 0;
    float menor_dif = 10000.0;

    for (i = 0; i < sizeof(frequencias_base) / sizeof(frequencias_base[0]); i++)
    {
        float dif = fabs(freq - frequencias_base[i]);
        if (dif < menor_dif)
        {
            menor_dif = dif;
            nota_index = i;
        }
    }

    return notas[nota_index % 12];
}

float interpolar_pico(int k, float *magnitudes)
{
    if (k <= 0 || k >= SAMPLES / 2 - 1)
        return k;
    float alpha = magnitudes[k - 1];
    float beta = magnitudes[k];
    float gamma = magnitudes[k + 1];

    float ajuste = (alpha - gamma) / (2.0 * (alpha - 2.0 * beta + gamma));
    return k + ajuste;
}
bool i2c_recovery()
{
    gpio_set_function(I2C_SDA, GPIO_FUNC_SIO);
    gpio_set_function(I2C_SCL, GPIO_FUNC_SIO);

    gpio_set_dir(I2C_SDA, GPIO_IN);
    gpio_set_dir(I2C_SCL, GPIO_OUT);

    for (int i = 0; i < 9; i++) // 9 ciclos para liberar o barramento
    {
        gpio_put(I2C_SCL, 1);
        sleep_us(10);
        gpio_put(I2C_SCL, 0);
        sleep_us(10);
    }

    gpio_set_dir(I2C_SDA, GPIO_OUT);
    gpio_put(I2C_SDA, 1);
    sleep_us(10);
    gpio_put(I2C_SCL, 1);
    sleep_us(10);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    return true;
}
void setup_display()
{
    i2c_recovery();

    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    int retries = 3;
    while (retries--)
    {
        ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);

        break;
    }

    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

void display_character(char ch);
void selecionar_nota();

void menu()
{
    ssd1306_fill(&ssd, false);

    if (tela_atual == MENU)
    {
        ssd1306_rect(&ssd, 3, 3, 122, 58, true, false);
        ssd1306_draw_string(&ssd, "Bem vindo!", 8, 8);
        ssd1306_draw_string(&ssd, "[A] Detectar", 8, 24);
        ssd1306_draw_string(&ssd, "Nota", 8, 32);
        ssd1306_draw_string(&ssd, "[B] Tocar Nota", 8, 48);
    }
    else if (tela_atual == DETECTAR)
    {
        char mensagem[32];
        snprintf(mensagem, sizeof(mensagem), "Frq: %.2f Hz", freq_dominante);

        char nota_mensagem[16];
        snprintf(nota_mensagem, sizeof(nota_mensagem), "Nota: %s", nota);

        ssd1306_rect(&ssd, 3, 3, 122, 58, true, false);
        ssd1306_draw_string(&ssd, "DETECTANDO:", 8, 8);
        ssd1306_draw_string(&ssd, mensagem, 8, 24);

        ssd1306_draw_string(&ssd, nota_mensagem, 8, 32);
        ssd1306_draw_string(&ssd, "[A] Para Voltar", 8, 48);

        draw_note(pio, sm, nota);
    }
    else if (tela_atual == TOCAR)
    {
        ssd1306_rect(&ssd, 3, 3, 122, 58, true, false);
        selecionar_nota();
    }

    ssd1306_send_data(&ssd);
}

void setup_button(uint gpio)
{
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
    gpio_pull_up(gpio);
}

void gpio_irq_handler(uint gpio, uint32_t events)
{
    uint current_time = to_ms_since_boot(get_absolute_time());

    if (gpio == BUTTON_A)
    {
        if (current_time - last_interrupt_a > DEBOUNCE_MS)
        {
            last_interrupt_a = current_time;

            if (tela_atual == MENU)
            {
                tela_atual = DETECTAR;
            }
            else if (tela_atual == DETECTAR || tela_atual == TOCAR)
            {
                tela_atual = MENU;
            }
        }
    }
    if (gpio == BUTTON_B)
    {
        if (current_time - last_interrupt_b > DEBOUNCE_MS)
        {
            last_interrupt_b = current_time;
            buzzer_enabled = !buzzer_enabled;

            if (tela_atual == MENU)
            {
                tela_atual = TOCAR;
            }
            else if (tela_atual == TOCAR)
            {
                buzzer_enabled = true;
            }
        }
    }
}

void enable_interrupt()
{
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);
}

void display_character(char ch)
{
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 3, 3, 122, 58, true, false);

    char string[2] = {ch, '\0'};

    ssd1306_draw_string(&ssd, "Caractere lido", 8, 10);
    ssd1306_draw_string(&ssd, string, 50, 25);

    ssd1306_send_data(&ssd);
    sleep_ms(1000);
}

void PIO_setup(PIO *pio, uint *sm)
{
    // configurações da PIO
    *pio = pio0;
    uint offset = pio_add_program(*pio, &pio_matrix_program);
    *sm = pio_claim_unused_sm(*pio, true);
    pio_matrix_program_init(*pio, *sm, offset, LED_PIN);
}

void selecionar_nota()
{
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 3, 3, 122, 58, true, false);
    ssd1306_draw_string(&ssd, "Selecione uma", 8, 8);
    ssd1306_draw_string(&ssd, "nota", 8, 16);
    ssd1306_draw_string(&ssd, "com o Joystick:", 8, 24);

    draw_note(pio, sm, notas[nota_selecionada]);
    ssd1306_draw_string(&ssd, notas[nota_selecionada], 8, 40);
    ssd1306_send_data(&ssd);
}

bool joystick_callback(struct repeating_timer *t)
{

    if (tela_atual == TOCAR)
    {

        const int limiar_esquerda = 1500; // Define limite para esquerda
        const int limiar_direita = 2500;  // Define limite para direita
        adc_select_input(1);
        int adc_value = adc_read();

        if (adc_value < limiar_esquerda)
        {
            nota_selecionada = (nota_selecionada - 1 + 12) % 12;
            selecionar_nota();
            buzzer_enabled = false;
        }
        else if (adc_value > limiar_direita)
        {
            nota_selecionada = (nota_selecionada + 1) % 12;
            selecionar_nota();
            buzzer_enabled = false;
        }
    }

    return true;
}