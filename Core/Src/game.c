#include "game.h"
#include "timer.h"
#include "gpio.h"
#include "uart.h"

#define MAX_LED_PER_LEVEL 10

static uint8_t current_level = 0;
static uint8_t current_index = 0;
static uint8_t difficulty = 5;

typedef enum {
    STATE_WAIT_DIFFICULTY,
	STATE_START,
    STATE_PLAYING,
	STATE_WAIT_SET_NAME,
    STATE_WAIT_RESTART
} GameState;

volatile GameState current_state = STATE_WAIT_DIFFICULTY;
volatile uint8_t temp_difficulty = 5;


// Delay time (ms) for each difficulty level
static const uint16_t delay_table[MAX_DIFFICULTY] = {
    800, 700, 600, 500, 400, 350, 300, 250, 200, 150
};

// LED sequences for each level
static const uint8_t led_sequence[MAX_LEVEL][MAX_LED_PER_LEVEL] = {
    {0}, {1,2}, {2,3,0}, {3,0,1,2}, {0,2,1,3,1},
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
    1,2,3,4,5,6,7,8,9,10, 2,3,4,5,6,7,8,8,8,10
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

    Timer_SetDelay(delay_table[d-1]);

    char buffer[32];
    sprintf(buffer, "Set difficulty: %u", difficulty);
    UART1_Printf(buffer, "\033[33m");

    UART1_Printf("Press button 1 to start", "\033[36m");

    current_state = STATE_START;
}


// === Start game ===
void Start(void) {
    UART1_Printf("[Game] Start", "\033[32m");
    current_level = 0;
    current_index = 0;
    for (uint32_t i = 0; i < 300000; i++);
    current_state = STATE_PLAYING;
}

// === Handle button press ===
void ChooseButtonWhenPlay(uint8_t button_index){
    uint8_t expected = led_sequence[current_level][current_index];
    if (button_index == expected) {
        current_index++;
        if (current_index >= led_count[current_level]) {
            current_level++;
            current_index = 0;
            if (current_level >= MAX_LEVEL) {
                Game_Win();
            } else {
                UART1_Printf("[Game] Level Up!", "\033[36m");
                Timer_Start();
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
    for (uint32_t i = 0; i < 300000; i++);

}

// === Display top gamer score ===
void DisplayTopGamer(uint8_t score) {
    char buffer[32];
    sprintf(buffer, "Top Score: %u", score);
    UART1_Printf(buffer, "\033[35m");
    UART1_Printf("Press button 1 to start", "\033[36m");
    state = STATE_WAIT_RESTART;
}

// === Show next LED (called by timer) ===
void Game_ShowNextLED(void) {
    if (current_index < led_count[current_level]) {
        uint8_t led = led_sequence[current_level][current_index];
        LED_On(led);
        current_index++;
    } else {
        current_index = 0;
        Timer_Stop();
    }
}

void EXTI9_5_IRQHandler(void) {
    if (EXTI->PR & (1 << 9)) {
        EXTI->PR |= (1 << 9);
        ChooseButtonWhenPlay(1);
    }
    if (EXTI->PR & (1 << 8)) {
        EXTI->PR |= (1 << 8);
        switch (current_state) {
            case STATE_WAIT_DIFFICULTY:
                 SetDifficulty(temp_difficulty);
                 Start();
                 current_state = STATE_PLAYING;
                  break;

             case STATE_WAIT_RESTART:
                 Game_Init();     // reset
                 current_state = STATE_WAIT_DIFFICULTY;
                 break;

             case STATE_PLAYING:
                 ChooseButtonWhenPlay(0);
                 break;

             default:
                 break;
        }
    }
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR & (1 << 10)) {
        EXTI->PR |= (1 << 10);
        ChooseButtonWhenPlay(2);
    }
    if (EXTI->PR & (1 << 11)) {
        EXTI->PR |= (1 << 11);
        ChooseButtonWhenPlay(3);
    }
}
