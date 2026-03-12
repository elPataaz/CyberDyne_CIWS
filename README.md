# CyberDyne_CIWS
DIY Terminator-inspired Close-In Weapon System HUD
-------------------------------------------------------------------------------
🛡️ Project: CyberDyne CIWS (v2.4)
Autonomous Close-In Weapon System HUD for ESP32
Inspired by the T-800 interface, this project is a DIY Close-In Weapon System prototype. It uses a VL53L1X Laser ToF sensor for millimeter-accurate tracking and dual servos for high-speed target acquisition.
-----------------------------------------------------------------------------------
⚠️ CRITICAL SETUP REQUIREMENT
To avoid PWM timer conflicts between the Buzzer and Servos (which causes jittering or crashes):
Library: ESP32Servo
Required Version: 2.0.14
Note: Do not use the latest version; it is known to break PWM stability on ESP32 logic.
-------------------------------------------------------------------------------------
🚀 System Features
Authentic Boot Sequence: Terminal-style text rendering with "1 Letter = 1 Beep" synchronized audio.
Precision Radar: High-speed laser scanning between 65° and 115°.
Dynamic Lock-On: Automatically switches to "Search & Destroy" mode when a target enters the 400mm kill zone.
Visual HUD: Real-time Distance (mm) and Angle (DEG) tracking with zero-ghosting UI updates.
Aggressive Tracking: Faster Servo B vertical "wobble" once a target is locked.
🔌 Hardware / Pin Map
Component	Pin	Notes
TFT CS	5	Standard SPI
TFT DC	2	
TFT RST	4	
Buzzer	13	Active Buzzer Required
LED Red	14	Target Lock Indicator
LED Green	27	System Status Indicator
Servo A	19	Pan (Horizontal)
Servo B	12	Tilt (Vertical)
🛠️ Installation
Install the ESP32 Board Manager in Arduino IDE.
Install Adafruit ST7789 and VL53L1X libraries.
Downgrade ESP32Servo to v2.0.14 via the Library Manager.
Open CyberDyne_CIWS.ino and upload.
📸 Prototypes
(Upload your "spaghetti" wiring photo here to show the raw build process!)
![Wiring Prototype](wiring_prototype.jpg)
Created for the maker community. Built for satisfaction, frustration, and a few sleepless nights. Feel free to remix and add your own weapon modules!

