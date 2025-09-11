/*
  SensorManager Library

  A library for managing sensor hardware communication
  and data acquisition.
*/

#include "SensorManager.h"

#include <Arduino.h>

#include "AHTX0.h"
#include "BMP280.h"
#include "DebugSerial.h"

SensorManager::SensorManager(AHTX0& thermometer, BMP280& barometer)
  : thermometer(thermometer), barometer(barometer) {}

bool SensorManager::begin() {
  if (!thermometer.begin()) {
    SERIAL_PRINTLN("Failed to initialize AHTX0!");
    return false;
  }
  if (!barometer.begin()) {
    SERIAL_PRINTLN("Failed to initialize BMP280!");
    return false;
  }

  return true;
}

bool SensorManager::readSensorData(SensorValues* values) {
  float temperatureValue, humidityValue;
  if (!temperature.measure(&temperatureValue, &humidityValue)) {
    SERIAL_PRINTLN("Failed to read AHTX0!");
    return false;
  }
  values->temperature = (int16_t)(temperatureValue * 10.0f);
  values->humidity = (int16_t)(humidityValue * 10.0f);

  float pressureValue;
  if (!barometer.measure(&pressureValue, nullptr)) {
    SERIAL_PRINTLN("Failed to read BMP280!");
    return false;
  }
  values->pressure = (int16_t)(pressure * 10.0f);

  return true;
}
