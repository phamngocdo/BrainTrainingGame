#include "game.h"
#include "timer.h"
#include "gpio.h"
#include "uart.h"
#include "adc.h"
#include <stdio.h>

#define MAX_LED_PER_LEVEL 10

static uint8_t current_level = 0;
static uint8_t current_index = 0;
static uint8_t difficulty = 5;
static uint8_t prev_difficulty = 0;
static uint8_t temp_difficulty = 5;
static uint8_t current_score = 0;

typedef enum {
    STATE_WAIT_DIFFICULTY,
	STATE_START,
	STATE_SHOWLEVEL,
    STATE_PLAYING,
	STATE_WAIT_SET_NAME,
    STATE_WAIT_RESTART
} GameState;

volatile GameState current_state = STATE_WAIT_DIFFICULTY;

// Delay time (ms) for each difficulty level
static const uint16_t delay_table[MAX_DIFFICULTY] = {
    2000, 700, 600, 500, 400, 350, 300, 250, 200, 150
};

// LED sequences for each level
static const uint8_t led_sequence[MAX_LEVEL][MAX_LED_PER_LEVEL] = {
    {0, 1, 2, 2}, {1,2}, {2,3,0}, {3,0,1,2}, {0,2,1,3,1},
    {3,1,0,2,1,0}, {2,0,3,1,2,3,0},
    {1,3,2,0,1,0,2,3}, {0,1,2,3,0,1,2,3,1},
    {3,2,1,0,3,2,1,0,1,2},
    {0,3}, {1,0,2}, {2,1,0,3}, {3,1,2,0,1},
    {2,0,3,1,0,3}, {1,2,3,0,1,2,3},
    {0,1,2,3,1,0,2,3}, {3,0,1,2,0,1,3,2},
    {1,2,3,0,2,1,0,3}, {0,1,2,3,0,1,2,3,1,2}
};

// Number of LEDs in each level
static const uint8_t led_count[MAX_LEVEL] = {
    4,2,3,4,5,6,7,8,9,10, 2,3,4,5,6,7,8,8,8,10
};

// === Game Initialization ===
void Game_Init(void) {
    UART1_Printf("[Game] Initialized", "\033[36m");
    current_level = 0;
    current_index = 0;
    current_state = STATE_WAIT_DIFFICULTY;
}

// === Set difficulty (1-10) ===
void SetDifficulty(uint8_t d) {
    if (d < 1) d = 1;
    if (d > MAX_DIFFICULTY) d = MAX_DIFFICULTY;
    difficulty = d;

}


// === Start game ===
void Start(void) {
    UART1_Printf("[Game] Start", "\033[32m");
    current_level = 0;
    current_index = 0;
    for (volatile int i = 0; i < 3000000; i++);
}

// === Handle button press ===
void ChooseButtonWhenPlay(uint8_t button_index) {
    // Chống dội nút đơn giản
      // Delay khoảng vài ms

    // In nút được bấm để debug
    char dbg[32];
    sprintf(dbg, "[Button] Pressed: %d", button_index);
    UART1_Printf(dbg, "\033[33m");

    // Lấy LED mong đợi tiếp theo
    uint8_t expected = led_sequence[current_level][current_index];

    if (button_index == expected) {
        current_index++;

        // Nếu vượt quá số LED của level hiện tại
        if (current_index >= led_count[current_level]) {
            current_level++;
            current_index = 0;

            if (current_level >= MAX_LEVEL) {
                Game_Win();
            } else {
            	current_score += 1;
                UART1_Printf("[Game] Level Up!", "\033[36m");
                for (volatile int i = 0; i < 3000000; i++);
                current_state = STATE_SHOWLEVEL;  // Cho phép hiển thị LED
            }
        }

    } else {
        Game_Over();
    }
}


// === Game win ===
void Game_Win(void) {
    UART1_Printf("[Game] You Win!", "\033[32m");
    for (uint32_t i = 0; i < 300000; i++);
}

// === Game over ===
void Game_Over(void) {
    UART1_Printf("[Game] Game Over!", "\033[31m");
    char buffer[32];
    sprintf(buffer, "Your score is: %u", current_score);
    UART1_Printf(buffer, "\033[36m");
    for (uint32_t i = 0; i < 300000; i++);

    current_state = STATE_WAIT_RESTART;
}

