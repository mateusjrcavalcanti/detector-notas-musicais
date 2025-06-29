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

    // Define o divisor do clock PWM baseado na frequência para melhor precisão
    float clock_div;
    if (frequency < 500) {
        clock_div = 8.0f;  // Para frequências baixas (como notas musicais)
    } else {
        clock_div = 2.0f;  // Para frequências mais altas
    }
    pwm_set_clkdiv(slice, clock_div);

    // Calcula o valor de 'wrap' (período do PWM) com melhor precisão
    uint32_t wrap_value = (uint32_t)((125000000.0f / (clock_div * frequency)) - 1);
    
    // Garante um valor mínimo para wrap_value
    if (wrap_value < 10) wrap_value = 10;
    
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

// Função alternativa para tocar som usando toggle direto do GPIO
void buzzer_tone_alt(uint gpio, uint16_t frequency, uint16_t duration_ms)
{
    if (frequency == 0)
        return;

    // Configurar GPIO como saída
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_OUT);
    gpio_put(gpio, 0);

    // Cálculos mais precisos para evitar problemas com frequências baixas
    uint32_t period_us = 1000000 / frequency;
    uint32_t half_period_us = period_us / 2;
    
    // Garantir que não temos valores muito pequenos
    if (half_period_us < 10) half_period_us = 10;
    
    uint32_t cycles = (duration_ms * 1000) / period_us;

    for (uint32_t i = 0; i < cycles; i++)
    {
        gpio_put(gpio, 1);
        sleep_us(half_period_us);
        gpio_put(gpio, 0);
        sleep_us(half_period_us);
    }
    
    // Garantir que o GPIO fica em LOW no final
    gpio_put(gpio, 0);
}
