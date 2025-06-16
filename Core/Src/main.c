#include "stm32f4xx.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include "game.h"
#include <stdio.h>

int main(void) {
    GPIO_Init();
    UART1_Init(115200);
    ADC_Init();
    Game_Init();

   UART1_Printf("== Welcome to BrainTraining ==\r\n", "\033[36m");
   UART1_Printf("Use potentiometer to choose difficulty (1-10)\r\n", "\033[36m");

    Game_Loop();

    while (1);
}

