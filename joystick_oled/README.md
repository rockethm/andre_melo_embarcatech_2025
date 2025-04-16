# Leitor de Joystick com Display OLED

## Descrição
Este projeto demonstra a leitura dos valores de um joystick analógico e exibe os resultados em um display OLED SSD1306, utilizando um Raspberry Pi Pico.

## Hardware Necessário
- Raspberry Pi Pico
- Display OLED SSD1306 (128x64 pixels)
- Joystick analógico
- Conexões I2C (SDA e SCL) para o display
- Conexões ADC para o joystick

## Conexões
- **Display OLED:**
  - SDA: GPIO 14
  - SCL: GPIO 15
  - VCC: 3.3V
  - GND: GND

- **Joystick:**
  - Eixo X: GPIO 26 (ADC0)
  - Eixo Y: GPIO 27 (ADC1)
  - VCC: 3.3V
  - GND: GND

## Funcionalidades
- Leitura dos valores analógicos dos eixos X e Y do joystick
- Exibição em tempo real dos valores no display OLED
- Atualização a cada 500ms

## Compilação e Execução
1. Certifique-se de ter o SDK do Raspberry Pi Pico instalado
2. Clone este repositório
3. Compile o código:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
4. Copie o arquivo .uf2 gerado para o Raspberry Pi Pico

## Observações
- Os valores exibidos estão na faixa de 0-4095 (resolução do ADC de 12 bits)
- O código inclui estruturas para funcionalidades adicionais como contagem regressiva, que podem ser expandidas em versões futuras