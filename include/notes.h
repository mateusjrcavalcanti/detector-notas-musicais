#ifndef NOTES_H
#define NOTES_H

#include "hardware/pio.h"
#include "leds.h"

// Estrutura para armazenar informações de cada nota
typedef struct {
    const char* nome;
     pixel cor;
    float frequencia;
} nota_info;

// Matriz global com todas as notas e suas cores
extern const nota_info notas_musicais[12];

void clear_all_leds(PIO pio, uint sm);
void draw_note(PIO pio, uint sm, const char *note);
void demo_note(PIO pio, uint sm, const char *note);
void tocar_nota(const char *note, uint duration_ms);
int get_nota_index(const char *note);
float get_nota_frequencia(const char *note);
void debug_teste_notas();
void debug_demo_sequencia(PIO pio, uint sm);
void validar_matriz_notas();

#endif
