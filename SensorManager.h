/*
  SensorManager Library

  A library for managing sensor hardware communication
  and data acquisition.
*/

#ifndef __SENSOR_MANAGER_H__
#define __SENSOR_MANAGER_H__

#include <Arduino.h>

class AHTX0;
class BMP280;

struct SensorValues {
  int16_t temperature;
  int16_t humidity;
  int16_t pressure;
};

class SensorManager {
public:
  SensorManager(AHTX0& thermometer, BMP280& barometer);

public:
  bool begin();
  bool readSensorData(SensorValues* values);

private:
  AHTX0& thermometer;
  BMP280& barometer;
};

#endif // __SENSOR_MANAGER_H__
