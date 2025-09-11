/*
  AHTX0 Temperature and Humidity Sensor Library

  A library for interfacing with AHTX0 sensors
  over I2C communication.
*/

#ifndef __AHTX0_H__
#define __AHTX0_H__

#include <Arduino.h>

class AbstractWire;

class AHTX0 {
public:
  AHTX0(AbstractWire& i2c);

public:
  bool begin();
  bool measure(float* temperature, float* humidity);

private:
  AbstractWire& i2c;

private:
  bool writeCommand(uint8_t cmd, uint8_t data1, uint8_t data2);
  bool readData(uint8_t* data, uint8_t length);
};

#endif  // __AHTX0_H__
