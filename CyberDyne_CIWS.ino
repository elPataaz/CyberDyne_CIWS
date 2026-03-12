#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_VL53L1X.h>

#define TFT_CS 5
#define TFT_DC 2
#define TFT_RST 4
#define BUZZER_PIN 13
#define BUZZ_CH 0
#define LED_RED 14
#define LED_GREEN 27
#define SERVO_A_PIN 19
#define SERVO_B_PIN 12

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Servo servoA, servoB;
Adafruit_VL53L1X vl53;

int cx = 160, cy = 241, maxR = 140;
int currentAngle = 90, lastAngle = 90;
int vertAngle = 90;
bool sweepingForward = true;
bool targetLocked = false;
unsigned long lockStartTime = 0;
const unsigned long LOCK_DURATION = 7000;

uint16_t lastGoodDistance = 0;
int lx = 0, ly = 0, lr = 0;
float vertStep = 0;

uint16_t RADAR_GREEN = 0x0400;
uint16_t BRIGHT_GREEN = 0x07E0;
uint16_t TARGET_RED = 0xF800;

float deg2rad(int d) { return d * 0.0174532925; }
void playTone(int f) { ledcWriteTone(BUZZ_CH, f); }

void beepRed() {
  digitalWrite(LED_RED, HIGH); playTone(1500); delay(20); playTone(0); digitalWrite(LED_RED, LOW);
}

void beepGreen() {
  digitalWrite(LED_GREEN, HIGH); playTone(1200); delay(25); playTone(0); digitalWrite(LED_GREEN, LOW);
}

void flashScreen(int n) {
  for (int i = 0; i < n; i++) {
    tft.fillScreen(TARGET_RED); playTone(400); delay(100);
    tft.fillScreen(0x0000); playTone(0); delay(100);
  }
}

void typeLine(const char* s, int x, int y, uint16_t c) {
  tft.setCursor(x, y); tft.setTextColor(c); tft.setTextSize(2);
  for (int i = 0; s[i]; i++) {
    tft.print(s[i]);
    if (s[i] != ' ') (c == TARGET_RED) ? beepRed() : beepGreen();
    delay(45);
  }
}

void printWord(const char* w) {
  tft.setTextColor(BRIGHT_GREEN);
  for (int i = 0; w[i]; i++) {
    tft.print(w[i]);
    if (w[i] != ' ') beepGreen();
    delay(45);
  }
}

void clearWithBeams() {
  for (int y = 0; y < 120; y += 2) {
    tft.fillRect(0, y, 320, 3, TARGET_RED);
    tft.fillRect(0, 240 - y - 3, 320, 3, TARGET_RED);
    if (y % 4 == 0) playTone(100 + (y * 3));
    delay(25);
    tft.fillRect(0, y, 320, 3, 0x0000);
    tft.fillRect(0, 240 - y - 3, 320, 3, 0x0000);
  }
  playTone(800); delay(200); playTone(0);
}

void drawRadarGUI() {
  int rings[] = {maxR, maxR * 2 / 3, maxR / 3};
  for (int i = 0; i < 3; i++) {
    for (int a = 0; a <= 180; a++) {
      float r = deg2rad(a);
      tft.drawPixel(cx + cos(r) * rings[i], cy - sin(r) * rings[i], RADAR_GREEN);
    }
  }
  tft.fillRect(cx - maxR, cy - 2, maxR * 2, 3, RADAR_GREEN);
  int b[] = {30, 60, 90, 120, 150};
  for (int i = 0; i < 5; i++) {
    float r = deg2rad(b[i]);
    int xE = cx + cos(r) * maxR;
    int yE = cy - sin(r) * maxR;
    tft.drawLine(cx, cy, xE, yE, RADAR_GREEN);
    int tx = cx + cos(r) * (maxR + 15);
    int ty = cy - sin(r) * (maxR + 15);
    tft.setCursor(tx - 6, ty - 4);
    tft.setTextColor(TARGET_RED); tft.setTextSize(1); tft.print(b[i]);
  }
}

void drawFan(int a, uint16_t c) {
  for (float i = -3.5; i <= 3.5; i += 1.0) {
    float r = deg2rad(a + (int)i);
    int x = cx + cos(r) * (maxR - 5);
    int y = cy - sin(r) * (maxR - 5);
    tft.drawLine(cx, cy, x, y, c);
  }
}

