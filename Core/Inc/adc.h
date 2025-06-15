#ifndef ADC_H
#define ADC_H

#include "stm32f4xx.h"

typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD
} Difficulty;

void ADC_Init(void);
uint16_t ADC_Read(void);  // Return 12-bit result (0–4095)
Difficulty GetDifficulty(void);

#endif
