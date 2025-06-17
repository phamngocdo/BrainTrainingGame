#include "game.h"
#include "timer.h"
#include "gpio.h"
#include "uart.h"
#include "adc.h"
#include "i2c.h"
#include <stdio.h>

static uint8_t char_to_index(char c) {
                      	     if (c >= 'A' && c <= 'Z') return c - 'A' + 33;
                      	     if (c >= 'a' && c <= 'z') return c - 'a' + 33;
                      	     if (c >= '0' && c <= '9') return c - '0' + 16;
                      	     if (c == ' ') return 0;
                      	     if (c == '(') return 28;  // Assuming '(' is at index 28 in your font array
                      	     if (c == ')') return 30;  // Assuming ')' is at index 29
                      	     if (c == '-') return 13;  // Assuming '-' is at index 16 (same as some numbers)
                      	     if (c == '!') return 1;
                      	     if (c == ':') return 26;
                      	     return 0; // default to space
                      	 }
                // Hàm chuyển số thành ký tự font tương ứng
static uint8_t num_to_font_index(uint8_t num) {
                    if (num >= 0 && num <= 9) {
                        return num + 16; // Vị trí số trong font (0=16, 1=17,...9=25)
                    }
                    return 0; // Mặc định là space nếu không hợp lệ
                }

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
    2000, 1800, 1600, 1400, 1200, 1000, 800, 600, 400, 200
};

// LED sequences for each level
static const uint8_t led_sequence[MAX_LEVEL][MAX_LED_PER_LEVEL] = {
    {3, 3, 3},
    {0, 0, 1, 1},
    {0, 1, 2, 3},
    {3, 2, 1, 3},
    {2, 1, 0, 0},
    {3, 2, 1, 3},
    {0, 1, 1, 2, 2},
    {0, 1, 2, 3, 0},
    {1, 2, 1, 2, 3, 3},
    {1, 3, 2, 0, 1, 3, 3},
    {3, 1, 0, 2, 2, 3, 0, 1},
    {0, 0, 1, 3, 3, 0, 1, 3, 2},
    {1, 2, 2, 1, 0, 1, 2, 3, 0, 1},
    {1, 0, 3, 1, 0, 3, 1, 2, 1, 0, 3, 2},
    {1, 1, 0, 3, 1, 2, 0, 2, 3, 2, 0, 1, 0}
};

// Number of LEDs in each level
static const uint8_t led_count[MAX_LEVEL] = {
    3, 4, 4, 4, 4, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12
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
    SSD1306_Clear();
    const uint8_t *start_message[] = {
        Font5x7[char_to_index('S')],
        Font5x7[char_to_index('t')],
        Font5x7[char_to_index('a')],
        Font5x7[char_to_index('r')],
        Font5x7[char_to_index('t')]
    };

    // Display "Start" (centered)
    uint8_t start_length = 5; // 5 letters in "Start"
    uint8_t start_col = (128 - (start_length * 6)) / 2;
    uint8_t current_col = start_col;

    for (int i = 0; i < start_length; i++) {
        SSD1306_DrawChar(current_col, 2, start_message[i]); // Display on page 2
        current_col += 6;
    }
    for (volatile int i = 0; i < 3000000; i++);
}

