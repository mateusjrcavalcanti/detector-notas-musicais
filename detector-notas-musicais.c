#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "libs/notas.h"

#define SAMPLE_RATE 44100
#define SAMPLES 1024
#define MIC_PIN 28
#define BUZZER_PIN 22

float magnitudes[SAMPLES / 2];

void fft(float *real, float *imag, int n);
const char *detectar_nota(float freq);
float interpolar_pico(int k, float *magnitudes);
void buzzer_pwm(uint gpio, uint16_t frequency, uint16_t duration_ms);

int main()
{
    stdio_init_all();
    adc_init();
    adc_gpio_init(MIC_PIN);
    adc_select_input(2);

    float real[SAMPLES], imag[SAMPLES];

    while (true)
    {
        absolute_time_t inicio = get_absolute_time();
        float soma = 0;

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

        float freq_dominante = interpolar_pico(freq_index, magnitudes) * SAMPLE_RATE / SAMPLES;
        const char *nota = detectar_nota(freq_dominante);

        printf("🎶 Frequência: %.2f Hz - Nota: %s\n", freq_dominante, nota);
        buzzer_pwm(BUZZER_PIN, (uint16_t)freq_dominante, 500);

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

void buzzer_pwm(uint gpio, uint16_t frequency, uint16_t duration_ms)
{
    if (frequency == 0)
        return;

    gpio_set_function(gpio, GPIO_FUNC_PWM);

    uint slice = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);

    float clock_div = 4.0f;
    pwm_set_clkdiv(slice, clock_div);

    uint32_t wrap_value = (125000000 / (clock_div * frequency)) - 1;
    pwm_set_wrap(slice, wrap_value);
    pwm_set_chan_level(slice, channel, wrap_value / 2);

    pwm_set_enabled(slice, true);
    sleep_ms(duration_ms);

    pwm_set_enabled(slice, false);
    gpio_set_function(gpio, GPIO_FUNC_SIO);
    gpio_set_dir(gpio, GPIO_OUT);
    gpio_put(gpio, 0);
}
