# 🛡️ Project: CyberDyne CIWS (v2.4)
### *Autonomous Close-In Weapon System HUD for ESP32*

This repository contains the source code and wiring for a DIY **Close-In Weapon System** prototype. It features a Terminator-inspired HUD, 65-115° high-speed radar scanning, and active target locking.

---

## 🔌 Master Wiring Map (ESP32)

### 🖥️ ST7789 TFT Display (High Priority)

| TFT Pin | ESP32 Pin | Function | Wire Color (Typical) |
| :--- | :--- | :--- | :--- |
| **VCC** | **3V3** | Power (3.3V) | Red |
| **GND** | **GND** | Ground | Black |
| **SCL** | **GPIO 18** | SPI Clock | Yellow |
| **SDA** | **GPIO 23** | SPI MOSI (Data) | Green |
| **RES** | **GPIO 4** | Reset | Orange |
| **DC** | **GPIO 2** | Data/Command | Blue |
| **CS** | **GPIO 5** | Chip Select | White |
| **BLK** | **3V3** | Backlight Control | Red |

### 🛰️ VL53L1X ToF Sensor (I2C)

| Sensor Pin | ESP32 Pin | Function |
| :--- | :--- | :--- |
| **VIN** | **3V3** | Power |
| **GND** | **GND** | Ground |
| **SCL** | **GPIO 22** | I2C Clock |
| **SDA** | **GPIO 21** | I2C Data |

### 🕹️ Servos & Alerts

| Component | ESP32 Pin | Function |
| :--- | :--- | :--- |
| **Servo A** | **GPIO 19** | Pan (Horizontal Sweep) |
| **Servo B** | **GPIO 12** | Tilt (Vertical Wobble) |
| **Buzzer** | **GPIO 13** | Active Audio Alerts |
| **LED Red** | **GPIO 14** | Target Lock Indicator |
| **LED Green** | **GPIO 27** | System Status |

---

## ⚠️ CRITICAL SETUP REQUIREMENT
To avoid PWM timer conflicts between the **Buzzer** and **Servos**:
*   **Library:** `ESP32Servo`
*   **Required Version:** `2.0.14` 
*   *Note: Using newer versions may cause the servos to jitter or the ESP32 to crash during audio play.*

---

## 🛠️ Build Notes
*   **Wiper Logic:** The radar sweeps between **65° and 115°**.
*   **Terminator HUD:** Features "1 Letter = 1 Beep" boot sequence.
*   **Power:** If the screen flickers, power the Servos from an external 5V source (Common GND).

---
*Built for the maker community. Feel free to remix, upgrade, and share your versions!*
