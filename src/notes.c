#include "notes.h"
#include "buzzer.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Matriz global com todas as notas e suas cores específicas
const nota_info notas_musicais[12] = {
    {"C",  {255, 0, 0},      261.63f},  // Vermelho - Dó
    {"C#", {255, 127, 0},    277.18f},  // Laranja - Dó#
    {"D",  {255, 255, 0},    293.66f},  // Amarelo - Ré
    {"D#", {127, 255, 0},    311.13f},  // Verde Claro - Ré#
    {"E",  {0, 255, 0},      329.63f},  // Verde - Mi
    {"F",  {0, 255, 127},    349.23f},  // Verde Azulado - Fá
    {"F#", {0, 255, 255},    369.99f},  // Ciano - Fá#
    {"G",  {0, 127, 255},    392.00f},  // Azul Claro - Sol
    {"G#", {0, 0, 255},      415.30f},  // Azul - Sol#
    {"A",  {127, 0, 255},    440.00f},  // Roxo - Lá
    {"A#", {255, 0, 255},    466.16f},  // Magenta - Lá#
    {"B",  {255, 0, 127},    493.88f}   // Rosa - Si
};
// Função para encontrar o índice de uma nota
int get_nota_index(const char *note)
{
    for (int i = 0; i < 12; i++)
    {
        if (strcmp(note, notas_musicais[i].nome) == 0)
        {
            return i;
        }
    }
    return -1; // Nota não encontrada
}

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

// Função para tocar um tom usando a biblioteca buzzer
void tocar_nota(const char *note, uint duration_ms)
{
    float freq = get_nota_frequencia(note);
    if (freq > 0) {
        printf("🔊 Tocando %s: %.1f Hz por %dms\n", note, freq, duration_ms);
        
        // Usar método alternativo que é mais preciso para frequências baixas
        buzzer_tone_alt(BUZZER_A, (uint16_t)(freq + 0.5f), duration_ms);
        
        // Pequena pausa após tocar para evitar ruído
        sleep_ms(50);
        
        // Método PWM como backup (pode descomentar se necessário)
        // buzzer_pwm(BUZZER_A, (uint16_t)(freq + 0.5f), duration_ms);
    } else {
        printf("❌ Erro: Frequência inválida para nota '%s'\n", note);
    }
}

void draw_note(PIO pio, uint sm, const char *note)
{
    pixel black = {0, 0, 0};
    pixel vermelho = {255, 0, 0};
    pixel azul = {0, 0, 255};
    
    // Encontra o índice da nota usando a função auxiliar
    int index = get_nota_index(note);
    
    if (index == -1) {
        return; // Nota não encontrada
    }
    
    // Usa a cor específica da nota da matriz global
    pixel cor = notas_musicais[index].cor;

    frame notes[12] = {
        // C - Vermelho
        {
            black, cor, cor, cor, cor,
            cor, black, black, black, black,
            black, black, black, black, cor,
            cor, black, black, black, black,
            black, cor, cor, cor, cor},
        // C# - Laranja
        {azul, cor, cor, cor, cor,
         cor, black, black, black, black,
         black, black, black, black, cor,
         cor, black, black, black, black,
         black, cor, cor, cor, cor},
        //  D - Amarelo
        {black, black, cor, cor, cor,
         cor, black, black, cor, black,
         black, cor, black, black, cor,
         cor, black, cor, cor, black,
         black, black, cor, cor, cor},
        //  D# - Verde Claro
        {vermelho, black, cor, cor, cor,
         cor, black, black, cor, black,
         black, cor, black, black, cor,
         cor, black, cor, cor, black,
         black, black, cor, cor, cor},
        //  E - Verde
        {black, cor, cor, cor, cor,
         cor, black, black, black, black,
         black, cor, cor, cor, cor,
         cor, black, black, black, black,
         black, cor, cor, cor, cor},
        //  F - Verde Azulado
        {black, black, black, black, cor,
         cor, black, black, black, black,
         black, cor, cor, cor, cor,
         cor, black, black, black, black,
         black, cor, cor, cor, cor},
        //  F# - Ciano
        {vermelho, black, black, black, cor,
         cor, black, black, black, black,
         black, cor, cor, cor, cor,
         cor, black, black, black, black,
         black, cor, cor, cor, cor},
        //  G - Azul Claro
        {black, cor, cor, cor, black,
         cor, black, black, black, cor,
         cor, cor, cor, black, cor,
         cor, black, black, black, black,
         black, cor, cor, cor, black},
        //  G# - Azul
        {vermelho, cor, cor, cor, black,
         cor, black, black, black, cor,
         cor, cor, cor, black, cor,
         cor, black, black, black, black,
         black, cor, cor, cor, black},
        //  A - Roxo
        {black, cor, black, black, cor,
         cor, black, black, cor, black,
         black, cor, cor, cor, cor,
         cor, black, black, cor, black,
         black, black, cor, cor, black},
        //  A# - Magenta
        {vermelho, cor, black, black, cor,
         cor, black, black, cor, black,
         black, cor, cor, cor, cor,
         cor, black, black, cor, black,
         black, black, cor, cor, black},
        //  B - Rosa
        {black, black, cor, cor, cor,
         cor, black, black, cor, black,
         black, cor, cor, cor, cor,
         cor, black, black, cor, black,
         black, black, cor, cor, cor},
    };

    // Desenha a nota específica na matriz LED
    draw_pio(notes[index], pio, sm, 0.2);
}