// === Handle button press ===
void ChooseButtonWhenPlay(uint8_t button_index) {
    char dbg[32];
    sprintf(dbg, "[Button] Pressed: %d", button_index);
    UART1_Printf(dbg, "\033[33m");
    SSD1306_Clear();
    const uint8_t *line3[] = {
            Font5x7[char_to_index('P')],  // P
            Font5x7[char_to_index('r')],  // r
            Font5x7[char_to_index('e')],  // e
            Font5x7[char_to_index('s')],  // s
            Font5x7[char_to_index('s')],  // s
            Font5x7[char_to_index('e')],  // e
            Font5x7[char_to_index('d')],  // d
            Font5x7[char_to_index(':')],  // :
            Font5x7[char_to_index(' ')],  // (space)
            Font5x7[num_to_font_index(button_index)]  // Button number (0-9)
        };

        // Display configuration
        uint8_t line3_length = 10;  // 10 characters in "Pressed: X"
        uint8_t char_width = 6;     // 5px char + 1px spacing
        uint8_t total_width3 = line3_length * char_width;
        uint8_t start_col3 = (128 - total_width3) / 2;  // Center calculation

        // Display "Pressed: X" (centered on page 3)
        uint8_t col3 = start_col3;
        for (uint8_t i = 0; i < line3_length; i++) {
            SSD1306_DrawChar(col3, 3, line3[i]);  // Page 3 (rows 24-31)
            col3 += char_width;
        }

    uint8_t expected = led_sequence[current_level][current_index];

    if (button_index == expected) {
        current_index++;

        if (current_index >= led_count[current_level]) {
            current_level++;
            current_index = 0;

            if (current_level >= MAX_LEVEL) {
                Game_Win();
            } else {
            	current_score += 1;
                UART1_Printf("[Game] Level Up!", "\033[36m");
                SSD1306_Clear();
                const uint8_t *level_up_message[] = {
                    Font5x7[char_to_index('L')],  // L
                    Font5x7[char_to_index('E')],  // E
                    Font5x7[char_to_index('V')],  // V
                    Font5x7[char_to_index('E')],  // E
                    Font5x7[char_to_index('L')],  // L
                    Font5x7[char_to_index(' ')],  // (space)
                    Font5x7[char_to_index('U')],  // U
                    Font5x7[char_to_index('P')],  // P
                    Font5x7[char_to_index('!')]   // !
                };

                // Display "LEVEL UP!" (centered)
                uint8_t message_length = 9; // 9 characters in "LEVEL UP!"
                uint8_t start_col = (128 - (message_length * 6)) / 2; // Calculate center position
                uint8_t current_col = start_col;

                for (int i = 0; i < message_length; i++) {
                    SSD1306_DrawChar(current_col, 2, level_up_message[i]); // Display on page 2 (rows 16-23)
                    current_col += 6; // Move to next character position
                }
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
    SSD1306_Clear();
    const uint8_t *win_message[] = {
            Font5x7[char_to_index('Y')],  // Y
            Font5x7[char_to_index('O')],  // O
            Font5x7[char_to_index('U')],  // U
            Font5x7[char_to_index(' ')],  // (space)
            Font5x7[char_to_index('W')],  // W
            Font5x7[char_to_index('I')],  // I
            Font5x7[char_to_index('N')],  // N
            Font5x7[char_to_index('!')]   // !
        };

        // Display configuration
        uint8_t message_length = 8;  // 8 characters in "YOU WIN!"
        uint8_t char_width = 6;      // 5px char + 1px spacing
        uint8_t total_width = message_length * char_width;
        uint8_t start_col = (128 - total_width) / 2;  // Center calculation

        // Display "YOU WIN!" (centered on page 2)
        for (uint8_t i = 0; i < message_length; i++) {
            SSD1306_DrawChar(start_col + (i * char_width), 2, win_message[i]);
        }
    for (uint32_t i = 0; i < 300000; i++);
}

// === Game over ===
void Game_Over(void) {
    UART1_Printf("[Game] Game Over!", "\033[31m");
    char buffer[32];
    sprintf(buffer, "Your score is: %u", current_score);
    UART1_Printf(buffer, "\033[36m");
    SSD1306_Clear();
    // Line 1: "Game over!"
        const uint8_t *line1[] = {
            Font5x7[char_to_index('G')], // G
            Font5x7[char_to_index('a')], // a
            Font5x7[char_to_index('m')], // m
            Font5x7[char_to_index('e')], // e
            Font5x7[char_to_index(' ')], // (space)
            Font5x7[char_to_index('o')], // o
            Font5x7[char_to_index('v')], // v
            Font5x7[char_to_index('e')], // e
            Font5x7[char_to_index('r')], // r
            Font5x7[char_to_index('!')]  // !
        };

        // Line 2: "Score: X"
        const uint8_t *line2[] = {
            Font5x7[char_to_index('S')], // S
            Font5x7[char_to_index('c')], // c
            Font5x7[char_to_index('o')], // o
            Font5x7[char_to_index('r')], // r
            Font5x7[char_to_index('e')], // e
            Font5x7[char_to_index(':')], // :
            Font5x7[char_to_index(' ')], // (space)
            Font5x7[16 + current_score]  // Score digit (0-9)
        };

        // Display line 1 (centered)
        uint8_t line1_width = 10 * 6; // 10 chars * 6px each
        uint8_t start_col1 = (128 - line1_width) / 2;
        for (uint8_t i = 0; i < 10; i++) {
            SSD1306_DrawChar(start_col1 + (i * 6), 2, line1[i]);
        }

        // Display line 2 (centered)
        uint8_t line2_width = 8 * 6; // 8 chars * 6px each
        uint8_t start_col2 = (128 - line2_width) / 2;
        for (uint8_t i = 0; i < 8; i++) {
            SSD1306_DrawChar(start_col2 + (i * 6), 4, line2[i]);
        }
        for (volatile int i = 0; i < 5000000; i++);

    current_state = STATE_WAIT_RESTART;
}

// === Show next LED (called by timer) ===
void Game_LEDNextLevel() {
	SSD1306_Clear();
    UART1_Printf("[Game] Show Level Sequence", "\033[36m");
    // Line 1: "Show level"
    const uint8_t *line1[] = {
        Font5x7[char_to_index('S')], // S
        Font5x7[char_to_index('h')], // h
        Font5x7[char_to_index('o')], // o
        Font5x7[char_to_index('w')], // w
        Font5x7[char_to_index(' ')], // (space)
        Font5x7[char_to_index('l')], // l
        Font5x7[char_to_index('e')], // e
        Font5x7[char_to_index('v')], // v
        Font5x7[char_to_index('e')], // e
        Font5x7[char_to_index('l')]  // l
    };

    // Line 2: "sequence"
    const uint8_t *line2[] = {
        Font5x7[char_to_index('s')], // s
        Font5x7[char_to_index('e')], // e
        Font5x7[char_to_index('q')], // q
        Font5x7[char_to_index('u')], // u
        Font5x7[char_to_index('e')], // e
        Font5x7[char_to_index('n')], // n
        Font5x7[char_to_index('c')], // c
        Font5x7[char_to_index('e')]  // e
    };

    // Display first line ("Show level")
    uint8_t line1_length = 10; // 10 characters
    uint8_t start_col1 = (128 - (line1_length * 6)) / 2;
    uint8_t col1 = start_col1;
    for (int i = 0; i < line1_length; i++) {
        SSD1306_DrawChar(col1, 2, line1[i]); // Page 2 (rows 16-23)
        col1 += 6;
    }

    // Display second line ("sequence")
    uint8_t line2_length = 8; // 8 characters
    uint8_t start_col2 = (128 - (line2_length * 6)) / 2;
    uint8_t col2 = start_col2;
    for (int i = 0; i < line2_length; i++) {
        SSD1306_DrawChar(col2, 4, line2[i]); // Page 4 (rows 32-39)
        col2 += 6;
    }

    for (uint8_t i = 0; i < led_count[current_level]; i++) {
        uint8_t led = led_sequence[current_level][i];
        LED_On(led);                             
        LED_Off(led);                            
        Timer_Start(delay_table[difficulty - 1]); // delay for the next led
    }

    UART1_Printf("[Game] Your turn!", "\033[33m");
    SSD1306_Clear();
    // Line 3: "Your turn!"
    const uint8_t *line3[] = {
        Font5x7[char_to_index('Y')],  // Y
        Font5x7[char_to_index('o')],  // o
        Font5x7[char_to_index('u')],  // u
        Font5x7[char_to_index('r')],  // r
        Font5x7[char_to_index(' ')],  // (space)
        Font5x7[char_to_index('t')],  // t
        Font5x7[char_to_index('u')],  // u
        Font5x7[char_to_index('r')],  // r
        Font5x7[char_to_index('n')],  // n
        Font5x7[char_to_index('!')]   // !
    };

    // Display configuration
    uint8_t line3_length = 10;  // 10 characters in "Your turn!"
    uint8_t char_width = 6;     // 5px char + 1px spacing
    uint8_t total_width3 = line3_length * char_width;
    uint8_t start_col3 = (128 - total_width3) / 2;  // Center calculation

    // Display "Your turn!" (centered)
    uint8_t col3 = start_col3;
    for (uint8_t i = 0; i < line3_length; i++) {
        SSD1306_DrawChar(col3, 3, line3[i]);  // Display on page 3 (rows 24-31)
        col3 += char_width;
    }
    current_index = 0;  
}

void Game_Loop() {
	Timer_Start(3000);
    while (1) {
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



                uint8_t tens = new_difficulty / 10;
                    uint8_t ones = new_difficulty % 10;
                    // Dòng 1: "Difficulty: X" (X là số từ 0-9)
                    const uint8_t *line1[] = {
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
                        Font5x7[char_to_index(':')],
                        Font5x7[char_to_index(' ')],
						Font5x7[num_to_font_index(tens)],  // Chữ số hàng chục
						Font5x7[num_to_font_index(ones)] // Số độ khó
                    };

                    // Dòng 2: "Press 0 to start"
                    const uint8_t *line2[] = {
                        Font5x7[char_to_index('P')],
                        Font5x7[char_to_index('r')],
                        Font5x7[char_to_index('e')],
                        Font5x7[char_to_index('s')],
                        Font5x7[char_to_index('s')],
                        Font5x7[char_to_index(' ')],
                        Font5x7[char_to_index('1')], // Số 0
                        Font5x7[char_to_index(' ')],
                        Font5x7[char_to_index('t')],
                        Font5x7[char_to_index('o')],
                        Font5x7[char_to_index(' ')],
                        Font5x7[char_to_index('s')],
                        Font5x7[char_to_index('t')],
                        Font5x7[char_to_index('a')],
                        Font5x7[char_to_index('r')],
                        Font5x7[char_to_index('t')]
                    };

                    SSD1306_Clear();
                    // Hiển thị dòng 1 (Difficulty: X)
                    uint8_t total_width1 = 14 * 6; // 14 ký tự (tăng thêm 1 cho số 2 chữ số)
                        uint8_t start_col1 = (128 - total_width1) / 2;
                        uint8_t col1 = start_col1;
                        for (int i = 0; i < 14; i++) {
                            SSD1306_DrawChar(col1, 2, line1[i]); // Hiển thị ở page 2
                            col1 += 6;
                        }

                    // Hiển thị dòng 2 (Press 0 to start)
                    uint8_t total_width2 = 16 * 6; // 16 ký tự
                    uint8_t start_col2 = (128 - total_width2) / 2;
                    uint8_t col2 = start_col2;
                    for (int i = 0; i < 16; i++) {
                        SSD1306_DrawChar(col2, 4, line2[i]); // Page 4 (hàng 32-39)
                        col2 += 6;
                    }

            }
        }

        else if (current_state == STATE_PLAYING) {
            char buffer[32];
            sprintf(buffer, "Playing level: %u", current_level + 1);
            UART1_Printf(buffer, "\033[36m");

            SSD1306_Clear();
            // Chuyển số thành 2 chữ số riêng
                uint8_t tens = (current_level + 1) / 10;
                uint8_t ones = (current_level + 1) % 10;

                // Dòng: "Playing level: XX"
                const uint8_t *line3[] = {
                    Font5x7[char_to_index('P')],  // P
                    Font5x7[char_to_index('l')],  // l
                    Font5x7[char_to_index('a')],  // a
                    Font5x7[char_to_index('y')],  // y
                    Font5x7[char_to_index('i')],  // i
                    Font5x7[char_to_index('n')],  // n
                    Font5x7[char_to_index('g')],  // g
                    Font5x7[char_to_index(' ')],  // (space)
                    Font5x7[char_to_index('l')],  // l
                    Font5x7[char_to_index('e')],  // e
                    Font5x7[char_to_index('v')],  // v
                    Font5x7[char_to_index('e')],  // e
                    Font5x7[char_to_index('l')],  // l
                    Font5x7[char_to_index(':')],  // :
                    Font5x7[char_to_index(' ')],  // (space)
                    Font5x7[num_to_font_index(tens)],  // Chữ số hàng chục
                    Font5x7[num_to_font_index(ones)]   // Chữ số hàng đơn vị
                };

                // Tính toán và hiển thị
                uint8_t total_width3 = 17 * 6; // 17 ký tự (tăng thêm 1 cho số 2 chữ số)
                uint8_t start_col3 = (128 - total_width3) / 2;
                uint8_t col3 = start_col3;

                for (int i = 0; i < 17; i++) {
                    SSD1306_DrawChar(col3, 2, line3[i]); // Hiển thị ở page 2
                    col3 += 6;
                }
            while(current_state == STATE_PLAYING);
        }

        else if (current_state == STATE_SHOWLEVEL) {
        	Game_LEDNextLevel();
        	current_state = STATE_PLAYING;
        }

        else if (current_state == STATE_WAIT_RESTART) {
        	SSD1306_Clear();
            UART1_Printf("Press button 1 to play again", "\033[36m");
            // Line 1: "Press 0"
            const uint8_t *line4[] = {  // Changed to line4
                Font5x7[char_to_index('P')],
                Font5x7[char_to_index('r')],
                Font5x7[char_to_index('e')],
                Font5x7[char_to_index('s')],
                Font5x7[char_to_index('s')],
                Font5x7[char_to_index(' ')],
                Font5x7[num_to_font_index(1)]
            };

            // Line 2: "to play again"
            const uint8_t *line5[] = {  // Changed to line5
                Font5x7[char_to_index('t')],
                Font5x7[char_to_index('o')],
                Font5x7[char_to_index(' ')],
                Font5x7[char_to_index('p')],
                Font5x7[char_to_index('l')],
                Font5x7[char_to_index('a')],
                Font5x7[char_to_index('y')],
                Font5x7[char_to_index(' ')],
                Font5x7[char_to_index('a')],
                Font5x7[char_to_index('g')],
                Font5x7[char_to_index('a')],
                Font5x7[char_to_index('i')],
                Font5x7[char_to_index('n')]
            };

            // Display first line ("Press 0")
            uint8_t line4_length = 7;  // Changed to line4_length
            uint8_t start_col4 = (128 - (line4_length * 6)) / 2;  // Changed to start_col4
            uint8_t col4 = start_col4;  // Changed to col4
            for (int i = 0; i < line4_length; i++) {
                SSD1306_DrawChar(col4, 3, line4[i]);  // Changed page to 3 (rows 24-31)
                col4 += 6;
            }

            // Display second line ("to play again")
            uint8_t line5_length = 13;  // Changed to line5_length
            uint8_t start_col5 = (128 - (line5_length * 6)) / 2;  // Changed to start_col5
            uint8_t col5 = start_col5;  // Changed to col5
            for (int i = 0; i < line5_length; i++) {
                SSD1306_DrawChar(col5, 5, line5[i]);  // Changed page to 5 (rows 40-47)
                col5 += 6;
            }
            while(current_state == STATE_WAIT_RESTART);
        }

        else if (current_state == STATE_START) {
        
        }
    }
}


// Handle event for button 2 and 3
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
            case STATE_WAIT_DIFFICULTY: // Enter difficulty
                SetDifficulty(temp_difficulty);
                Start();
                current_state = STATE_SHOWLEVEL;
                break;

             case STATE_WAIT_RESTART: // Restart game
            	current_level = 0;
            	current_index = 0;
            	current_score = 0;
                current_state = STATE_WAIT_DIFFICULTY;
                char buffer[32];
                sprintf(buffer, "Set difficulty: %u", temp_difficulty);
                UART1_Printf(buffer, "\033[33m");
                UART1_Printf("Press button 1 to start", "\033[36m");
                uint8_t tens = temp_difficulty / 10;
                uint8_t ones = temp_difficulty % 10;
                                    // Dòng 1: "Difficulty: X" (X là số từ 0-9)
                                    const uint8_t *line1[] = {
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
                                        Font5x7[char_to_index(':')],
                                        Font5x7[char_to_index(' ')],
                						Font5x7[num_to_font_index(tens)],  // Chữ số hàng chục
                						Font5x7[num_to_font_index(ones)] // Số độ khó
                                    };

                                    // Dòng 2: "Press 0 to start"
                                    const uint8_t *line2[] = {
                                        Font5x7[char_to_index('P')],
                                        Font5x7[char_to_index('r')],
                                        Font5x7[char_to_index('e')],
                                        Font5x7[char_to_index('s')],
                                        Font5x7[char_to_index('s')],
                                        Font5x7[char_to_index(' ')],
                                        Font5x7[char_to_index('1')], // Số 0
                                        Font5x7[char_to_index(' ')],
                                        Font5x7[char_to_index('t')],
                                        Font5x7[char_to_index('o')],
                                        Font5x7[char_to_index(' ')],
                                        Font5x7[char_to_index('s')],
                                        Font5x7[char_to_index('t')],
                                        Font5x7[char_to_index('a')],
                                        Font5x7[char_to_index('r')],
                                        Font5x7[char_to_index('t')]
                                    };

                                    SSD1306_Clear();
                                    // Hiển thị dòng 1 (Difficulty: X)
                                    uint8_t total_width1 = 14 * 6; // 14 ký tự (tăng thêm 1 cho số 2 chữ số)
                                        uint8_t start_col1 = (128 - total_width1) / 2;
                                        uint8_t col1 = start_col1;
                                        for (int i = 0; i < 14; i++) {
                                            SSD1306_DrawChar(col1, 2, line1[i]); // Hiển thị ở page 2
                                            col1 += 6;
                                        }

                                    // Hiển thị dòng 2 (Press 0 to start)
                                    uint8_t total_width2 = 16 * 6; // 16 ký tự
                                    uint8_t start_col2 = (128 - total_width2) / 2;
                                    uint8_t col2 = start_col2;
                                    for (int i = 0; i < 16; i++) {
                                        SSD1306_DrawChar(col2, 4, line2[i]); // Page 4 (hàng 32-39)
                                        col2 += 6;
                                    }
                break;

             case STATE_PLAYING: // Choose button when playing
                ChooseButtonWhenPlay(0);
                break;

             default:
                break;
        }
        // Avoid debounce
        for (volatile int i = 0; i < 100000; i++);
        EXTI->PR |= (1 << 8);
    }

}

// Handle event for button 2 and 3
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



