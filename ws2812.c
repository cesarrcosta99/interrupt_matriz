#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "ws2812.pio.h"

// --- Definições de Hardware ---
#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7

// GPIOs do LED RGB
#define LED_R 13
#define LED_G 12
#define LED_B 11

// GPIOs dos Botões
#define BUTTON_A 5
#define BUTTON_B 6

// --- Variáveis Globais (volatile para uso em interrupções) ---
volatile int contador = 0; // Contador para os números (0-9)
volatile uint32_t ultimo_tempo_A = 0;
volatile uint32_t ultimo_tempo_B = 0;
bool led_rgb_estado = false; // Estado do LED RGB

// Buffer da matriz de LEDs (será atualizado com os números)
bool led_buffer[NUM_PIXELS] = {0};

// Inicializa o LED RGB
void setup_rgb_led()
{
    gpio_init(LED_R);
    gpio_init(LED_G);
    gpio_init(LED_B);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_set_dir(LED_B, GPIO_OUT);
}

// Controla o LED vermelho (piscar)
void piscar_led_vermelho()
{
    led_rgb_estado = !led_rgb_estado;
    gpio_put(LED_R, led_rgb_estado);
}



// Protótipo da função debounce
bool debounce(volatile uint32_t *ultimo_tempo);

// Tratador de interrupção único para ambos os botões
void button_handler(uint gpio, uint32_t events)
{
    if (gpio == BUTTON_A)
    {
        if (debounce(&ultimo_tempo_A) && contador < 9)
        {
            contador++;
        }
    }
    else if (gpio == BUTTON_B)
    {
        if (debounce(&ultimo_tempo_B) && contador > 0)
        {
            contador--;
        }
    }
}

// Função de debouncing 
bool debounce(volatile uint32_t *ultimo_tempo) {
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
    if (tempo_atual - *ultimo_tempo > 200) {
        *ultimo_tempo = tempo_atual;
        return true;
    }
    return false;
}

// Configura os botões
void setup_botoes()
{
    gpio_init(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_pull_up(BUTTON_B);

    // Configura interrupções com um único callback
    gpio_set_irq_enabled(BUTTON_A, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_callback(&button_handler); // Define o callback global
    irq_set_enabled(IO_IRQ_BANK0, true);    // Habilita interrupções
}

// Lookup table para números de 0 a 9 (cada número é um array de 25 posições)
const bool numeros[10][25] = {
    // Número 0
     {1,1,1,1,1,   // Linha 0: 0-4
     1,0,0,0,1,   // Linha 1: 9-5 (invertida: 1 no índice 4 e 0)
     1,0,0,0,1,   // Linha 2: 10-14
     1,0,0,0,1,   // Linha 3: 19-15 (invertida)
     1,1,1,1,1},  // Linha 4: 20-24

    // Número 1 (Coluna central)
    {0,0,1,0,0,   // Linha 0
     0,0,1,0,0,   // Linha 1 (invertida: 1 no índice 2)
     0,0,1,0,1,   // Linha 2
     0,1,1,0,0,   // Linha 3 (invertida)
     0,0,1,0,0},  // Linha 4

    // Número 2
    {1, 1, 1, 1, 1,
     1, 0, 0, 0, 0,
     1, 1, 1, 1, 1,
     0, 0, 0, 0, 1,
     1, 1, 1, 1, 1},


    // Número 3
     {1,1,1,1,1,   
     0,0,0,0,1,   
     1,1,1,1,1,   
     0,0,0,0,1,   
     1,1,1,1,1},

    // Número 4
    {1,0,0,0,0,   
     0,0,0,0,1,   
     1,1,1,1,1,   
     1,0,0,0,1,   
     1,0,0,0,1},

    // Número 5
   {1,1,1,1,1,   // Linha 0
     0,0,0,0,1,   // Linha 1 (invertida: 1 no índice 4)
     1,1,1,1,1,   // Linha 2
     1,0,0,0,0,   // Linha 3 (invertida: 1 no índice 0)
     1,1,1,1,1},  // Linha 4

    // Número 6
    {1, 1, 1, 1, 1,
     1, 0, 0, 0, 1,
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 0,
     1, 1, 1, 1, 1},

    // Número 7 
{
    1, 0, 0, 0, 0,  
    0, 0, 0, 0, 1,  
    1, 0, 0, 0, 0,  
    0, 0, 0, 0, 1,  
    1, 1, 1, 1, 1   
},

    // Número 8
    {1, 1, 1, 1, 1,
     1, 0, 0, 0, 1,
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 1,
     1, 1, 1, 1, 1},

    // Número 9
    {1, 1, 1, 1, 1,
     0, 0, 0, 0, 1,
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 1,
     1, 1, 1, 1, 1}};

// Atualiza o buffer com o número atual
void atualizar_matriz()
{
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        led_buffer[i] = numeros[contador][i];
    }
}

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void set_one_led(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t color = urgb_u32(r, g, b);
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        put_pixel(led_buffer[i] ? color : 0);
    }
}

int main()
{
    stdio_init_all();
    setup_rgb_led();
    setup_botoes();

    // Inicialização do PIO para a matriz WS2812
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    uint32_t ultimo_piscar = 0;

    while (1)
    {
        // Atualiza a matriz com o número atual
        atualizar_matriz();
        set_one_led(0, 0, 255); // Azul para os números

        // Piscar o LED vermelho a 5 Hz (100 ms ligado, 100 ms desligado)
        uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
        if (tempo_atual - ultimo_piscar > 100)
        {
            piscar_led_vermelho();
            ultimo_piscar = tempo_atual;
        }

        // Debug: Mostra o valor do contador
        printf("Contador: %d\n", contador);

        sleep_ms(10); // Reduz o consumo de CPU
    }

    return 0;
}