// === Show next LED (called by timer) ===
void Game_LEDNextLevel() {
    UART1_Printf("[Game] Show Level Sequence", "\033[36m");

    for (uint8_t i = 0; i < led_count[current_level]; i++) {
        uint8_t led = led_sequence[current_level][i];
        LED_On(led);                             // Bật LED tương ứng
        LED_Off(led);                            // Tắt tất cả LED trước khi hiện tiếp
        Timer_Start(delay_table[difficulty - 1]); // Tự động show LED
    }

    UART1_Printf("[Game] Your turn!", "\033[33m");
    current_index = 0;  // Reset index để người chơi nhập lại
}

void Game_Loop() {
    while (1) {
        // === Chờ chọn độ khó ===
        if (current_state == STATE_WAIT_DIFFICULTY) {
            uint16_t adc_val = ADC_Read();  // 0 - 4095
            uint8_t new_difficulty = (adc_val * 10) / 4096 + 1;
            if (new_difficulty > 10) new_difficulty = 10;

            if (new_difficulty != prev_difficulty) {
                prev_difficulty = new_difficulty;
                temp_difficulty = new_difficulty;

                char buffer[32];
                sprintf(buffer, "Set difficulty: %u", new_difficulty);
                UART1_Printf(buffer, "\033[33m");
                UART1_Printf("Press button 1 to start", "\033[36m");
            }
        }

        // === Đang chơi game ===
        else if (current_state == STATE_PLAYING) {
            // Có thể thêm logic chờ hiệu ứng, hoặc hiện level
            char buffer[32];
            sprintf(buffer, "Playing level: %u", current_level + 1);
            UART1_Printf(buffer, "\033[36m");


            while(current_state == STATE_PLAYING);
        }

        else if (current_state == STATE_SHOWLEVEL) {
        	Game_LEDNextLevel();
        	current_state = STATE_PLAYING;
        }

        // === Hiển thị điểm & chờ restart ===
        else if (current_state == STATE_WAIT_RESTART) {
            UART1_Printf("Press button 1 to play again", "\033[36m");
            while(current_state == STATE_WAIT_RESTART);
            // Đợi người dùng nhấn để reset trong EXTI
        }

        // === Chờ hiển thị hiệu ứng Start xong thì về PLAYING ===
        else if (current_state == STATE_START) {
            // Đợi hiệu ứng LED được Timer phát xong
            // Mình nên để Timer_Stop ở hàm Game_ShowNextLED khi đủ LED
            // Khi LED xong thì tự chuyển sang PLAYING lại
        }
    }
}



void EXTI9_5_IRQHandler(void) {

    if (EXTI->PR & (1 << 9)) {

        if (current_state == STATE_PLAYING) {
            ChooseButtonWhenPlay(1);
        }
        for (volatile int i = 0; i < 100000; i++);
        EXTI->PR |= (1 << 9);
    }
    if (EXTI->PR & (1 << 8)) {

        switch (current_state) {
            case STATE_WAIT_DIFFICULTY:
                 SetDifficulty(temp_difficulty);
                 Start();
                 current_state = STATE_SHOWLEVEL;
                 break;

             case STATE_WAIT_RESTART:
            	 current_level = 0;
            	 current_index = 0;
            	 current_score = 0;
                 current_state = STATE_WAIT_DIFFICULTY;
                 char buffer[32];
                 sprintf(buffer, "Set difficulty: %u", temp_difficulty);
                 UART1_Printf(buffer, "\033[33m");
                 UART1_Printf("Press button 1 to start", "\033[36m");

                 break;



             case STATE_PLAYING:
                 ChooseButtonWhenPlay(0);
                 break;

             default:
                 break;
        }
        for (volatile int i = 0; i < 100000; i++);
        EXTI->PR |= (1 << 8);
    }

}

void EXTI15_10_IRQHandler(void) {


    if (EXTI->PR & (1 << 10)) {

        if (current_state == STATE_PLAYING) {
            ChooseButtonWhenPlay(2);
        }
        for (volatile int i = 0; i < 100000; i++);
        EXTI->PR |= (1 << 10);
    }
    if (EXTI->PR & (1 << 11)) {

        if (current_state == STATE_PLAYING) {
            ChooseButtonWhenPlay(3);
        }
        for (volatile int i = 0; i < 100000; i++);
        EXTI->PR |= (1 << 11);
    }

}



