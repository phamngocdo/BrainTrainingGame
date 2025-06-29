# 🧠 Brain Training Game using STM32

A brain-training memory game built with the STM32F401CCU6 microcontroller. The game challenges players to remember and repeat the sequence of flashing LEDs, similar to the classic "Simon Says" game.

![Platform](https://img.shields.io/badge/platform-STM32-blue) ![Language](https://img.shields.io/badge/language-C%20(low%20level)-important) ![License](https://img.shields.io/badge/license-MIT-green)

## 🔧 Features

- 🚥 3 LEDs connected to GPIO pins (PA0 - PA2)
- 🔘 3 buttons for user input (PA9 - PA11)
- 🎮 Randomly flashing LED sequence each round
- ✅ Player wins by pressing buttons in correct order
- ❌ Game ends if player makes a wrong move
- 🔄 Game resets and increases difficulty after each round

## 📷 Demo

> _(Add a GIF or short video of gameplay here if available)_  
> _(Or a screenshot of your setup with STM32 board and LEDs)_

## 🛠️ Hardware Requirements

- STM32F401CCU6 (Blue Pill or similar board)
- 3 LEDs + resistors
- 3 Push buttons + pull-down resistors
- Breadboard and jumper wires
- USB-TTL adapter for serial communication (optional, for debugging)

## ⚙️ Software Used

- STM32CubeIDE (for development and debugging)
- HAL or direct register-based programming (register-level code)
- Optional: STM32CubeProgrammer for flashing firmware

## 🧩 How It Works

1. Game begins by flashing a random sequence of LEDs.
2. Player must press the corresponding buttons in the same order.
3. If the input matches, next round starts with a longer sequence.
4. If the input is wrong, game ends and restarts.

## 📁 Repository Structure

