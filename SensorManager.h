// SensorManager

#ifndef __SENSOR_MANAGER_H__
#define __SENSOR_MANAGER_H__

#include <Arduino.h>

class AHTX0;
class BMP280;

class SensorManager {
public:
  SensorManager(AHTX0& thermometer, BMP280& barometer);

  struct SensorData {
    int16_t temperature;
    int16_t humidity;
    int16_t pressure;
  };

  void begin();
  void acquire(SensorData* values);

private:
  AHTX0& thermometer;
  BMP280& barometer;
};

#endif  // __SENSOR_MANAGER_H__
