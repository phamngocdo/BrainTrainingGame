#include "timer.h"
#include "gpio.h"

static uint8_t next_led_index = 0;

// ===== Initialize TIM3 as 1ms base timer =====
void Timer_Init(void) {
    // Enable TIM3 clock (bit 1 of APB1ENR)
    RCC->APB1ENR |= (0b1 << 1);

    TIM3->PSC = 84 - 1;       // 84 MHz / 84 = 1 MHz (1 tick = 1 us)
    TIM3->ARR = 1000 - 1;     // Default: 1ms * 1000 = 1s
    TIM3->CNT = 0;            // Reset counter

    TIM3->DIER |= (1 << 0);   // Enable update interrupt
    TIM3->CR1  |= (1 << 2);   // URS: Only overflow generates interrupt

    NVIC_EnableIRQ(TIM3_IRQn); // Enable TIM3 interrupt
}

// ===== Start the timer with given delay (in ms) =====
void Timer_Start(uint32_t ms) {
    TIM3->ARR  = ms * 1000 - 1; // 1 tick = 1us → ms * 1000
    TIM3->CNT  = 0;
    TIM3->SR  &= ~(1 << 0);     // Clear update interrupt flag
    TIM3->CR1 |= (1 << 0);      // Start timer
}

// ===== Stop the timer =====
void Timer_Stop(void) {
    TIM3->CR1 &= ~(1 << 0);     // Disable timer
}

// ===== Set which LED will turn on in the next step =====
void Timer_SetNextLED(uint8_t led) {
    next_led_index = led;
}

// ===== TIM3 interrupt handler =====
void TIM3_IRQHandler(void) {
    if (TIM3->SR & (1 << 0)) {
        TIM3->SR &= ~(1 << 0); // Clear update flag

        // Fade in the next LED
        LED_On(next_led_index);

        // Stop timer so it only triggers once
        Timer_Stop();
    }
}