// Função para exibir e tocar uma nota no modo demo
void demo_note(PIO pio, uint sm, const char *note)
{
    // Verificar se a nota é válida
    int index = get_nota_index(note);
    if (index == -1) {
        printf("❌ Erro: Nota '%s' não encontrada!\n", note);
        return;
    }
    
    // Exibir a nota na matriz LED
    draw_note(pio, sm, note);
    
    printf("🎵 Demo: %s (%.1f Hz) - RGB(%d,%d,%d)\n", 
           note, 
           get_nota_frequencia(note),
           notas_musicais[index].cor.red,
           notas_musicais[index].cor.green,
           notas_musicais[index].cor.blue);
    
    // Tocar o tom da nota por 1.5 segundos (deixar 0.5s de pausa)
    tocar_nota(note, 1500); // Tocar por 1500ms (1.5 segundos)
}

// Função para obter a frequência base de uma nota (quarta oitava)
float get_nota_frequencia(const char *note)
{
    int index = get_nota_index(note);
    if (index == -1) {
        printf("⚠️  Aviso: Nota '%s' não encontrada na matriz\n", note);
        return 0.0f; // Nota não encontrada
    }
    
    float freq = notas_musicais[index].frequencia;
    
    // Verificação adicional de sanidade
    if (freq <= 0 || freq > 10000) {
        printf("⚠️  Aviso: Frequência inválida %.2f Hz para nota '%s'\n", freq, note);
        return 0.0f;
    }
    
    return freq;
}

// Função para validar todas as notas da matriz
void validar_matriz_notas()
{
    printf("🔍 Validando matriz de notas musicais...\n");
    bool todas_validas = true;
    
    for (int i = 0; i < 12; i++) {
        const nota_info *nota = &notas_musicais[i];
        
        printf("   %2d. %-3s: %.2f Hz - RGB(%3d,%3d,%3d)", 
               i + 1, nota->nome, nota->frequencia,
               nota->cor.red, nota->cor.green, nota->cor.blue);
        
        // Verificar se a frequência está no range válido para notas musicais
        if (nota->frequencia < 100.0f || nota->frequencia > 2000.0f) {
            printf(" ❌ INVÁLIDA");
            todas_validas = false;
        } else {
            printf(" ✅");
        }
        printf("\n");
    }
    
    if (todas_validas) {
        printf("✅ Todas as notas são válidas!\n");
    } else {
        printf("❌ Algumas notas têm problemas!\n");
    }
    printf("\n");
}

// Função para teste e debug das notas do buzzer
void debug_teste_notas()
{
    printf("🧪 Testando todas as notas do buzzer...\n");
    
    for (int i = 0; i < 12; i++) {
        const char* nota = notas_musicais[i].nome;
        float freq = notas_musicais[i].frequencia;
        
        printf("🎵 Testando %s (%.2f Hz)...\n", nota, freq);
        
        // Verificar se a frequência é válida
        if (freq <= 0) {
            printf("❌ Erro: Frequência inválida para %s\n", nota);
            continue;
        }
        
        // Testar com método alternativo primeiro
        printf("   Método alternativo para %s...\n", nota);
        buzzer_tone_alt(BUZZER_A, (uint16_t)(freq + 0.5f), 800);
        sleep_ms(200); // Pausa entre métodos
        
        // Testar com PWM também
        printf("   PWM test para %s...\n", nota);
        buzzer_pwm(BUZZER_A, (uint16_t)(freq + 0.5f), 800);
        sleep_ms(300); // Pausa maior entre notas
    }
    
    printf("✅ Teste de notas concluído!\n");
}

// Função adicional para debug do demo
void debug_demo_sequencia(PIO pio, uint sm)
{
    printf("🎭 Iniciando debug do demo completo...\n");
    
    for (int i = 0; i < 12; i++) {
        const char* nota = notas_musicais[i].nome;
        printf("\n--- Testando nota %d/12: %s ---\n", i + 1, nota);
        
        // Testar cada componente separadamente
        printf("1. Testando exibição LED...\n");
        draw_note(pio, sm, nota);
        sleep_ms(500);
        
        printf("2. Testando som...\n");
        tocar_nota(nota, 1000);
        
        printf("3. Testando demo completo...\n");
        demo_note(pio, sm, nota);
        
        sleep_ms(1000); // Pausa entre notas do debug
        
        // Limpar LEDs entre notas
        clear_all_leds(pio, sm);
        sleep_ms(200);
    }
    
    printf("\n✅ Debug do demo concluído!\n");
}
