#include "uart.h"

// ===== Initialize UART1: PB6 = TX, PB7 = RX =====
void UART1_Init(uint32_t baudrate) {
    // 1. Enable clocks: GPIOB + USART1
    RCC->AHB1ENR |= (1 << 1);      // Enable GPIOB
    RCC->APB2ENR |= (1 << 4);      // Enable USART1

    // 2. Configure PB6 (TX) and PB7 (RX) to Alternate Function AF7 (USART1)
    GPIOB->MODER &= ~((0b11 << (6 * 2)) | (0b11 << (7 * 2)));   // Clear
    GPIOB->MODER |=  ((0b10 << (6 * 2)) | (0b10 << (7 * 2)));   // AF mode
    GPIOB->AFR[0] &= ~((0b1111 << (6 * 4)) | (0b1111 << (7 * 4)));
    GPIOB->AFR[0] |=  ((0b0111 << (6 * 4)) | (0b0111 << (7 * 4))); // AF7

    // 3. Configure USART1 baudrate
    // PCLK2 = 84 MHz → USARTDIV = 84MHz / baudrate
    USART1->BRR = 16000000 / baudrate;

    // 4. Enable USART1: TE, RE, UE
    USART1->CR1 = (1 << 13) | (1 << 3) | (1 << 2); // UE, TE, RE
}

// ===== Send a character via UART1 =====
void UART1_SendChar(char c) {
    while (!(USART1->SR & (1 << 7))); // Wait until TXE = 1 (empty)
    USART1->DR = c;
}

// ===== Send a null-terminated string via UART1 =====
void UART1_SendString(const char *str) {
    while (*str) {
        UART1_SendChar(*str++);
    }
}

// === Helper: Send message with ANSI color ===
void UART1_Printf(const char* msg, const char* color_code) {
    UART1_SendString(color_code);
    UART1_SendString(msg);
    UART1_SendString("\033[0m\r\n"); // Reset color
}
