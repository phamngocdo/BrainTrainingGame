#include "gpio.h"

// ===== GPIO + PWM + EXTI Initialization =====
void GPIO_Init(void) {
    // 1. Enable clocks for GPIOA, SYSCFG, TIM2
    RCC->AHB1ENR |= (0b1 << 0);         // Enable GPIOA
    RCC->APB2ENR |= (0b1 << 14);        // Enable SYSCFG
    RCC->APB1ENR |= (0b1 << 0);         // Enable TIM2

    // 2. Configure PA0–PA3 as Alternate Function (AF1 = TIM2_CH1–4)
    for (int i = 0; i <= 3; i++) {
        GPIOA->MODER &= ~(0b11 << (i * 2));         // Clear mode bits
        GPIOA->MODER |=  (0b10 << (i * 2));         // Set to AF mode
        GPIOA->AFR[i >> 2] &= ~(0b1111 << ((i % 4) * 4));
        GPIOA->AFR[i >> 2] |=  (0b0001 << ((i % 4) * 4)); // AF1 = TIM2
        GPIOA->OTYPER &= ~(0b1 << i);               // Push-pull
        GPIOA->OSPEEDR |= (0b11 << (i * 2));        // High speed
        GPIOA->PUPDR &= ~(0b11 << (i * 2));         // No pull
    }

    // 3. Configure PA8–PA11 as Input with Pull-up + EXTI
    for (int i = 8; i <= 11; i++) {
        GPIOA->MODER &= ~(0b11 << (i * 2));         // Input mode
        GPIOA->PUPDR &= ~(0b11 << (i * 2));          // Clear pull
        GPIOA->PUPDR |=  (0b01 << (i * 2));          // Pull-up

        // Map EXTI line i to PORTA (0b0000)
        uint8_t exti_index = i / 4;
        uint8_t offset = (i % 4) * 4;
        SYSCFG->EXTICR[exti_index] &= ~(0b1111 << offset);  // Clear to select PORTA

        EXTI->IMR  |= (1 << i);   // Unmask EXTI line i
        EXTI->RTSR |= (1 << i);   // Rising edge trigger
    }

    // 4. Enable EXTI interrupts in NVIC for lines 8–11
    NVIC_EnableIRQ(EXTI9_5_IRQn);   // PA8, PA9
    NVIC_EnableIRQ(EXTI15_10_IRQn); // PA10, PA11

    // 5. Configure TIM2 for PWM (frequency = 10kHz)
    TIM2->PSC = 84 - 1;         // Timer clock = 1 MHz
    TIM2->ARR = 100 - 1;        // Period = 100 → 10 kHz

    // Enable PWM mode 1 with preload for all 4 channels
    TIM2->CCMR1 |= (0b110 << 4)  | (1 << 3);    // CH1
    TIM2->CCMR1 |= (0b110 << 12) | (1 << 11);   // CH2
    TIM2->CCMR2 |= (0b110 << 4)  | (1 << 3);    // CH3
    TIM2->CCMR2 |= (0b110 << 12) | (1 << 11);   // CH4

    // Enable output on all 4 PWM channels
    TIM2->CCER |= (1 << 0) | (1 << 4) | (1 << 8) | (1 << 12);

    TIM2->CR1 |= (1 << 7);      // Enable Auto-reload preload
    TIM2->CR1 |= (1 << 0);      // Start timer
}

// ===== Set PWM duty cycle (0–100%) =====
void PWM_SetDuty(uint8_t index, uint8_t duty_percent) {
    if (duty_percent > 100) duty_percent = 100;
    uint32_t value = (TIM2->ARR + 1) * duty_percent / 100;

    switch (index) {
        case 0: TIM2->CCR1 = value; break;
        case 1: TIM2->CCR2 = value; break;
        case 2: TIM2->CCR3 = value; break;
        case 3: TIM2->CCR4 = value; break;
    }
}

// ===== Fade out LED gradually using software delay =====
void PWM_FadeOut(uint8_t index) {
    for (int i = 100; i >= 0; i -= 5) {
        PWM_SetDuty(index, i);
        for (volatile int d = 0; d < 30000; d++); // Software delay
    }
}

// ===== LED Control (100% = fully ON) =====
void LED_On(uint8_t index) {
    PWM_SetDuty(index, 100);
}

void LED_Off(uint8_t index) {
    PWM_FadeOut(index);
    PWM_SetDuty(index, 0);
}

void LED_Toggle(uint8_t index) {
    uint32_t current = 0;
    switch (index) {
        case 0: current = TIM2->CCR1; break;
        case 1: current = TIM2->CCR2; break;
        case 2: current = TIM2->CCR3; break;
        case 3: current = TIM2->CCR4; break;
    }
    if (current == 0)
        LED_On(index);
    else
        LED_Off(index);
}