void setup() {
  Wire.begin();
  vl53.begin(0x29, &Wire);
  vl53.setTimingBudget(20);
  vl53.startRanging();

  tft.init(240, 320); tft.setRotation(3); tft.fillScreen(0x0000);
  ledcSetup(BUZZ_CH, 2000, 8); ledcAttachPin(BUZZER_PIN, BUZZ_CH);

  ESP32PWM::allocateTimer(3);
  servoA.attach(SERVO_A_PIN, 500, 2400);
  servoB.attach(SERVO_B_PIN, 500, 2400);
  servoA.write(90); servoB.write(90);

  pinMode(LED_RED, OUTPUT); pinMode(LED_GREEN, OUTPUT);

  flashScreen(2);
  typeLine("  CYBERDYNE SYSTEMS", 5, 20, TARGET_RED);
  typeLine("PROJECT: CIWS / REV 2.4", 5, 50, TARGET_RED);
  typeLine("MEMORY CHECK...", 5, 90, TARGET_RED); printWord(" OK");
  typeLine("ARM_FREQ=1500..", 5, 120, TARGET_RED); printWord(" OK");
  typeLine("GPU_FREQ=500...", 5, 150, TARGET_RED); printWord(" OK");
  typeLine("SYSTEM STATUS: ", 5, 190, TARGET_RED); printWord(" ONLINE");

  clearWithBeams();

  // RESTORED INITIALIZE SEQUENCE
  typeLine("INITIALIZE SERVOS", 5, 10, TARGET_RED);
  servoA.write(45); servoB.write(70); delay(400);
  servoA.write(135); servoB.write(110); delay(400);
  servoA.write(90); servoB.write(90); printWord(" ONLINE");

  typeLine("INITIALIZE RADAR", 5, 35, TARGET_RED);
  printWord(" ONLINE");
  drawRadarGUI();

  typeLine("INITIALIZE LAZER", 5, 60, TARGET_RED);
  printWord(" ONLINE");
  drawFan(90, BRIGHT_GREEN);
  delay(1000);

  tft.fillRect(0, 0, 320, 95, 0x0000); 
  drawRadarGUI();

  typeLine("DISTANCE:", 5, 5, TARGET_RED);
  while (!vl53.dataReady());
  lastGoodDistance = vl53.distance();
  vl53.clearInterrupt();
  tft.setCursor(115, 5); tft.setTextColor(BRIGHT_GREEN); 
  tft.print(lastGoodDistance); tft.print("mm"); beepGreen();

  typeLine("ANGLE:", 5, 35, TARGET_RED);
  tft.setCursor(80, 35); tft.setTextColor(BRIGHT_GREEN); 
  tft.print(currentAngle); tft.print(" DEG"); beepGreen();

  typeLine("WEAPONS LOCKON:", 5, 65, TARGET_RED);
  digitalWrite(LED_GREEN, HIGH);
}

void loop() {
  if (vl53.dataReady()) {
    uint16_t n = vl53.distance();
    vl53.clearInterrupt();
    if (n > 30 && n < 4000) lastGoodDistance = n;
  }

  uint16_t dist = lastGoodDistance;
  if (lr > 0) {
    tft.fillCircle(lx, ly, lr + 1, 0x0000);
    if (dist > 400) lr = 0;
  }

  tft.setTextSize(2); tft.setTextColor(BRIGHT_GREEN);
  tft.fillRect(115, 5, 100, 20, 0x0000);
  tft.setCursor(115, 5); tft.print(dist); tft.print("mm");

  tft.fillRect(80, 35, 100, 20, 0x0000);
  tft.setCursor(80, 35); tft.print(currentAngle); tft.print(" DEG");

  bool near = (dist > 20 && dist <= 400);

  if (near) {
    if (!targetLocked) { targetLocked = true; lockStartTime = millis(); }
    float r = deg2rad(currentAngle);
    int dR = map(constrain(dist, 50, 400), 50, 400, 20, maxR);
    lx = cx + cos(r) * dR; ly = cy - sin(r) * dR;
    lr = map(constrain(dist, 50, 400), 50, 400, 16, 5);
    tft.fillCircle(lx, ly, lr, TARGET_RED);

    int bInt = map(constrain(dist, 50, 400), 50, 400, 80, 300);
    if ((millis() / bInt) % 2 == 0) {
      tft.setCursor(185, 65); tft.setTextColor(BRIGHT_GREEN); tft.print("WASTED!"); 
      digitalWrite(LED_GREEN, HIGH);
    } else {
      tft.fillRect(185, 65, 135, 20, 0x0000); digitalWrite(LED_GREEN, LOW);
    }

    if ((millis() % 100) < 50) { playTone(1800); digitalWrite(LED_RED, HIGH); } 
    else { playTone(0); digitalWrite(LED_RED, LOW); }

    // INCREASED SPEED FOR SERVO B
    vertStep += 0.2; 
    servoB.write(90 + sin(vertStep) * 20);

    if (dist > 550 || (millis() - lockStartTime > LOCK_DURATION)) {
      targetLocked = false; tft.fillRect(185, 65, 135, 20, 0x0000);
      digitalWrite(LED_GREEN, LOW); servoB.write(90);
    }
  } else {
    targetLocked = false; playTone(0); digitalWrite(LED_RED, LOW); digitalWrite(LED_GREEN, HIGH);
    servoB.write(90);
    
    if (sweepingForward) { if (++currentAngle >= 115) sweepingForward = false; } 
    else { if (--currentAngle <= 65) sweepingForward = true; }
    
    servoA.write(currentAngle);
    delay(10); 

    if (millis() % 500 < 50) tft.fillRect(185, 65, 135, 20, 0x0000);
  }

  drawFan(lastAngle, 0x0000);
  drawFan(currentAngle, BRIGHT_GREEN);
  if (currentAngle % 15 == 0) drawRadarGUI();
  lastAngle = currentAngle;
}
