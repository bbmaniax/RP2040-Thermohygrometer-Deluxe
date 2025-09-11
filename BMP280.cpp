/*
  BMP280 Pressure and Temperature Sensor Library

  Implementation for interfacing with BMP280 sensors
  over I2C communication.
*/

#include <Arduino.h>

#include "AbstractWire.h"
#include "BMP280.h"
#include "DebugSerial.h"

#define BMP280_I2C_ADDRESS 0x77
#define BMP280_REGISTER_CONTROL 0xF4
#define BMP280_REGISTER_PRESSUREDATA 0xF7
#define BMP280_REGISTER_TEMPDATA 0xFA
#define BMP280_REGISTER_DIG_T1 0x88
#define BMP280_REGISTER_DIG_P1 0x8E

bool BMP280::begin() {
  if (!writeReg(BMP280_REGISTER_CONTROL, 0x27)) {
    return false;
  }
  delay(100);

  return readCalibrationData(&cal);
}

bool BMP280::measure(float* pressure, float* temperature) {
  if (!pressure && !temperature) {
    return true;
  }

  uint32_t adc_T_raw;
  if (!readRawData(BMP280_REGISTER_TEMPDATA, &adc_T_raw)) {
    return false;
  }

  int32_t adc_T = adc_T_raw >> 4;
  int32_t var1 = ((((adc_T >> 3) - ((int32_t)cal.dig_T1 << 1))) * ((int32_t)cal.dig_T2)) >> 11;
  int32_t var2 = (((((adc_T >> 4) - ((int32_t)cal.dig_T1)) * ((adc_T >> 4) - ((int32_t)cal.dig_T1))) >> 12) * ((int32_t)cal.dig_T3)) >> 14;
  int32_t t_fine = var1 + var2;

  if (temperature) {
    *temperature = (float)t_fine / 5120.0;
  }

  if (pressure) {
    uint32_t adc_P_raw;
    if (!readRawData(BMP280_REGISTER_PRESSUREDATA, &adc_P_raw)) {
      return false;
    }

    int64_t var1_64 = ((int64_t)t_fine) - 128000;
    int64_t var2_64 = var1_64 * var1_64 * (int64_t)cal.dig_P6;
    var2_64 = var2_64 + ((var1_64 * (int64_t)cal.dig_P5) << 17);
    var2_64 = var2_64 + (((int64_t)cal.dig_P4) << 35);
    var1_64 = (((int64_t)cal.dig_P3) * (var1_64 * var1_64)) >> 8;
    var1_64 = var1_64 + (((int64_t)cal.dig_P2) * var1_64) >> 12;
    var1_64 = (((((int64_t)1) << 47) + var1_64)) * ((int64_t)cal.dig_P1) >> 33;
    if (var1_64 == 0) {
      return false;
    }

    int32_t adc_P = adc_P_raw >> 4;
    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2_64) * 3125) / var1_64;
    var1_64 = (((int64_t)cal.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2_64 = (((int64_t)cal.dig_P8) * p) >> 19;
    p = ((p + var1_64 + var2_64) >> 8) + (((int64_t)cal.dig_P7) << 4);
    *pressure = (float)p / 25600.0;
  }

  return true;
}

bool BMP280::writeReg(uint8_t reg, uint8_t value) {
  i2c.beginTransmission(BMP280_I2C_ADDRESS);
  i2c.write(reg);
  i2c.write(value);

  return i2c.endTransmission() == 0;
}

bool BMP280::readReg(uint8_t reg, uint8_t* value) {
  i2c.beginTransmission(BMP280_I2C_ADDRESS);
  i2c.write(reg);

  if (i2c.endTransmission() != 0) {
    return false;
  }

  if (i2c.requestFrom(BMP280_I2C_ADDRESS, 1) != 1) {
    return false;
  }

  if (i2c.available() != 1) {
    return false;
  }

  *value = i2c.read();
  return true;
}

bool BMP280::readRegs(uint8_t reg, uint8_t* data, uint8_t length) {
  i2c.beginTransmission(BMP280_I2C_ADDRESS);
  i2c.write(reg);

  if (i2c.endTransmission() != 0) {
    return false;
  }

  uint8_t requested = i2c.requestFrom((uint8_t)BMP280_I2C_ADDRESS, (uint8_t)length);
  if (requested != length) {
    return false;
  }

  if (i2c.available() != length) {
    return false;
  }

  for (uint8_t i = 0; i < length; i++) {
    data[i] = i2c.read();
  }

  return true;
}

bool BMP280::readCalibrationData(CalibrationData* calData) {
  uint8_t cal_data[24];
  if (!readRegs(BMP280_REGISTER_DIG_T1, cal_data, 6)) {
    return false;
  }

  calData->dig_T1 = (cal_data[1] << 8) | cal_data[0];
  calData->dig_T2 = (cal_data[3] << 8) | cal_data[2];
  calData->dig_T3 = (cal_data[5] << 8) | cal_data[4];

  if (!readRegs(BMP280_REGISTER_DIG_P1, cal_data, 18)) {
    return false;
  }

  calData->dig_P1 = (cal_data[1] << 8) | cal_data[0];
  calData->dig_P2 = (cal_data[3] << 8) | cal_data[2];
  calData->dig_P3 = (cal_data[5] << 8) | cal_data[4];
  calData->dig_P4 = (cal_data[7] << 8) | cal_data[6];
  calData->dig_P5 = (cal_data[9] << 8) | cal_data[8];
  calData->dig_P6 = (cal_data[11] << 8) | cal_data[10];
  calData->dig_P7 = (cal_data[13] << 8) | cal_data[12];
  calData->dig_P8 = (cal_data[15] << 8) | cal_data[14];
  calData->dig_P9 = (cal_data[17] << 8) | cal_data[16];

  return true;
}

bool BMP280::readRawData(uint8_t reg, uint32_t* value) {
  i2c.beginTransmission(BMP280_I2C_ADDRESS);
  i2c.write(reg);

  if (i2c.endTransmission() != 0) {
    return false;
  }

  uint8_t requested = i2c.requestFrom(BMP280_I2C_ADDRESS, 3);
  if (requested != 3) {
    return false;
  }

  if (i2c.available() != 3) {
    return false;
  }

  *value = 0;
  *value = (uint32_t)i2c.read() << 16;
  *value |= (uint32_t)i2c.read() << 8;
  *value |= i2c.read();

  return true;
}
