/*
  Thermohygrometer Deluxe

  A multi-sensor thermohygrometer with OLED displays
  for temperature, humidity, and pressure monitoring.
*/

#include <Arduino.h>

#include "AHTX0.h"
#include "AppState.h"
#include "AppView.h"
#include "AppViewState.h"
#include "BMP280.h"
#include "Button.h"
#include "DebugSerial.h"
#include "DebugSerial.h"
#include "GND.h"
#include "History.h"
#include "SoftI2CWire.h"
#include "SSD1306.h"
#include "SSD1306.h"
#include "SSD1306Display.h"

#define BUTTON1_GND_PIN 2
#define BUTTON2_GND_PIN 8

#define BUTTON1_INPUT_PIN 0
#define BUTTON2_INPUT_PIN 6

#define BUS1_SDA_PIN 15
#define BUS1_SCL_PIN 14

#define BUS2_SDA_PIN 5
#define BUS2_SCL_PIN 7

#define BUS3_SDA_PIN 3
#define BUS3_SCL_PIN 4

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32

#define SENSOR_READ_INTERVAL_MS 3000
#define PLOT_HORIZONTAL_SPACING 1

GND gnd1(BUTTON1_GND_PIN);
GND gnd2(BUTTON2_GND_PIN);
Button button1(BUTTON1_INPUT_PIN);
Button button2(BUTTON2_INPUT_PIN);

SoftI2CWire bus1(BUS1_SDA_PIN, BUS1_SCL_PIN);
SoftI2CWire bus2(BUS2_SDA_PIN, BUS2_SCL_PIN);
SoftI2CWire bus3(BUS3_SDA_PIN, BUS3_SCL_PIN);

AHTX0 ahtx0(bus1);
BMP280 bmp280(bus1);
SSD1306 ssd13061(bus1);
SSD1306 ssd13062(bus2);
SSD1306 ssd13063(bus3);

SensorValues sensorValues;
SensorManager sensorManager(thermometer, barometer);

int16_t temperatureHistoryBuffer[DISPLAY_WIDTH / (PLOT_HORIZONTAL_SPACING + 1) + PLOT_HORIZONTAL_SPACING];
int16_t humidityHistoryBuffer[DISPLAY_WIDTH / (PLOT_HORIZONTAL_SPACING + 1) + PLOT_HORIZONTAL_SPACING];
int16_t pressureHistoryBuffer[DISPLAY_WIDTH / (PLOT_HORIZONTAL_SPACING + 1) + PLOT_HORIZONTAL_SPACING];
History temperatureHistory(temperatureHistoryBuffer, sizeof(temperatureHistoryBuffer) / sizeof(temperatureHistoryBuffer[0]));
History humidityHistory(humidityHistoryBuffer, sizeof(humidityHistoryBuffer) / sizeof(humidityHistoryBuffer[0]));
History pressureHistory(pressureHistoryBuffer, sizeof(pressureHistoryBuffer) / sizeof(pressureHistoryBuffer[0]));

int8_t displayBuffer[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];
SSD1306Display display1(DISPLAY_WIDTH, DISPLAY_HEIGHT, displayBuffer);
SSD1306Display display2(DISPLAY_WIDTH, DISPLAY_HEIGHT, displayBuffer);
SSD1306Display display3(DISPLAY_WIDTH, DISPLAY_HEIGHT, displayBuffer);

AppState appState(SENSOR_READ_INTERVAL_MS);
AppViewState appViewState;
AppView appView1(display1, DISPLAY_WIDTH, DISPLAY_HEIGHT, PLOT_HORIZONTAL_SPACING);
AppView appView2(display2, DISPLAY_WIDTH, DISPLAY_HEIGHT, PLOT_HORIZONTAL_SPACING);
AppView appView3(display3, DISPLAY_WIDTH, DISPLAY_HEIGHT, PLOT_HORIZONTAL_SPACING);

void setup() {
  SERIAL_BEGIN(115200);
  SERIAL_WAIT_FOR();
  SERIAL_PRINTLN();
  SERIAL_PRINTLN("--");

  appState.begin();
  appViewState.begin();

  gnd1.begin();
  gnd2.begin();
  button1.begin();
  button2.begin();

  bus1.begin();
  bus2.begin();
  bus3.begin();
  delay(100);

  if (!ahtx0.begin()) { SERIAL_PRINTLN("Failed to initialize AHTX0!"); }
  if (!bmp280.begin()) { SERIAL_PRINTLN("Failed to initialize BMP280!"); }
  if (!sensorManager.begin()) { SERIAL_PRINTLN("Failed to initialize SensorManager!"); }

  if (!sensorManager.readSensorData(&sensorValues)) {
    SERIAL_PRINTLN("Failed to read sensors!");
  } else {
    temperatureHistory.fill(sensorValues.temperature);
    humidityHistory.fill(sensorValues.humidity);
    pressureHistory.fill(sensorValues.pressure);
  }

  if (!appView1.begin()) { SERIAL_PRINTLN("Failed to initialize Display1!"); }
  if (!appView2.begin()) { SERIAL_PRINTLN("Failed to initialize Display2!"); }
  if (!appView3.begin()) { SERIAL_PRINTLN("Failed to initialize Display3!"); }

  SERIAL_PRINTLN("Thermohygrometer Deluxe");
}

void loop() {
  button1.update();
  button2.update();

  bool needDisplayUpdate = false;

  if (button1.isClicked()) {
    state.increaseDisplayPattern();
    needDisplayUpdate = true;
  }

  if (button2.isClicked()) {
    state.decreaseDisplayPattern();
    needDisplayUpdate = true;
  }

  if (!button1.isPressed() && !button2.isPressed() && state.shouldUpdateSensors()) {
    if (!readSensors()) {
      SERIAL_PRINTLN("Failed to read sensors!");
      goto EOL;
    }
    state.markSensorUpdateComplete();
    needDisplayUpdate = true;
  }

  if (needDisplayUpdate) { updateDisplays(); }

EOL:
  delay(10);
}

void abend() {
  while (true) { delay(100); }
}
