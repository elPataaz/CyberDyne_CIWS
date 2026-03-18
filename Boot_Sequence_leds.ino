#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h> 

// Hardware Pins (Waveshare 2" ST7789 + ESP32)
#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   4
#define BUZZER_PIN 13
#define BUZZ_CH    0
#define LED_RED    14
#define LED_GREEN  27

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// ---------- 1. FX: RED PULSE STARTUP ----------
void screenFlickerFade() {
  for (int i = 0; i <= 31; i++) {
    tft.fillScreen(i << 11); 
    if (i % 2 == 0) ledcWriteTone(BUZZ_CH, 80 + (i * 4)); 
    delay(15); 
  }
  for (int j = 0; j < 6; j++) {
    tft.fillScreen(0xF800); ledcWriteTone(BUZZ_CH, 350); delay(30);
    tft.fillScreen(0x0000); ledcWriteTone(BUZZ_CH, 0);   delay(30);
  }
  for (int i = 31; i >= 0; i--) {
    tft.fillScreen(i << 11); delay(15);
  }
  ledcWriteTone(BUZZ_CH, 0);
  tft.fillScreen(0x0000);
}

// ---------- 2. FX: SCANNING BEAM WIPE ----------
void clearScreenWithBeams() {
  for (int pass = 0; pass < 2; pass++) {
    for (int y = -5; y < 245; y += 3) {
      tft.drawFastHLine(0, y,     320, 0xF800); 
      tft.drawFastHLine(0, y - 1, 320, 0x8000); 
      tft.drawFastHLine(0, y + 1, 320, 0x8000);
      tft.drawFastHLine(0, y - 3, 320, 0x0000); 
      if (y % 12 == 0) ledcWriteTone(BUZZ_CH, 150 + (pass * 50)); 
      delay(3);
    }
    ledcWriteTone(BUZZ_CH, 0);
    tft.fillScreen(0x0000);
    delay(150);
  }
}

// ---------- 3. UI UTILITIES ----------
void beepRed() {
  digitalWrite(LED_RED, HIGH);
  ledcWriteTone(BUZZ_CH, 1200);
  delay(25);
  ledcWriteTone(BUZZ_CH, 0);
  digitalWrite(LED_RED, LOW);
}

void beepGreen() {
  digitalWrite(LED_GREEN, HIGH);
  ledcWriteTone(BUZZ_CH, 1200);
  delay(25);
  ledcWriteTone(BUZZ_CH, 0);
  digitalWrite(LED_GREEN, LOW);
}

void typeLine(const char* text, int x, int y) {
  tft.setCursor(x, y);
  tft.setTextColor(0xF800); 
  tft.setTextSize(2); 
  for (int i = 0; text[i]; i++) {
    tft.print(text[i]);
    if (text[i] != ' ') beepRed();
    delay(45);
  }
}

void blinkCursor(uint16_t ms) {
  uint16_t elapsed = 0;
  int x = tft.getCursorX(), y = tft.getCursorY();
  while (elapsed < ms) {
    tft.fillRect(x, y + 12, 12, 3, ( (elapsed / 166) % 2 == 0) ? 0xF800 : 0x0000);
    delay(166); 
    elapsed += 166;
  }
  tft.fillRect(x, y + 12, 12, 3, 0x0000);
  tft.setCursor(x, y);
}

void printOK() {
  blinkCursor(800);
  tft.setTextColor(0x07E0); 
  tft.print("OK");
  beepGreen(); beepGreen();
}

void typeONLINE() {
  blinkCursor(1500);
  tft.setTextColor(0x07E0);
  const char* t = "ONLINE";
  for (int i = 0; t[i]; i++) { 
    tft.print(t[i]); 
    beepGreen(); 
    delay(45); 
  }
}

// ---------- 4. MAIN SETUP ----------
void setup() {
  tft.init(240, 320); 
  tft.setRotation(3); 
  tft.fillScreen(0x0000);

  // Core 2.0.14 legacy LEDC setup
  ledcSetup(BUZZ_CH, 2000, 8); 
  ledcAttachPin(BUZZER_PIN, BUZZ_CH);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  // Boot FX
  screenFlickerFade();
  delay(500);

  // Cyberdyne Sequence
  typeLine("CYBERDYNE SYSTEMS", 20, 20);
  typeLine("MODEL T-800 / REV 2.4", 20, 50);
  typeLine("MEMORY CHECK...", 20, 90); printOK();
  typeLine("ARM_FREQ=1500..", 20, 120); printOK();
  typeLine("GPU_FREQ=500...", 20, 150); printOK();
  typeLine("SYSTEM STATUS: ", 20, 190);
  typeONLINE();
  
  delay(2000);

  // Transition to Main Loop
  clearScreenWithBeams();
  digitalWrite(LED_GREEN, HIGH);
}

void loop() {
  // Main Terminator HUD Logic goes here
}
