#ifndef GPIO_H
#define GPIO_H

#include "stm32f4xx.h"

#define LED1_PIN        0
#define LED2_PIN        1
#define LED3_PIN        2
#define LED4_PIN        3

#define BTN1_PIN        0
#define BTN2_PIN        1
#define BTN3_PIN        2
#define BTN4_PIN        3

void GPIO_Init(void);
void PWM_SetDuty(uint8_t index, uint8_t duty_percent);
void PWM_FadeOut(uint8_t index);
void LED_On(uint8_t index);
void LED_Off(uint8_t index);
void LED_Toggle(uint8_t index);

#endif
