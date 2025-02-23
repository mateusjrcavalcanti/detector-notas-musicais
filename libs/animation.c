#include "animation.h"
#include "hardware/pio.h"
#include <stdlib.h>
#include <string.h>

// Função para gerar um pixel com cores aleatórias
pixel random_pixel()
{
    pixel p;
    p.red = rand() % 256;
    p.green = rand() % 256;
    p.blue = rand() % 256;
    return p;
}
void clear_all_leds(PIO pio, uint sm)
{
    pixel black = {0, 0, 0};
    frame clear_frame;
    for (int i = 0; i < 25; i++)
    {
        clear_frame[i] = black;
    }
    draw_pio(clear_frame, pio, sm, 1);
}
void draw_number(PIO pio, uint sm, uint index, bool random_colors)
{
    pixel red = {255, 0, 0}, black = {0, 0, 0};
    if (random_colors)
    {
        red = random_pixel();
    }
    frame numbers[10] = {
        // 0
        {black, red, red, red, black,
         black, red, black, red, black,
         black, red, black, red, black,
         black, red, black, red, black,
         black, red, red, red, black},
        //  1
        {black, red, red, black, black,
         black, black, black, red, black,
         black, red, black, black, black,
         black, black, red, red, black,
         black, red, black, black, black},
        // 2
        {black, red, red, red, black,
         black, red, black, black, black,
         black, red, red, red, black,
         black, black, black, red, black,
         black, red, red, red, black},
        //  3
        {black, red, red, red, black,
         black, black, black, red, black,
         black, red, red, red, black,
         black, black, black, red, black,
         black, red, red, red, black},
        //  4
        {
            black,
            red,
            black,
            black,
            black,
            black,
            black,
            black,
            red,
            black,
            black,
            red,
            red,
            red,
            black,
            black,
            red,
            black,
            red,
            black,
            black,
            red,
            black,
            red,
            black,
        },
        //  5
        {black, red, red, red, black,
         black, black, black, red, black,
         black, red, red, red, black,
         black, red, black, black, black,
         black, red, red, red, black},

        // 6
        {black, red, red, red, black,
         black, red, black, red, black,
         black, red, red, red, black,
         black, red, black, black, black,
         black, red, red, red, black},
        //  7
        {black, red, black, black, black,
         black, black, black, red, black,
         black, red, black, black, black,
         black, black, black, red, black,
         black, red, red, red, black},
        //  8
        {black, red, red, red, black,
         black, red, black, red, black,
         black, red, red, red, black,
         black, red, black, red, black,
         black, red, red, red, black},
        //  9
        {black, red, red, red, black,
         black, black, black, red, black,
         black, red, red, red, black,
         black, red, black, red, black,
         black, red, red, red, black},

    };

    // for (int i = 0; i <= 10; i++)
    // {
    //     draw_pio(numbers[i], pio, sm, 1);
    //     sleep_ms(500);
    // }
    draw_pio(numbers[index], pio, sm, .2);
}

void draw_note(PIO pio, uint sm, char *note)
{
    pixel red = {255, 0, 0}, black = {0, 0, 0}, blue = {0, 0, 255};
    const char *notas[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

    frame notes[12] = {
        // C
        {
            black, red, red, red, red,
            red, black, black, black, black,
            black, black, black, black, red,
            red, black, black, black, black,
            black, red, red, red, red},
        // C#
        {blue, red, red, red, red,
         red, black, black, black, black,
         black, black, black, black, red,
         red, black, black, black, black,
         black, red, red, red, red},
        //  D
        {black, black, red, red, red,
         red, black, black, red, black,
         black, red, black, black, red,
         red, black, red, red, black,
         black, black, red, red, red},
        //  D#
        {blue, black, red, red, red,
         red, black, black, red, black,
         black, red, black, black, red,
         red, black, red, red, black,
         black, black, red, red, red},
        //  E
        {black, red, red, red, red,
         red, black, black, black, black,
         black, red, red, red, red,
         red, black, black, black, black,
         black, red, red, red, red},
        //  F
        {black, black, black, black, red,
         red, black, black, black, black,
         black, red, red, red, red,
         red, black, black, black, black,
         black, red, red, red, red},
        //  F#
        {blue, black, black, black, red,
         red, black, black, black, black,
         black, red, red, red, red,
         red, black, black, black, black,
         black, red, red, red, red},
        //  G
        {black, red, red, red, black,
         red, black, black, black, red,
         red, red, red, black, red,
         red, black, black, black, black,
         black, red, red, red, black},
        //  G#
        {blue, red, red, red, black,
         red, black, black, black, red,
         red, red, red, black, red,
         red, black, black, black, black,
         black, red, red, red, black},
        //  A
        {black, red, black, black, red,
         red, black, black, red, black,
         black, red, red, red, red,
         red, black, black, red, black,
         black, black, red, red, black},
        //  A#
        {blue, red, black, black, red,
         red, black, black, red, black,
         black, red, red, red, red,
         red, black, black, red, black,
         black, black, red, red, black},
        //  B
        {black, black, red, red, red,
         red, black, black, red, black,
         black, red, red, red, red,
         red, black, black, red, black,
         black, black, red, red, red},
    };

    int index = -1;
    for (int i = 0; i < 12; i++)
    {
        if (strcmp(note, notas[i]) == 0)
        {
            index = i;
            break;
        }
    }

    if (index >= 0)
    {
        draw_pio(notes[index], pio, sm, 0.2);
    }
}