#include <stdio.h>              // Biblioteca padrão para entrada/saída
#include "pico/stdlib.h"        // Biblioteca padrão do Pico
#include "hardware/timer.h"     // Para usar o timer/alarm
#include "ssd1306/ssd1306.h"    // Para usar o display OLED
#include "hardware/i2c.h"       // Para comunicação I2C com o display
#include "hardware/adc.h"       // Para leitura do sensor via ADC
#include "pico/rand.h"          // Para geração de números aleatórios (não utilizado neste código)
#include "hardware/pwm.h"       // Para controle PWM (não utilizado neste código)
#include "hardware/clocks.h"    // Para configuração de clock (não utilizado neste código)

// Define o canal do ADC para o sensor de temperatura interno do RP2040
#define ADC_CHANNEL 4 // O sensor de temperatura interno está no canal 4

// Variável global para controlar o display OLED
ssd1306_t disp;

/**
 * Inicializa o display OLED com comunicação I2C
 * - Configura os pinos GPIO 14 (SDA) e 15 (SCL)
 * - Inicializa com resolução 128x64 no endereço I2C 0x3C
 */
void inicializar_display(void) {
    i2c_init(i2c1, 400000);                     // Inicializa I2C com 400kHz
    gpio_set_function(14, GPIO_FUNC_I2C);       // Configura GPIO14 como SDA
    gpio_set_function(15, GPIO_FUNC_I2C);       // Configura GPIO15 como SCL
    gpio_pull_up(14);                           // Ativa pull-up no pino SDA
    gpio_pull_up(15);                           // Ativa pull-up no pino SCL
    disp.external_vcc = false;                  // O display não usa fonte externa
    ssd1306_init(&disp, 128, 64, 0x3C, i2c1);   // Inicializa com resolução 128x64 no endereço 0x3C
    ssd1306_clear(&disp);                       // Limpa o display
}

/**
 * Escreve texto no display OLED
 * @param str - String a ser escrita
 * @param x - Posição X no display
 * @param y - Posição Y no display
 * @param limpar - Se true, limpa o display antes de escrever
 */
void escrever_texto(char *str, uint32_t x, uint32_t y, bool limpar) {
    if (limpar) {
        ssd1306_clear(&disp);                   // Limpa o display se solicitado
        sleep_ms(10);                           // Pequeno delay para estabilização
    }
    ssd1306_draw_string(&disp, x, y, 1, str);   // Desenha a string nas coordenadas especificadas
    ssd1306_show(&disp);                        // Atualiza o display com o novo conteúdo
}

int main(void){

    stdio_init_all();                           // Inicializa a comunicação serial padrão
    inicializar_display();                      // Inicializa o display OLED

    char str_temp[10];                          // Buffer para guardar a string com a temperatura

    // Configurando o ADC para leitura do sensor de temperatura
    adc_init();                                 // Inicializa o ADC
    adc_set_temp_sensor_enabled(true);          // Ativa o sensor de temperatura interno
    adc_select_input(ADC_CHANNEL);              // Seleciona o canal 4 (sensor de temperatura)

    // Loop principal - lê e exibe a temperatura continuamente
    while (1) {
        uint16_t raw = adc_read();              // Lê o valor bruto do ADC (0-4095)
        const float conversao = 3.3f / (1 << 12); // Fator de conversão para 3.3V com ADC de 12 bits
        float resultado = raw * conversao;      // Converte valor ADC para tensão (V)
        
        // Fórmula de conversão da tensão para temperatura em graus Celsius
        // Baseada na documentação do RP2040: T = 27 - (V - 0.706)/0.001721
        float temp = 27 - (resultado - 0.706) / 0.001721;

        // Formata o resultado para exibição
        sprintf(str_temp, "Temp: %.2f C", temp);
        // Mostra no display, centralizado na posição x=30, y=30
        escrever_texto(str_temp, 30, 30, true);

        sleep_ms(1000);                         // Aguarda 1 segundo até a próxima leitura
    }
}