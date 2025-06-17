/*
 * i2c.h
 *
 *  Created on: Jun 16, 2025
 *      Author: Buh
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "stm32f4xx.h"
#include <stdint.h>

void I2C1_Init(void);
void SSD1306_Init(void);
void SSD1306_Clear(void);
extern const uint8_t Font5x7[][5];
void SSD1306_Send(uint8_t control, uint8_t *data, uint16_t size);
void SSD1306_DrawChar(uint8_t col, uint8_t page, const uint8_t *bitmap);


#endif /* INC_I2C_H_ */
