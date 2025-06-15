#include "adc.h"

void ADC_Init(void) {
    // 1. Enable clock for GPIOA and ADC1
    RCC->AHB1ENR |= (0b1 << 0);     // Enable GPIOA
    RCC->APB2ENR |= (0b1 << 8);     // Enable ADC1

    // 2. Set PA5 (ADC1_IN5) as analog input
    GPIOA->MODER |=  (0b11 << (5 * 2));  // Analog mode
    GPIOA->PUPDR &= ~(0b11 << (5 * 2));  // No pull

    // 3. Configure ADC
    ADC->CCR &= ~(0b11 << 16);     // ADC prescaler: PCLK2 / 2
    ADC1->CR2 = 0;                 // Disable before config

    ADC1->SQR3 = 5;                // Channel 5 (PA5)
    ADC1->SMPR2 |= (0b111 << (3 * 5)); // Max sample time for ch5

    ADC1->CR2 |= (1 << 0);         // Enable ADC1
}

uint16_t ADC_Read(void) {
    ADC1->CR2 |= (1 << 30);                // Start conversion
    while (!(ADC1->SR & (1 << 1)));        // Wait for EOC
    return (uint16_t)(ADC1->DR);           // Read result
}
