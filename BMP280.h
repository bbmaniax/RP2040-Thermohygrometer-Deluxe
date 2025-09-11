// BMP280

#ifndef BMP280_H
#define BMP280_H

#include <Arduino.h>

class AbstractWire;

class BMP280 {
public:
  BMP280(AbstractWire& wire);

  bool begin();
  bool measure(float* pressure, float* temperature);

private:
  struct CalibrationData {
    uint16_t dig_T1;
    int16_t dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
  };

  AbstractWire& wire;
  CalibrationData cal;

  bool writeReg(uint8_t reg, uint8_t value);
  bool readReg(uint8_t reg, uint8_t* value);
  bool readRegs(uint8_t reg, uint8_t* data, uint8_t length);
  bool readCalibrationData(CalibrationData* calData);
  bool readRawData(uint8_t reg, uint32_t* value);
};

#endif
