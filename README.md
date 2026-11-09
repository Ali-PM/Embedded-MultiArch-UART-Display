# Multi-Architecture Embedded System: Keypad Scanner, UART & 7-Segment Driver

A comparative embedded engineering project demonstrating real-time peripheral control, matrix keypad scanning, serial bus communication, and multiplexed 7-segment display driving across multiple abstraction layers: **STM32 Low-Layer/Bare-Metal (LL)**, **HAL / Standard Peripheral Library (SPL)**, and **AVR/Arduino**.

---

## 🚀 Architectural Implementations

This repository showcases four different hardware and firmware design paradigms for solving embedded I/O and communication tasks:

1. **STM32 Pure Register / Low-Layer (LL) Implementation:**
   * Direct register-level programming (`RCC`, `GPIO`, `USART1`) on ARM Cortex-M3 (STM32F103C8).
   * Zero overhead execution, zero-HAL dependency, and sub-microsecond register read/write cycles.
   * Runtime input bounds validation (0–255 range limiting), segment locking (`#`), and buffer flushing (`*`).

2. **STM32 HAL / SPL Abstraction Implementation:**
   * Structured firmware implementation leveraging peripheral drivers for system clock init (72MHz PLL via HSE) and structured GPIO/USART interrupt management.

3. **Hybrid Cross-MCU Bridge (Arduino TX -> STM32 RX):**
   * ATmega328P acting as a hardware-debounced matrix keypad scanner (`DDRD`, `PORTD`, `PINB` direct port access).
   * Asynchronous UART transmission of key codes to the STM32 display controller at 9600 baud.

4. **Standalone AVR/Arduino Dual-Digit Counter:**
   * Standalone 2-digit 0–99 counter utilizing bitwise segment mask decoding (`0x3F`–`0x6F`) and button edge-detection debounce logic.

---

## 📊 Feature & Implementation Comparison

| Implementation Mode | MCU Architecture | Layer / Abstraction | Memory Footprint | Latency / Overhead |
| :--- | :---: | :---: | :---: | :---: |
| **STM32 Low-Layer (LL)** | ARM Cortex-M3 | Direct CMSIS Registers | Minimal (~1.9 KB) | Direct CPU cycle execution |
| **STM32 HAL / SPL** | ARM Cortex-M3 | Standard Library | Moderate | Driver abstraction overhead |
| **Hybrid Bridge** | ATmega328P + STM32 | Mixed (Bare-metal + HAL) | Distributed | Decoupled sensor-display processing |
| **Standalone Arduino** | ATmega328P (AVR) | Arduino Framework | Standard | Timer/polling based |

---

## 🛠️ Toolchains & Environment

* **Microcontrollers:** STM32F103C8T6 (Blue Pill), ATmega328P
* **IDEs & Compilers:** Keil uVision v5 (ArmClang v6), Arduino IDE
* **Simulation & Circuit Analysis:** Proteus 8 Professional
* **Protocols:** USART / UART (9600 Baud, 8N1)
* **Languages:** Embedded C, C++

---

## 📁 Repository Structure

```text
├── 1-STM32-LL-BareMetal/         # Low-layer / pure register firmware
├── 2-STM32-HAL-SPL/              # HAL & standard peripheral implementations
├── 3-STM32-Arduino-Bridge/       # Multi-MCU UART keypad bridge
├── 4-Standalone-Arduino/         # 2-Digit debounce counter
├── Simulation/                   # Complete Proteus schematic files (.pdsprj)
├── .gitignore
└── README.md

⚙️ Pinout & Bus Connections

    Serial Parameters: 9600 bps, 8 Data Bits, 1 Stop Bit, No Parity

    STM32 Interface:

        PA9: USART1 TX

        PA10: USART1 RX (Connected to Master TX)

        PB0 - PB6: 7-Segment Unit 0

        PB7 - PB13: 7-Segment Unit 1

        PA0 - PA6: 7-Segment Unit 2

    Keypad Matrix (ATmega328P):

        Rows: PD2 - PD5

        Columns: PB0, PD6, PD7
```

👨‍💻 Author

Ali Molaei

M.Sc. in Mechatronics Engineering | Embedded Systems & Automation

    Email: molaeiali24@gmail.com

    Telegram: @AliMoj4z
