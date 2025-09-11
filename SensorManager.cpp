// SensorManager

#include <Arduino.h>

#include "AHTX0.h"
#include "BMP280.h"
#include "DebugSerial.h"
#include "SensorManager.h"

SensorManager::SensorManager(AHTX0& thermometer, BMP280& barometer) : thermometer(thermometer), barometer(barometer) {}

void SensorManager::begin() {
  // DEBUG_SERIAL_PRINTLN("Initializing SensorManager");
  if (!thermometer.begin()) { DEBUG_SERIAL_PRINTLN("Failed to initialize AHTX0!"); }
  if (!barometer.begin()) { DEBUG_SERIAL_PRINTLN("Failed to initialize BMP280!"); }
}

void SensorManager::readSensorData(SensorData* values) {
  // DEBUG_SERIAL_PRINTLN("Reading sensor data");
  float temperature, humidity;
  if (thermometer.measure(&temperature, &humidity)) {
    values->temperature = static_cast<int16_t>(temperature * 10.0f);
    values->humidity = static_cast<int16_t>(humidity * 10.0f);
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to read AHTX0!");
  }
  float pressure;
  if (barometer.measure(&pressure, nullptr)) {
    values->pressure = static_cast<int16_t>(pressure / 10.0f);
  } else {
    DEBUG_SERIAL_PRINTLN("Failed to read BMP280!");
  }
}
