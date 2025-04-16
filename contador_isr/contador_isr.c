#include <stdio.h>              // Biblioteca padrão para entrada/saída
#include "pico/stdlib.h"        // Biblioteca padrão do Raspberry Pi Pico
#include "hardware/i2c.h"       // Biblioteca para comunicação I2C
#include "ssd1306/ssd1306.h"    // Biblioteca para controlar display OLED SSD1306

// Definição dos pinos e constantes
#define BUTTON_A 5              // Pino GPIO para o botão A (inicia contagem)
#define BUTTON_B 6              // Pino GPIO para o botão B (contador de cliques)
#define DEBOUNCE_TIME_MS 200    // Tempo de debounce para evitar detecções múltiplas
#define COUNTDOWN_START 9       // Valor inicial da contagem regressiva

// Variáveis globais
ssd1306_t disp;                        // Objeto para controlar o display OLED
static volatile bool button_A_pressed = false;  // Flag para indicar se botão A foi pressionado
static volatile bool button_B_pressed = false;  // Flag para indicar se botão B foi pressionado
static uint8_t button_count = 0;       // Contador de cliques no botão B
static absolute_time_t last_button_time = {0};  // Último tempo registrado de um botão pressionado
static char string_contador[20];       // Buffer para armazenar string do contador e cliques

// Variáveis específicas para a contagem regressiva
static int countdown_value = 0;        // Valor atual da contagem regressiva
static bool countdown_active = false;  // Indica se a contagem está ativa
static absolute_time_t last_second_time = {0};  // Tempo da última atualização da contagem
static bool countdown_finished = false; // Indica se a contagem terminou (chegou a zero)

/**
 * Inicializa o display OLED via I2C
 */
void inicializar_display(void) {
    i2c_init(i2c1, 400000);            // Inicializa I2C1 com velocidade de 400kHz
    gpio_set_function(14, GPIO_FUNC_I2C); // Configura GPIO14 como SDA
    gpio_set_function(15, GPIO_FUNC_I2C); // Configura GPIO15 como SCL
    gpio_pull_up(14);                  // Ativa pull-up no pino SDA
    gpio_pull_up(15);                  // Ativa pull-up no pino SCL
    disp.external_vcc = false;         // Display usa VCC interno
    ssd1306_init(&disp, 128, 64, 0x3C, i2c1); // Inicializa display 128x64 no endereço 0x3C
    ssd1306_clear(&disp);              // Limpa o display
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
 * Função de manipulação de interrupção para os botões
 * Acionada quando qualquer botão configurado é pressionado
 * @param gpio Número do pino GPIO que gerou a interrupção
 * @param evento Tipo de evento que ocorreu (borda de subida/descida)
 */
static void manipulador_interrupcao_gpio(uint gpio, uint32_t evento) {
    absolute_time_t tempo_atual = get_absolute_time();
    
    // Verificar se passou tempo suficiente desde o último pressionamento (debounce)
    if (absolute_time_diff_us(last_button_time, tempo_atual) > DEBOUNCE_TIME_MS * 1000) {
        if (gpio == BUTTON_A) {
            button_A_pressed = true;   // Sinaliza que o botão A foi pressionado
        } else if (gpio == BUTTON_B && (countdown_active && !countdown_finished)) {
            // Registrar clique do Botão B apenas se a contagem estiver ativa e não tiver terminado
            button_B_pressed = true;   // Sinaliza que o botão B foi pressionado
        }
        last_button_time = tempo_atual; // Atualiza o tempo do último pressionamento
    }
}

/**
 * Inicializa todos os componentes do sistema
 */
void srk_init() {
    stdio_init_all();                  // Inicializa stdio (Serial, USB)
    inicializar_display();             // Inicializa o display OLED
    
    // Inicializar botões com pull-up
    gpio_init(BUTTON_A);               // Inicializa pino do botão A
    gpio_set_dir(BUTTON_A, GPIO_IN);   // Define como entrada
    gpio_pull_up(BUTTON_A);            // Ativa resistor de pull-up
    
    gpio_init(BUTTON_B);               // Inicializa pino do botão B
    gpio_set_dir(BUTTON_B, GPIO_IN);   // Define como entrada
    gpio_pull_up(BUTTON_B);            // Ativa resistor de pull-up
    
    // Configurar interrupções para os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &manipulador_interrupcao_gpio);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);
    
    // Exibir mensagem inicial
    escrever_texto("Pressione A para comecar", 10, 5, true);
}

/**
 * Função principal
 */
int main() {
    srk_init();                        // Inicializa os componentes
    
    // Estado inicial
    countdown_value = 0;
    countdown_active = false;
    countdown_finished = true;
    button_count = 0;
    
    // Loop principal
    while (true) {
        // Tratamento do pressionamento do Botão A - Reiniciar contagem
        if (button_A_pressed) {
            countdown_value = COUNTDOWN_START; // Restaura contagem para valor inicial
            countdown_active = true;           // Ativa a contagem
            countdown_finished = false;        // Contagem não está finalizada
            button_count = 0;                  // Zerar contador do botão B
            last_second_time = get_absolute_time(); // Registra tempo atual para contagem
            button_A_pressed = false;          // Limpa a flag do botão
            
            // Atualizar display imediatamente após o reset
            sprintf(string_contador, "T:%d C:%d", countdown_value, button_count);
            escrever_texto(string_contador, 20, 5, true);
        }
        
        // Tratamento do pressionamento do Botão B - Incrementar apenas se a contagem estiver ativa
        if (button_B_pressed && countdown_active && !countdown_finished) {
            button_count++;            // Incrementa contador de cliques
            button_B_pressed = false;  // Limpa a flag do botão
            
            // Atualizar display quando o botão B é pressionado
            sprintf(string_contador, "T:%d C:%d", countdown_value, button_count);
            escrever_texto(string_contador, 20, 5, true);
        } else if (button_B_pressed) {
            // Limpar o flag se a contagem não estiver ativa
            button_B_pressed = false;
        }
        
        // Lógica da contagem regressiva
        if (countdown_active && !countdown_finished) {
            absolute_time_t now = get_absolute_time();
            
            // Verificar se passou um segundo (1.000.000 microssegundos)
            if (absolute_time_diff_us(last_second_time, now) >= 1000000) {
                if (countdown_value > 0) {
                    countdown_value--; // Decrementa o contador
                    last_second_time = now; // Atualiza o tempo da última contagem
                    
                    // Atualizar display a cada segundo da contagem
                    sprintf(string_contador, "T:%d C:%d", countdown_value, button_count);
                    escrever_texto(string_contador, 20, 5, true);
                } 
                
                if (countdown_value == 0) {
                    countdown_active = false;  // Desativa a contagem
                    countdown_finished = true; // Marca a contagem como finalizada
                    
                    // Exibir mensagem final quando a contagem chegar a zero
                    sprintf(string_contador, "ZERO! C:%d", button_count);
                    escrever_texto(string_contador, 20, 5, true);
                }
            }
        }
        
        // Pequeno delay para estabilidade do sistema
        sleep_ms(20);
    }
}