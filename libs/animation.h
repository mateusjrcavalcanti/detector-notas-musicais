#ifndef ANIMATION_H
#define ANIMATION_H

#include "hardware/pio.h"
#include "libs/leds.h"
void draw_number(PIO pio, uint sm, uint index, bool random_colors);
void clear_all_leds(PIO pio, uint sm);
void draw_note(PIO pio, uint sm, const char *note);
#endif
