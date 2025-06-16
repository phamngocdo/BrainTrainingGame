#include "timer.h"
#include "gpio.h"

void Timer_Start(uint32_t ms) {
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