#ifndef GAME_H
#define GAME_H

#include "stm32f4xx.h"
#include <stdint.h>

#define MAX_DIFFICULTY     10
#define MAX_LEVEL          20

void Game_Init(void);
void SetDifficulty(uint8_t difficulty);
void Start(void);
void ChooseButtonWhenPlay(uint8_t button_index);
void Game_Win(void);
void Game_Over(void);
void DisplayTopGamer(uint8_t score);
void Game_ShowNextLED(void);

#endif
