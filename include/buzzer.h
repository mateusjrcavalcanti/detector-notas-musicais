#ifndef BUZZER_H
#define BUZZER_H

#define BUZZER_A 10
#define BUZZER_B 21

#include <stdint.h>
#include <stddef.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include <string.h>

// Inicializa os buzzers configurados para os pinos fornecidos
void initialization_buzzers(uint gpio_buzzer_dot, uint gpio_buzzer_dash);

// Configuração e PWM para os buzzers
void buzzer_pwm(uint gpio, uint16_t frequency, uint16_t duration_ms);

// Emite a sequência de som em código Morse
void play_morse_code(const char *morse, uint gpio_buzzer_dot, uint gpio_buzzer_dash);

// Emite um único tom no buzzer especificado
void play_tone(uint gpio, uint frequency, uint duration_ms);

// Função alternativa para tocar som usando toggle direto do GPIO
void buzzer_tone_alt(uint gpio, uint16_t frequency, uint16_t duration_ms);

#endif // BUZZER_H
