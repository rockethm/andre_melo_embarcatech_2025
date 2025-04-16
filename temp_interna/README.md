# Leitor de Temperatura Interna do RP2040

## Descrição
Este projeto demonstra como utilizar o sensor de temperatura interno do chip RP2040 (presente no Raspberry Pi Pico) e exibir as leituras em um display OLED SSD1306. O código lê a temperatura a cada segundo e atualiza o display com o valor em graus Celsius.

## Hardware Necessário
- Raspberry Pi Pico ou outro microcontrolador baseado no RP2040
- Display OLED SSD1306 (128x64 pixels) com interface I2C
- Conexões para I2C (SDA e SCL) entre o Pico e o display

## Conexões
- GPIO 14: SDA (Serial Data Line) para o display OLED
- GPIO 15: SCL (Serial Clock Line) para o display OLED
- O sensor de temperatura é interno, não requer conexões externas

## Como Funciona
1. O código inicializa o display OLED via I2C
2. Ativa o sensor de temperatura interno do RP2040
3. Em loop contínuo:
   - Lê o valor analógico do sensor de temperatura
   - Converte o valor ADC para temperatura em graus Celsius
   - Exibe a temperatura no display OLED
   - Aguarda 1 segundo antes da próxima leitura

## Compilação e Instalação
Para compilar e instalar este projeto usando o SDK do Raspberry Pi Pico:

1. Configure o ambiente de desenvolvimento do Pico SDK
2. Clone ou copie este código para um diretório de projeto
3. Adicione a biblioteca SSD1306 ao seu projeto
4. Compile usando CMake: