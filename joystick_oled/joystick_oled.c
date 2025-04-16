#include <stdio.h>              // Biblioteca padrão para entrada/saída
#include "pico/stdlib.h"        // Biblioteca padrão do Raspberry Pi Pico
#include "hardware/timer.h"     // Para usar o timer/alarm do Pico
#include "ssd1306/ssd1306.h"    // Para controlar o display OLED SSD1306
#include "hardware/i2c.h"       // Para comunicação I2C com o display
#include "hardware/adc.h"       // Para leitura analógica do joystick via ADC
#include "pico/rand.h"          // Para geração de números aleatórios
#include "hardware/pwm.h"       // Para controle PWM (LEDs e buzzer)
#include "hardware/clocks.h"    // Para configuração de clock do Pico

// Definições de constantes
#define COUNTDOWN_START 9 // Valor inicial da contagem regressiva
#define JOYSTICK_X 26     // Pino GPIO para o eixo X do joystick
#define JOYSTICK_Y 27     // Pino GPIO para o eixo Y do joystick
#define ADC_CHANNEL_0 0   // Canal ADC para leitura X do joystick
#define ADC_CHANNEL_1 1   // Canal ADC para leitura Y do joystick

// Variáveis globais
ssd1306_t disp;                        // Objeto para controlar o display OLED
static volatile bool button_A_pressed = false;  // Flag para botão A
static volatile bool button_B_pressed = false;  // Flag para botão B
static uint8_t button_count = 0;       // Contador de botões pressionados
static absolute_time_t last_button_time = {0};  // Tempo do último pressionamento de botão
static char string_contador[20];       // Buffer para armazenar string do contador

// Variáveis da contagem regressiva
static int countdown_value = 0;        // Valor atual da contagem regressiva
static bool countdown_active = false;  // Flag de contagem regressiva ativa
static absolute_time_t last_second_time = {0};  // Tempo da última atualização do contador
static bool countdown_finished = false; // Flag para controlar o estado após a contagem terminar

/**
 * Inicializa o display OLED SSD1306 via I2C
 */
void inicializar_display(void) {
    i2c_init(i2c1, 400000);            // Inicializa I2C1 com 400kHz
    gpio_set_function(14, GPIO_FUNC_I2C); // Configura GPIO14 como SDA
    gpio_set_function(15, GPIO_FUNC_I2C); // Configura GPIO15 como SCL
    gpio_pull_up(14);                  // Ativa pull-up para SDA
    gpio_pull_up(15);                  // Ativa pull-up para SCL
    
    disp.external_vcc = false;         // OLED usa fonte interna
    ssd1306_init(&disp, 128, 64, 0x3C, i2c1); // Inicializa display 128x64 no endereço 0x3C
    ssd1306_clear(&disp);              // Limpa o display
}

/**
 * Inicializa o joystick analógico via ADC
 */
void inicializar_joystick(void) {
    adc_init();                        // Inicializa o ADC
    adc_gpio_init(JOYSTICK_X);         // Configura pino do joystick X como entrada ADC
}

/**
 * Lê o valor do eixo X do joystick
 * @param eixo_x Ponteiro para armazenar o valor lido
 */
void ler_joystick_x(uint16_t *eixo_x) {
    adc_select_input(ADC_CHANNEL_0);   // Seleciona canal ADC para eixo X
    sleep_us(2);                       // Pequeno delay para estabilização
    *eixo_x = adc_read();              // Lê o valor (0-4095)
}

/**
 * Lê o valor do eixo Y do joystick
 * @param eixo_y Ponteiro para armazenar o valor lido
 */
void ler_joystick_y(uint16_t *eixo_y) {
    adc_select_input(ADC_CHANNEL_1);   // Seleciona canal ADC para eixo Y
    sleep_us(2);                       // Pequeno delay para estabilização
    *eixo_y = adc_read();              // Lê o valor (0-4095)
}

/**
 * Escreve texto no display OLED
 * @param str String a ser escrita
 * @param x Posição X inicial
 * @param y Posição Y inicial
 * @param limpar Se true, limpa o display antes de escrever
 */
void escrever_texto(char *str, uint32_t x, uint32_t y, bool limpar) {
    if (limpar) {
        ssd1306_clear(&disp);          // Limpa o display se solicitado
        sleep_ms(10);                  // Pequeno delay após limpar
    }
    ssd1306_draw_string(&disp, x, y, 1, str);  // Desenha a string no buffer
    ssd1306_show(&disp);               // Atualiza o display físico
}

/**
 * Inicializa todos os componentes do sistema
 */
void srk_init() {
    stdio_init_all();                  // Inicializa stdio (Serial, USB)
    inicializar_display();             // Inicializa o display OLED
    inicializar_joystick();            // Inicializa o joystick
}

/**
 * Função principal
 */
int main() {
    srk_init();                        // Inicializa os componentes
    
    // Estado inicial do programa
    countdown_value = 0;
    countdown_active = false;
    countdown_finished = true;
    button_count = 0;
    
    // Variáveis para leitura do joystick
    uint16_t valor_x = 0;
    uint16_t valor_y = 0;
    
    // Buffers para conversão de valores numéricos para string
    char string_x[10];
    char string_y[10];

    // Loop principal
    while (true) {
        // Lê os valores X e Y do joystick
        ler_joystick_x(&valor_x);
        ler_joystick_y(&valor_y);
        
        // Converte os valores para strings
        sprintf(string_x, "%d", valor_x);
        sprintf(string_y, "%d", valor_y);
        
        // Mostra os valores no display
        escrever_texto(string_x, 30, 30, false);
        escrever_texto(string_y, 30, 45, false);
        
        // Limpa uma área específica do display
        ssd1306_clear_square(&disp, 30, 10, 50, 50);
        
        // Pequeno delay para estabilidade do sistema
        sleep_ms(500);
    }
}