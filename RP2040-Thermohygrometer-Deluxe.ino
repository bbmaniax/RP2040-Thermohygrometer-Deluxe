// RP2040-Thermohygrometer-Deluxe

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

#include "AHTX0.h"
#include "BMP280.h"
#include "Button.h"
#include "DebugSerial.h"
#include "EventManager.h"
#include "GND.h"
#include "History.h"
#include "Model.h"
#include "SensorManager.h"
#include "SoftI2C.h"
#include "SoftI2CWire.h"
#include "SSD1306.h"
#include "SSD1306Display.h"
#include "TimeKeeper.h"
#include "View.h"

#define BUTTON1_GND_PIN 0
#define BUTTON2_GND_PIN 3
#define BUTTON1_INPUT_PIN 2
#define BUTTON2_INPUT_PIN 5
#define I2C1_SDA_PIN 29
#define I2C1_SCL_PIN 28
#define I2C2_SDA_PIN 4
#define I2C2_SCL_PIN 6
#define I2C3_SDA_PIN 7
#define I2C3_SCL_PIN 8
#define DISPLAY_I2C_ADDRESS 0x3C
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32

#define HISTORY_BUFFER_SIZE (DISPLAY_WIDTH / (PLOT_HORIZONTAL_SPACING + 1) + PLOT_HORIZONTAL_SPACING)
#define SENSOR_READ_INTERVAL_MS 3000
#define PLOT_HORIZONTAL_SPACING 1

uint8_t displayBuffer1[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];
uint8_t displayBuffer2[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];
uint8_t displayBuffer3[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];
int16_t temperatureHistoryBuffer[HISTORY_BUFFER_SIZE];
int16_t humidityHistoryBuffer[HISTORY_BUFFER_SIZE];
int16_t pressureHistoryBuffer[HISTORY_BUFFER_SIZE];
History temperatureHistory(temperatureHistoryBuffer, HISTORY_BUFFER_SIZE);
History humidityHistory(humidityHistoryBuffer, HISTORY_BUFFER_SIZE);
History pressureHistory(pressureHistoryBuffer, HISTORY_BUFFER_SIZE);

SoftI2C softI2C1(I2C1_SDA_PIN, I2C1_SCL_PIN);
SoftI2C softI2C2(I2C2_SDA_PIN, I2C2_SCL_PIN);
SoftI2C softI2C3(I2C3_SDA_PIN, I2C3_SCL_PIN);
SoftI2CWire wire1(softI2C1);
SoftI2CWire wire2(softI2C2);
SoftI2CWire wire3(softI2C3);
GND gnd1(BUTTON1_GND_PIN);
GND gnd2(BUTTON2_GND_PIN);
Button button1(BUTTON1_INPUT_PIN);
Button button2(BUTTON2_INPUT_PIN);
TimeKeeper timeKeeper1(SENSOR_READ_INTERVAL_MS);
AHTX0 thermometer(wire1);
BMP280 barometer(wire1);
SSD1306 oled1(wire1, DISPLAY_WIDTH, DISPLAY_HEIGHT);
SSD1306 oled2(wire2, DISPLAY_WIDTH, DISPLAY_HEIGHT);
SSD1306 oled3(wire3, DISPLAY_WIDTH, DISPLAY_HEIGHT);
SSD1306Display display1(oled1, DISPLAY_WIDTH, DISPLAY_HEIGHT, displayBuffer1);
SSD1306Display display2(oled2, DISPLAY_WIDTH, DISPLAY_HEIGHT, displayBuffer2);
SSD1306Display display3(oled3, DISPLAY_WIDTH, DISPLAY_HEIGHT, displayBuffer3);

EventManager eventManager(button1, button2, timeKeeper1);
SensorManager sensorManager(thermometer, barometer);

Model model(temperatureHistory, humidityHistory, pressureHistory);
View view(model, display1, display2, display3, DISPLAY_WIDTH, DISPLAY_HEIGHT);

void setup() {
  DEBUG_SERIAL_BEGIN(9600);
  DEBUG_SERIAL_WAIT_FOR();
  DEBUG_SERIAL_PRINTLN();
  DEBUG_SERIAL_PRINTLN("--");
  DEBUG_SERIAL_PRINTLN("Thermohygrometer Deluxe");

  gnd1.begin();
  gnd2.begin();

  wire1.begin();
  wire2.begin();
  wire3.begin();
  delay(100);

  sensorManager.begin();
  eventManager.begin();

  SensorManager::SensorData sensorData;
  sensorManager.readSensorData(&sensorData);

  model.begin(sensorData.temperature, sensorData.humidity, sensorData.pressure);
  view.begin(DISPLAY_I2C_ADDRESS);
  delay(1000);

  if (digitalRead(BUTTON2_INPUT_PIN) == LOW) {
    scan(wire1, display1);
    scan(wire2, display2);
    scan(wire3, display3);
    while (digitalRead(BUTTON2_INPUT_PIN) == LOW) {}
  }
}

void loop() {
  eventManager.update();

  static bool needUpdate = true;

  if (eventManager.getTimeKeeper(0)->isTimeUp()) {
    // DEBUG_SERIAL_PRINTLN("Time to read sensors");
    SensorManager::SensorData sensorData;
    sensorManager.readSensorData(&sensorData);
    model.update(sensorData.temperature, sensorData.humidity, sensorData.pressure);
    eventManager.getTimeKeeper(0)->reset();
    needUpdate = true;
  }

  if (eventManager.getButton(0)->isClicked()) {
    DEBUG_SERIAL_PRINTLN("Button 1 clicked");
    view.flip();
    needUpdate = true;
  }

  if (eventManager.getButton(1)->isClicked()) {
    DEBUG_SERIAL_PRINTLN("Button 2 clicked");
    view.switchToNextViewMode();
    needUpdate = true;
  }

  if (needUpdate) {
    // DEBUG_SERIAL_PRINTLN("Time to render views");
    view.render();
    needUpdate = false;
  }

  delay(10);
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
