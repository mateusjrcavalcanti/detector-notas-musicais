#include "buzzer.h"

// Inicializa os buzzers a serem utilizados
void initialization_buzzers(uint gpio_buzzer_a, uint gpio_buzzer_b)
{
    gpio_init(gpio_buzzer_a);
    gpio_set_dir(gpio_buzzer_a, GPIO_OUT);
    gpio_put(gpio_buzzer_a, 0);

    gpio_init(gpio_buzzer_b);
    gpio_set_dir(gpio_buzzer_b, GPIO_OUT);
    gpio_put(gpio_buzzer_b, 0);
}

// Configura e ativa o PWM para controlar o buzzer
void buzzer_pwm(uint gpio, uint16_t frequency, uint16_t duration_ms)
{
    if (frequency == 0)
        return; // Evita divisão por zero

    // Configura o pino como saída de PWM
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    // Obtém o número do slice PWM associado ao pino
    uint slice = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);

    // Define o divisor do clock PWM
    float clock_div = 4.0f; // Ajustável para diferentes frequências
    pwm_set_clkdiv(slice, clock_div);

    // Calcula o valor de 'wrap' (período do PWM)
    uint32_t wrap_value = (125000000 / (clock_div * frequency)) - 1;
    pwm_set_wrap(slice, wrap_value);

    // Define o duty cycle para 50% (meio ciclo ligado, meio desligado)
    pwm_set_chan_level(slice, channel, wrap_value / 2);

    // Habilita o PWM
    pwm_set_enabled(slice, true);

    // Mantém o tom pelo tempo especificado
    sleep_ms(duration_ms);

    // Desliga o PWM e reseta o pino
    pwm_set_enabled(slice, false);
    gpio_set_function(gpio, GPIO_FUNC_SIO);
    gpio_set_dir(gpio, GPIO_OUT);
    gpio_put(gpio, 0);
}
// Toca uma sequência de código Morse
void play_morse_code(const char *morse, uint gpio_buzzer_dot, uint gpio_buzzer_dash)
{
    for (size_t i = 0; i < strlen(morse); i++)
    {
        if (morse[i] == '.')
        {
            buzzer_pwm(gpio_buzzer_dot, 5280, 100); // Ponto
        }
        else if (morse[i] == '-')
        {
            buzzer_pwm(gpio_buzzer_dash, 5280, 300); // Traço
        }
        sleep_ms(100); // Intervalo entre pontos/traços
    }
    sleep_ms(300); // Intervalo entre letras
}

// Gera um som com a frequência especificada por um tempo determinado
void play_tone(uint gpio, uint frequency, uint duration_ms)
{
    if (frequency == 0)
        return;

    uint period = 1000000 / frequency;
    uint half_period = period / 2;

    for (uint i = 0; i < (duration_ms * 1000) / period; i++)
    {
        gpio_put(gpio, 1);
        sleep_us(half_period);
        gpio_put(gpio, 0);
        sleep_us(half_period);
    }
}
