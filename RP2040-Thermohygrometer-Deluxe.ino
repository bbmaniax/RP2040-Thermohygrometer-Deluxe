// RP2040-Thermohygrometer-Deluxe

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>

#include "AHTX0.h"
#include "BMP280.h"
#include "Button.h"
#include "DebugSerial.h"
#include "History.h"
#include "Model.h"
#include "SensorManager.h"
#include "SoftI2C.h"
#include "SoftI2CWire.h"
#include "SSD1306.h"
#include "SSD1306Display.h"
#include "TimeKeeper.h"
#include "View.h"

#define RGBLED_PIN 16
#define BUTTON_PIN 29
#define I2C1_SDA_PIN 28
#define I2C1_SCL_PIN 14
#define I2C2_SDA_PIN 5
#define I2C2_SCL_PIN 6
#define I2C3_SDA_PIN 7
#define I2C3_SCL_PIN 8
#define DISPLAY_I2C_ADDRESS 0x3C
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32

#define SENSOR_READ_INTERVAL_MS 3000
#define PLOT_HORIZONTAL_SPACING 1

#define HISTORY_BUFFER_SIZE (DISPLAY_WIDTH / (PLOT_HORIZONTAL_SPACING + 1) + PLOT_HORIZONTAL_SPACING)

SensorManager::SensorData sensorData;
uint8_t displayBuffer1[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];
uint8_t displayBuffer2[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];
uint8_t displayBuffer3[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];
int16_t temperatureHistoryBuffer[HISTORY_BUFFER_SIZE];
int16_t humidityHistoryBuffer[HISTORY_BUFFER_SIZE];
int16_t pressureHistoryBuffer[HISTORY_BUFFER_SIZE];
History temperatureHistory(temperatureHistoryBuffer, HISTORY_BUFFER_SIZE);
History humidityHistory(humidityHistoryBuffer, HISTORY_BUFFER_SIZE);
History pressureHistory(pressureHistoryBuffer, HISTORY_BUFFER_SIZE);

Adafruit_NeoPixel rgbled(1, RGBLED_PIN, NEO_GRB + NEO_KHZ800);
Button button(BUTTON_PIN);
SoftI2C softI2C1(I2C1_SDA_PIN, I2C1_SCL_PIN);
SoftI2C softI2C2(I2C2_SDA_PIN, I2C2_SCL_PIN);
SoftI2C softI2C3(I2C3_SDA_PIN, I2C3_SCL_PIN);
SoftI2CWire wire1(softI2C1);
SoftI2CWire wire2(softI2C2);
SoftI2CWire wire3(softI2C3);
AHTX0 thermometer(wire1);
BMP280 barometer(wire1);
SSD1306 oled1(wire1, DISPLAY_WIDTH, DISPLAY_HEIGHT);
SSD1306 oled2(wire2, DISPLAY_WIDTH, DISPLAY_HEIGHT);
SSD1306 oled3(wire3, DISPLAY_WIDTH, DISPLAY_HEIGHT);
SSD1306Display display1(oled1, DISPLAY_WIDTH, DISPLAY_HEIGHT, displayBuffer1);
SSD1306Display display2(oled2, DISPLAY_WIDTH, DISPLAY_HEIGHT, displayBuffer2);
SSD1306Display display3(oled3, DISPLAY_WIDTH, DISPLAY_HEIGHT, displayBuffer3);
SensorManager sensorManager(thermometer, barometer);
TimeKeeper timeKeeper(SENSOR_READ_INTERVAL_MS);

Model model(temperatureHistory, humidityHistory, pressureHistory);
View view(model, display1, display2, display3, DISPLAY_WIDTH, DISPLAY_HEIGHT);

void setup() {
  rgbled.begin();
  rgbled.setBrightness(127);
  blink(160, 160, 0, 200);
  blink(160, 160, 0, 200);
  blink(160, 160, 0, 200);

  Serial.begin(9600);
  while (!Serial && millis() < 1000) {}
  Serial.println();
  Serial.println("--");
  Serial.println("Thermohygrometer Deluxe (build " + timestamp() + ")");

  button.begin();
  wire1.begin();
  wire2.begin();
  wire3.begin();
  delay(100);

  sensorManager.begin();
  timeKeeper.begin();
  model.begin(sensorData.temperature, sensorData.humidity, sensorData.pressure);
  view.begin(DISPLAY_I2C_ADDRESS);

  sensorManager.acquire(&sensorData);
  delay(1000);

  if (digitalRead(BUTTON_PIN) == LOW) {
    scan(wire1, display1);
    scan(wire2, display2);
    scan(wire3, display3);
    while (digitalRead(BUTTON_PIN) == LOW) {}
  }
}

void loop() {
  button.update();
  timeKeeper.update();

  static bool needUpdate = true;

  if (timeKeeper.isTimeUp()) {
    // DEBUG_SERIAL_PRINTLN("Time to read sensors");
    sensorManager.acquire(&sensorData);
    model.update(sensorData.temperature, sensorData.humidity, sensorData.pressure);
    timeKeeper.reset();
    needUpdate = true;
  }

  if (button.isLongPressed()) {
    // DEBUG_SERIAL_PRINTLN("Button 1 long pressed");
    view.flip();
    needUpdate = true;
  }

  if (button.isClicked()) {
    // DEBUG_SERIAL_PRINTLN("Button 1 clicked");
    view.switchToNextViewMode();
    needUpdate = true;
  }

  if (needUpdate) {
    Serial.println("T:" + String(sensorData.temperature / 10.0f, 1) + " H:" + String(sensorData.humidity / 10.0f, 1) + " P:" + String(sensorData.pressure / 10.0f, 1));
    view.render();
    needUpdate = false;
  }

  delay(10);
}

void blink(uint8_t r, uint8_t g, uint8_t b, unsigned long durationMs) {
  rgbled.setPixelColor(0, r, g, b);
  rgbled.show();
  delay(durationMs);
  rgbled.setPixelColor(0, 0, 0, 0);
  rgbled.show();
  delay(durationMs);
}

String timestamp() {
  const char* m = "JanFebMarAprMayJunJulAugSepOctNovDec";
  char mon[4];
  int d, y, hh, mm, ss;
  sscanf(__DATE__, "%3s %d %d", mon, &d, &y);
  sscanf(__TIME__, "%d:%d:%d", &hh, &mm, &ss);
  int mo = (strstr(m, mon) - m) / 3 + 1;

  char buf[16];
  sprintf(buf, "%04d%02d%02d.%02d%02d%02d", y, mo, d, hh, mm, ss);
  return String(buf);
}

void scan(AbstractWire& wire, SSD1306Display& display) {
  DEBUG_SERIAL_PRINTLN("SCANNING");
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("SCANNING");
  display.display();
  display.setCursor(0, 16);

  uint8_t deviceCount = 0;
  for (uint8_t address = 1; address < 127; address++) {
    wire.beginTransmission(address);
    uint8_t error = wire.endTransmission();
    if (error == 0) {
      if (address < 16) DEBUG_SERIAL_PRINT("0");
      DEBUG_SERIAL_PRINTHEX(address);
      DEBUG_SERIAL_PRINT(" ");
      if (deviceCount == 0) {
        display.clearDisplay();
        display.setCursor(0, 0);
      }
      display.printf("%X ", address);
      display.display();
      deviceCount++;
    }
    delay(10);
  }

  if (deviceCount == 0) {
    DEBUG_SERIAL_PRINTLN("NO DEVICES");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("NO DEVICES");
    display.display();
  }
}
