#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// HC-SR04
#define TRIG_PIN 27
#define ECHO_PIN 14
// VCC - 3V3
// GND - GND

// OLED
// VCC - 3V3
// GND - GND
// SCL - 22
// SDA - 21

int waveStep = 0;
const int maxRadius = 60;
const int waveSpacing = 5;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED gagal");
    while (1);
  }

  display.clearDisplay();
  display.display();
}

long readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;

  return duration * 0.034 / 2;
}

void drawWiFiWave(int centerX, int centerY, int radius) {
  for (int angle = 45; angle <= 135; angle++) {
    float rad = radians(angle);
    int x = centerX + cos(rad) * radius;
    int y = centerY - sin(rad) * radius;
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
      display.drawPixel(x, y, WHITE);
  }
}

void drawTransmitterCircle(int centerX, int centerY) {
  display.fillCircle(centerX, centerY, 2, WHITE);
}

void drawSingleDetectedDot(int centerX, int centerY, long distance) {
  int radius = map(distance, 0, 50, 10, maxRadius);
  if (radius < 10) radius = 10;
  if (radius > maxRadius) radius = maxRadius;

  float angle = radians(90); // lurus ke atas
  int x = centerX + cos(angle) * radius;
  int y = centerY - sin(angle) * radius;
  if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
    display.fillCircle(x, y, 2, WHITE);
}

void loop() {
  display.clearDisplay();

  int cx = SCREEN_WIDTH / 2;
  int cy = SCREEN_HEIGHT - 2;

  drawTransmitterCircle(cx, cy);

  for (int r = waveSpacing * waveStep; r <= maxRadius; r += waveSpacing * 5) {
    drawWiFiWave(cx, cy, r);
  }

  long distance = readDistanceCM();
  Serial.print("Jarak: "); Serial.println(distance);

  if (distance > 0 && distance <= 50) {
    drawSingleDetectedDot(cx, cy, distance);
  }

  display.display();
  delay(200);

  waveStep++;
  if (waveStep >= 5) waveStep = 0;
}