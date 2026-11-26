#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "HX711.h"

// =====================================================
// OLED
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// ESP8266 NodeMCU
#define OLED_SDA D2
#define OLED_SCL D1

// =====================================================
// HX711
// =====================================================

#define HX711_DOUT D6
#define HX711_SCK  D5

HX711 scale;

// =====================================================
// CALIBRATION
// =====================================================

// Nilai awal.
// NANTI harus diganti berdasarkan hasil kalibrasi load cell.
float calibrationFactor = 420.52;

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("================================");
  Serial.println(" ESP8266 LOAD CELL SYSTEM");
  Serial.println("================================");

  // ---------------------------------------------------
  // OLED
  // ---------------------------------------------------

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {

    Serial.println("OLED : GAGAL");

    while (true) {
      delay(100);
    }
  }

  Serial.println("OLED : OK");

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("LOAD CELL");

  display.setTextSize(1);
  display.setCursor(0, 30);
  display.println("Initializing...");

  display.display();

  delay(1000);

  // ---------------------------------------------------
  // HX711
  // ---------------------------------------------------

  scale.begin(HX711_DOUT, HX711_SCK);

  Serial.println("HX711 : INITIALIZING");

  // Tunggu HX711 siap
  unsigned long startTime = millis();

  while (!scale.is_ready()) {

    if (millis() - startTime > 5000) {

      Serial.println("HX711 : GAGAL");

      display.clearDisplay();

      display.setTextSize(2);
      display.setCursor(0, 0);
      display.println("HX711");

      display.setTextSize(1);
      display.setCursor(0, 30);
      display.println("NOT READY");

      display.display();

      while (true) {
        delay(100);
      }
    }

    delay(100);
  }

  Serial.println("HX711 : OK");

  // ---------------------------------------------------
  // TARE
  // ---------------------------------------------------

  Serial.println();
  Serial.println("TARE...");
  Serial.println("Pastikan load cell tidak diberi beban.");

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("TARE");

  display.setTextSize(1);
  display.setCursor(0, 30);
  display.println("Remove load...");

  display.display();

  delay(2000);

  scale.tare();

  Serial.println("TARE : DONE");

  // ---------------------------------------------------
  // SET CALIBRATION
  // ---------------------------------------------------

  scale.set_scale(calibrationFactor);

  Serial.print("Calibration factor: ");
  Serial.println(calibrationFactor);

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("READY");

  display.setTextSize(1);
  display.setCursor(0, 30);
  display.println("Place a weight");

  display.display();

  delay(1500);
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  // Pastikan HX711 siap
  if (!scale.is_ready()) {

    Serial.println("HX711 : NOT READY");

    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("HX711");

    display.setTextSize(1);
    display.setCursor(0, 30);
    display.println("NOT READY");

    display.display();

    delay(500);

    return;
  }

  // ---------------------------------------------------
  // Baca berat
  // ---------------------------------------------------

  float weight = scale.get_units(10);

  // Jika nilai sangat kecil, anggap 0
  if (abs(weight) < 1.0) {
    weight = 0;
  }

  // ---------------------------------------------------
  // Serial Monitor
  // ---------------------------------------------------

  Serial.print("Weight: ");
  Serial.print(weight, 2);
  Serial.println(" g");

  // ---------------------------------------------------
  // OLED
  // ---------------------------------------------------

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("WEIGHT");

  display.setTextSize(3);
  display.setCursor(0, 18);
  display.print(weight, 1);

  display.setTextSize(1);
  display.setCursor(105, 30);
  display.println("g");

  display.display();

  delay(500);
}