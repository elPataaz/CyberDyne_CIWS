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
| **Servo B** | **GPIO 12** | Tilt (Vertical Inclination sweep) |
| **Buzzer** | **GPIO 13** | Active Audio Alerts |
| **LED Red** | **GPIO 14** | Target Lock Indicator |
| **LED Green** | **GPIO 27** | System Status |
----
### 💡 LED Hardware Protection (220Ω Resistors)
The ESP32 GPIO pins output 3.3V, but they are sensitive to high current. You **must** use a current-limiting resistor for each LED to prevent hardware damage.

- **Recommended Value**: **220Ω** (Red-Red-Brown stripes). 
- **The Setup**: 
  - **Red LED (Lock)**: ESP Pin 14 ➡️ 220Ω Resistor ➡️ LED Anode (+) ➡️ GND.
  - **Green LED (Status)**: ESP Pin 27 ➡️ 220Ω Resistor ➡️ LED Anode (+) ➡️ GND.
- **Why?**: A 220Ω resistor strikes the perfect brightness balance

----
## ⚠️ CRITICAL: Common Ground Requirement
If you are using an external battery pack or power supply for your servos (highly recommended), you **MUST** connect the Ground (GND) of that power supply to a **GND pin on the ESP32**.

*   **Why?** Without a shared ground, the "Signal" wire from the ESP32 has no reference point. This causes the servos to jitter uncontrollably and can lead to I2C communication errors with the Laser sensor.
*   **The Rule:** All black (GND) wires from the Display, Sensor, Servos, and ESP32 must eventually meet on the same "Minus" rail of your breadboard.


## ⚠️ CRITICAL SETUP REQUIREMENT
To avoid PWM timer conflicts between the **Buzzer** and **Servos**:
*   **Library:** `ESP32Servo`
*   **Required Version:** `2.0.14` 
*   *Note: Using newer versions may cause the servos to jitter or the ESP32 to crash during audio play.*

*   ## 🔒 Stability Lockdown (Do Not Update)
This project is precisely tuned to specific core versions. Updating may break the PWM timers for the Buzzer and Servos.

- **Arduino IDE**: 2.3.2 (Recommended stable version)
- **ESP32 Board Manager**: **Version 2.0.14** (IMPORTANT)
  - *If you use 3.0.x or higher, the `ledc` functions and `ESP32Servo` library will conflict.*
- **Library Pins**: Do not change the Pin assignments unless you understand ESP32 Hardware Timer allocation.

### How to lock your version:
1. In Arduino IDE, go to **Tools** > **Board** > **Boards Manager**.
2. Search for **esp32** by Espressif Systems.
3. If it says a higher version is installed, click the dropdown and select **2.0.14**, then click **Install**.

4. ## 💡 Pro Tip: Flash and Forget
Once you have successfully flashed the **CyberDyne CIWS** code onto your ESP32, the board is "Production Ready." 

- **Independent Operation**: The ESP32 does not need the Arduino IDE or your computer to run. It will boot the HUD and start the 65-125° sweep as soon as it gets 5V power.
- **IDE Updates**: After your CIWS is "Live," you can safely update your Arduino IDE and ESP32 Board Manager (v3.0+) for **other** projects. Just remember that if you ever need to edit or re-flash THIS project, you must downgrade your Board Manager back to **v2.0.14** first.



---

## 🛠️ Build Notes
*   **Wiper Logic:** The radar sweeps between **65° and 115°**.
*   **Terminator HUD:** Features "1 Letter = 1 Beep" boot sequence.
*   **Power:** If the screen flickers, power the Servos from an external 5V source (Common GND).

*   ## ⚡ Pro-Maker Hardware Tips

### 1. Stabilizing the Power (Anti-Jitter)
Servos create significant "electrical noise" and sudden current spikes when they move. 
*   **The Fix:** Add a **470µF to 1000µF electrolytic capacitor** across the 5V and GND rails near the servos. This acts as a "mini-battery" to prevent the ESP32 from rebooting during fast sweeps.
*   **Separate Rails:** Always keep the **TFT and ToF sensor on the 3.3V rail** and the **Servos on the 5V rail** to prevent high-voltage damage.

*   
*   ## ⚠️ CRITICAL: Common Ground Requirement
If you are using an external battery pack or power supply for your servos (highly recommended), you **MUST** connect the Ground (GND) of that power supply to a **GND pin on the ESP32**.

*   **Why?** Without a shared ground, the "Signal" wire from the ESP32 has no reference point. This causes the servos to jitter uncontrollably and can lead to I2C communication errors with the Laser sensor.
*   **The Rule:** All black (GND) wires from the Display, Sensor, Servos, and ESP32 must eventually meet on the same "Minus" rail of your breadboard.


### 2. I2C Reliability (VL53L1X)
The ToF sensor communicates via I2C. If your wires are longer than 10cm, you might experience "hangs".
*   **Pull-up Resistors:** Most VL53L1X modules have built-in 10k resistors. If yours does not, add **4.7kΩ pull-up resistors** from SDA and SCL to 3.3V to stabilize the signal.

### 3. Display Performance
*   **SPI Speed:** For the cleanest HUD updates, use the **Hardware SPI pins** (18 and 23) rather than defining random pins in software.
*   **Backlight:** If the screen is too dim, you can connect the **BLK** pin to a PWM-capable pin (like GPIO 32) to control brightness in code, or just tie it to 3.3V for max brightness.

---

## 🐛 Troubleshooting the "Sleepless Nights"
- **Ghosting Text:** Ensure the `tft.fillRect()` coordinates match the `tft.setCursor()` exactly to prevent old numbers from staying on screen.
- **Boot Loop:** If the ESP32 restarts every time the servo moves, your power source (USB) isn't strong enough. Switch to a **2A wall adapter**.
- **I2C Not Found:** Use an "I2C Scanner" sketch to verify the VL53L1X is responding on address `0x29`.


---
*Built for the maker community. Feel free to remix, upgrade, and share your versions!*
