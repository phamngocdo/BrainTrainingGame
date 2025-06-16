#include "stm32f4xx.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include "game.h"
#include <stdio.h>

int main(void) {
    // === Khởi tạo các thành phần ===
    GPIO_Init();         // LED + nút nhấn
    UART1_Init(115200);    // UART1 TX trên PB6
    ADC_Init();         // Biến trở trên PA0
    Game_Init();         // Reset game

//    UART1_Printf("== Memory LED Game ==\r\n", "\033[36m");
//    UART1_Printf("Use potentiometer to choose difficulty (1-10)\r\n", "\033[36m");
//    UART1_Printf("Press Button 1 to start\r\n", "\033[36m");

    Game_Loop();

    while (1);
}

