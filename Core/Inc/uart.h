#ifndef UART_H
#define UART_H

#include "stm32f4xx.h"

void UART1_Init(uint32_t baudrate);
void UART1_SendChar(char c);
void UART1_SendString(const char *str);
void UART1_Printf(const char* msg, const char* color_code);

#endif
