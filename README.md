### Link Do Vídeo: https://www.youtube.com/shorts/BRcyTP7d9yI
# Projeto de Controle de Matriz de LEDs WS2812 com Interrupções

## 🚀 Funcionalidades

- **Piscar LED RGB Vermelho:** Pisca continuamente a 5 Hz (5 vezes por segundo).
- **Contador com Botões:**
  - **Botão A:** Incrementa um contador (0 a 9).
  - **Botão B:** Decrementa o contador (9 a 0).
- **Display de Números na Matriz 5x5:**
  - Mostra números de 0 a 9 em estilo "display digital".
  - Efeitos visuais com LEDs WS2812 (azul padrão).
- **Debouncing via Software:** Filtra ruídos mecânicos dos botões.

## 📋 Pré-requisitos

- **Hardware:**
  - Placa BitDogLab com Raspberry Pi Pico.
  - Matriz de LEDs WS2812 5x5.
  - Botões conectados às GPIOs 5 e 6.
- **Software:**
  - SDK do Raspberry Pi Pico (`pico-sdk`).
  - Toolchain para compilação (CMake, GCC, etc.).

## 🎮 Uso
**Funcionamento:**

- Ao ligar a placa, o LED RGB vermelho piscará a 5 Hz.
-Pressione Botão A para aumentar o número exibido na matriz.
- Pressione Botão B para diminuir o número.
  Exemplo:
-Contador = 3 → Exibe o número 3 na matriz em azul.
-Contador = 9 → Exibe o número 9.

## Principais Funções:
- button_handler: Trata interrupções dos botões com debouncing.

- atualizar_matriz: Atualiza o buffer de LEDs com base no contador.

- set_one_led: Renderiza a matriz com a cor especificada (azul).

## 🛠️ Tecnologias Utilizadas
- Linguagem: C/C++ (com suporte ao SDK do Pico).
## Hardware:
- RP2040 (Dual-core ARM Cortex-M0+).
- Protocolo WS2812 (NeoPixel) para LEDs endereçáveis.

## DESEVOLVIDO POR CÉSAR REBOUÇAS COSTA
