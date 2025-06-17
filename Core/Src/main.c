#include "stm32f4xx.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include "i2c.h"
#include "game.h"
#include <stdio.h>

int main(void) {
    GPIO_Init();
    UART1_Init(115200);
    ADC_Init();
    I2C1_Init();
    Game_Init();

    Timer_Start(1000);
    SSD1306_Init();
    SSD1306_Clear();
   UART1_Printf("== Welcome to BrainTraining ==\r\n", "\033[36m");
   UART1_Printf("Use potentiometer to choose difficulty (1-10)\r\n", "\033[36m");

   uint8_t char_to_index(char c) {
      	     if (c >= 'A' && c <= 'Z') return c - 'A' + 33;
      	     if (c >= 'a' && c <= 'z') return c - 'a' + 33;
      	     if (c >= '0' && c <= '9') return c - '0' + 16;
      	     if (c == ' ') return 0;
      	     if (c == '(') return 28;  // Assuming '(' is at index 28 in your font array
      	     if (c == ')') return 30;  // Assuming ')' is at index 29
      	     if (c == '-') return 13;  // Assuming '-' is at index 16 (same as some numbers)
      	     return 0; // default to space
      	 }
   const uint8_t *line1[] = {
   	        Font5x7[char_to_index('W')],
   	        Font5x7[char_to_index('E')],
   	        Font5x7[char_to_index('L')],
   	        Font5x7[char_to_index('C')],
   	        Font5x7[char_to_index('O')],
   	        Font5x7[char_to_index('M')],
   	        Font5x7[char_to_index('E')],
   	        Font5x7[char_to_index(' ')],
   	        Font5x7[char_to_index('T')],
   	        Font5x7[char_to_index('O')]
   	    };

   	    // Dòng 2: "BRAINTRAINING"
   	    const uint8_t *line2[] = {
   	        Font5x7[char_to_index('B')],
   	        Font5x7[char_to_index('R')],
   	        Font5x7[char_to_index('A')],
   	        Font5x7[char_to_index('I')],
   	        Font5x7[char_to_index('N')],
   	        Font5x7[char_to_index('T')],
   	        Font5x7[char_to_index('R')],
   	        Font5x7[char_to_index('A')],
   	        Font5x7[char_to_index('I')],
   	        Font5x7[char_to_index('N')],
   	        Font5x7[char_to_index('I')],
   	        Font5x7[char_to_index('N')],
   	        Font5x7[char_to_index('G')]
   	    };

   	    // Hiển thị dòng 1 (WELCOME TO)
   	    uint8_t total_width1 = 10 * 6; // 10 ký tự
   	    uint8_t start_col1 = (128 - total_width1) / 2;
   	    uint8_t col1 = start_col1;
   	    for (int i = 0; i < 10; i++) {
   	        SSD1306_DrawChar(col1, 2, line1[i]); // Page 2 (hàng 16-23)
   	        col1 += 6;
   	    }

   	    // Hiển thị dòng 2 (BRAINTRAINING)
   	    uint8_t total_width2 = 13 * 6; // 13 ký tự
   	    uint8_t start_col2 = (128 - total_width2) / 2;
   	    uint8_t col2 = start_col2;
   	    for (int i = 0; i < 13; i++) {
   	        SSD1306_DrawChar(col2, 4, line2[i]); // Page 4 (hàng 32-39)
   	        col2 += 6;
   	    }

//   	    Timer_Start(50000);
   	 Timer_Start(5000);
   	 SSD1306_Clear();
   	 // Helper function (keep this in your i2c.c file)


   	 const uint8_t *message[] = {
   	     Font5x7[char_to_index('C')],
   	     Font5x7[char_to_index('h')],
   	     Font5x7[char_to_index('o')],
   	     Font5x7[char_to_index('o')],
   	     Font5x7[char_to_index('s')],
   	     Font5x7[char_to_index('e')],
   	     Font5x7[char_to_index(' ')],
   	     Font5x7[char_to_index('D')],
   	     Font5x7[char_to_index('i')],
   	     Font5x7[char_to_index('f')],
   	     Font5x7[char_to_index('f')],
   	     Font5x7[char_to_index('i')],
   	     Font5x7[char_to_index('c')],
   	     Font5x7[char_to_index('u')],
   	     Font5x7[char_to_index('l')],
   	     Font5x7[char_to_index('t')],
   	     Font5x7[char_to_index('y')],
   	     Font5x7[char_to_index(' ')],
   	     Font5x7[char_to_index('(')],
   	     Font5x7[char_to_index('1')],
   	     Font5x7[char_to_index('-')],
   	     Font5x7[char_to_index('1')],
   	     Font5x7[char_to_index('0')],
   	     Font5x7[char_to_index(')')]
   	 };

   	 // Split into two lines
   	 uint8_t line3_length = 6;  // "Choose" (6 letters)
   	 uint8_t line4_length = 18; // "Difficulty (1-10)" (18 characters)

   	 // Display first line ("Choose")
   	 uint8_t start_col3 = (128 - (line3_length * 6)) / 2;
   	 uint8_t col3 = start_col3;
   	 for (int i = 0; i < line3_length; i++) {
   	     SSD1306_DrawChar(col3, 2, message[i]); // Page 2
   	     col3 += 6;
   	 }

   	 // Display second line ("Difficulty (1-10)")
   	 uint8_t start_col4 = (128 - (line4_length * 6)) / 2;
   	 uint8_t col4 = start_col4;
   	 for (int i = 0; i < line4_length; i++) {
   	     SSD1306_DrawChar(col4, 4, message[line3_length + i]); // Page 4
   	     col4 += 6;
   	 }

    Game_Loop();

    while (1);
}

