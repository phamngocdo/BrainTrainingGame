#ifndef TIMER_H
#define TIMER_H

#include "stm32f4xx.h"
#include <stdint.h>

void Timer_Init(void);
void Timer_Start(uint32_t ms);
void Timer_Stop(void);
void Timer_SetNextLED(uint8_t led);

#endif
