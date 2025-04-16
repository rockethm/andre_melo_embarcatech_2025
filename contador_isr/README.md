# Contador com ISR para Raspberry Pi Pico

## Descrição
Este projeto implementa um contador regressivo com contagem de cliques usando interrupções (ISR - Interrupt Service Routine) em um Raspberry Pi Pico. Um display OLED mostra uma contagem regressiva que começa quando o botão A é pressionado. Durante a contagem, o usuário pode pressionar o botão B para incrementar um contador de cliques. O número total de cliques é exibido ao final da contagem.

## Hardware Necessário
- Raspberry Pi Pico
- Display OLED SSD1306 (128x64)
- Dois botões momentâneos (pull-up)
- Jumpers e cabos de conexão

## Conexões
- **Display OLED:**
  - SDA: GPIO 14
  - SCL: GPIO 15
  - VCC: 3.3V
  - GND: GND

- **Botões:**
  - Botão A: GPIO 5 (configurado com pull-up interno)
  - Botão B: GPIO 6 (configurado com pull-up interno)
  - Ambos botões devem ter o outro terminal conectado ao GND

## Funcionalidades
- Pressionar o Botão A inicia/reinicia uma contagem regressiva de 9 até 0
- Durante a contagem ativa, o Botão B incrementa um contador de cliques
- O display OLED mostra o tempo restante (T) e o número de cliques (C)
- Ao finalizar a contagem, exibe "ZERO!" e o número final de cliques

## Técnicas Implementadas
- Interrupções de GPIO para detecção de botões pressionados (edge-falling)
- Debounce por software para evitar múltiplas detecções
- Temporização precisa usando o relógio interno do Pico
- Comunicação I2C com display OLED

## Compilação e Execução
1. Configure o SDK do Raspberry Pi Pico em seu sistema
2. Clone este repositório
3. Compile o código:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
4. Conecte seu Raspberry Pi Pico enquanto pressiona o botão BOOTSEL
5. Copie o arquivo UF2 gerado para o drive que aparecer
6. O programa iniciará automaticamente assim que a cópia for concluída

## Observações

- O sistema implementa uma técnica de debounce de 200ms para evitar detecções falsas
- O botão B só incrementa o contador durante a contagem regressiva ativa
- A contagem regressiva ocorre em intervalos precisos de 1 segundo