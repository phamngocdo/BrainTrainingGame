# 🧠 Brain Training Game using STM32

A brain-training memory game built with the STM32F401CCU6 microcontroller. The game challenges players to remember and repeat the sequence of flashing LEDs, similar to the classic "Simon Says" game.

![Platform](https://img.shields.io/badge/platform-STM32-blue) ![Language](https://img.shields.io/badge/language-C%20(low%20level)-important) ![License](https://img.shields.io/badge/license-MIT-green)

## 🔧 Features

- 🚥 4 LEDs connected to GPIO pins (PA0 - PA3)
- 🔘 4 buttons for user input (PA8 - PA11)
- 🎚️ 1 potentiometer connected to analog input pin PA5 for adjusting game difficulty
- 🎮 Randomly flashing LED sequence each round
- ✅ Player wins by pressing buttons in correct order
- ❌ Game ends if player makes a wrong move
- 🔄 Game resets and increases difficulty after each round

## 📷 Demo



## 🛠️ Hardware Requirements

- STM32F401CCU6 (Blue Pill or similar board)
- 4 LEDs + resistors
- 4 Push buttons + pull-down resistors
- Breadboard and jumper wires
- USB-TTL adapter for serial communication (optional, for debugging)
- SSD1306 Screen

## ⚙️ Software Used

- STM32CubeIDE (for development and debugging)
- HAL or direct register-based programming (register-level code)
- Optional: STM32CubeProgrammer for flashing firmware

## 🧩 How It Works

1. Game begins by flashing a random sequence of LEDs.
2. Player must press the corresponding buttons in the same order.
3. If the input matches, next round starts with a longer sequence.
4. If the input is wrong, game ends and restarts.

## 🚀 Getting Started

1. Clone the repo
2. Open project with STM32CubeIDE
3. Connect STM32F401 board and flash the code
4. Enjoy the game!

## 👥 Authors
- **[Pham Ngoc Do](https://github.com/phamngocdo)**
- **[Nguyen Tien Dung](https://github.com/guin-D)**
