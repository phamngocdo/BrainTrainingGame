#include "timer.h"
#include "gpio.h"

static uint8_t next_led_index = 0;

//// ===== Initialize TIM3 as 1ms base timer =====
//void Timer_Init(void) {
//    // Enable TIM3 clock (bit 1 of APB1ENR)
//    RCC->APB1ENR |= (0b1 << 1);
//
//    TIM3->PSC = 16 - 1;       // 84 MHz / 84 = 1 MHz (1 tick = 1 us)
//    TIM3->ARR = 1000 - 1;     // Default: 1ms * 1000 = 1s
//    TIM3->CNT = 0;            // Reset counter
//
//    TIM3->DIER |= (1 << 0);   // Enable update interrupt
//    TIM3->CR1  |= (1 << 2);   // URS: Only overflow generates interrupt
//}

// ===== Start the timer with given delay (in ms) =====
void Timer_Start(uint32_t ms) {
//    TIM3->ARR  = ms * 1000 - 1; // 1 tick = 1us → ms * 1000
//    TIM3->CNT  = 0;
//    TIM3->SR  &= ~(1 << 0);     // Clear update interrupt flag
//    TIM3->CR1 |= (1 << 0);      // Start timer
	// Enable Timer 3 clock (using RCC->APB1ENR register)
	RCC->APB1ENR |= 1 << 1;

	// Set prescaler to get 1 ms tick (16 MHz / 16000 = 1 kHz)
	TIM3->PSC = 16000 - 1;

	// Set auto-reload value for desired ms delay
	TIM3->ARR = ms - 1;

	// Enable update event generation and counter
	TIM3->CR1 |= 1 << 0;

	// Poll the update event flag
	while (!(TIM3->SR & 0x1)); // Wait until the event update flag (UIF) is set
	TIM3->SR &= ~(1 << 0);
	TIM3->CR1 &= ~(1 << 0);
}

//// ===== Stop the timer =====
//void Timer_Stop(void) {
//    TIM3->CR1 &= ~(1 << 0);     // Disable timer
//}

//// ===== Set which LED will turn on in the next step =====
//void Timer_SetNextLED(uint8_t led) {
//    next_led_index = led;
//}
//
//// ===== TIM3 interrupt handler =====
//void TIM3_IRQHandler(void) {
//    if (TIM3->SR & (1 << 0)) {
//        TIM3->SR &= ~(1 << 0); // Clear update flag
//
//        // Fade in the next LED
//        LED_On(next_led_index);
//
//        // Stop timer so it only triggers once
//        Timer_Stop();
//    }
//}
