// BMP280

#include <Arduino.h>

#include "AbstractWire.h"
#include "BMP280.h"
#include "DebugSerial.h"

#define BMP280_I2C_ADDRESS 0x77
#define BMP280_REGISTER_CHIPID 0xD0
#define BMP280_REGISTER_RESET 0xE0
#define BMP280_REGISTER_CONTROL 0xF4
#define BMP280_REGISTER_PRESSUREDATA 0xF7
#define BMP280_REGISTER_TEMPDATA 0xFA
#define BMP280_REGISTER_DIG_T1 0x88
#define BMP280_REGISTER_DIG_P1 0x8E
#define BMP280_CHIPID 0x58
#define BMP280_RESET_CMD 0xB6

BMP280::BMP280(AbstractWire& wire) : wire(wire) {}

bool BMP280::begin() {
  delay(10);

  uint8_t chipid;
  if (!readReg(BMP280_REGISTER_CHIPID, &chipid)) {
    DEBUG_SERIAL_PRINTLN("BMP280::begin() - Failed to read chip ID!");
    return false;
  }

  if (chipid != BMP280_CHIPID) {
    DEBUG_SERIAL_PRINT("BMP280::begin() - Wrong chip ID: 0x");
    DEBUG_SERIAL_PRINTLN(chipid);
    return false;
  }

  if (!writeReg(BMP280_REGISTER_RESET, BMP280_RESET_CMD)) {
    DEBUG_SERIAL_PRINTLN("BMP280::begin() - Failed to reset!");
    return false;
  }
  delay(10);

  if (!readCalibrationData(&cal)) {
    DEBUG_SERIAL_PRINTLN("BMP280::begin() - Failed to read calibration data!");
    return false;
  }

  if (!writeReg(BMP280_REGISTER_CONTROL, 0x27)) {
    DEBUG_SERIAL_PRINTLN("BMP280::begin() - Failed to write control register!");
    return false;
  }

  return true;
}

bool BMP280::measure(float* pressure, float* temperature) {
  if (!pressure && !temperature) {
    return true;
  }

  uint32_t adc_T_raw;
  if (!readRawData(BMP280_REGISTER_TEMPDATA, &adc_T_raw)) {
    DEBUG_SERIAL_PRINTLN("BMP280::measure() - Failed to read temperature data!");
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
      DEBUG_SERIAL_PRINTLN("BMP280::measure() - Failed to read pressure data!");
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
      DEBUG_SERIAL_PRINTLN("BMP280::measure() - Pressure calculation division by zero!");
      return false;
    }

    int32_t adc_P = adc_P_raw >> 4;
    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2_64) * 3125) / var1_64;
    var1_64 = (((int64_t)cal.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2_64 = (((int64_t)cal.dig_P8) * p) >> 19;
    p = ((p + var1_64 + var2_64) >> 8) + (((int64_t)cal.dig_P7) << 4);
    *pressure = (float)p / 256.0;
  }

  return true;
}

bool BMP280::writeReg(uint8_t reg, uint8_t value) {
  wire.beginTransmission(BMP280_I2C_ADDRESS);
  wire.write(reg);
  wire.write(value);

  uint8_t result = wire.endTransmission();
  if (result != 0) {
    DEBUG_SERIAL_PRINT("BMP280::writeReg() - I2C error: ");
    DEBUG_SERIAL_PRINTLN(result);
  }
  return result == 0;
}

bool BMP280::readReg(uint8_t reg, uint8_t* value) {
  wire.beginTransmission(BMP280_I2C_ADDRESS);
  wire.write(reg);

  uint8_t result = wire.endTransmission();
  if (result != 0) {
    DEBUG_SERIAL_PRINT("BMP280::readReg() - I2C transmission error: ");
    DEBUG_SERIAL_PRINTLN(result);
    return false;
  }

  uint8_t requested = wire.requestFrom(BMP280_I2C_ADDRESS, 1);
  if (requested != 1) {
    DEBUG_SERIAL_PRINT("BMP280::readReg() - Failed to request data, got: ");
    DEBUG_SERIAL_PRINTLN(requested);
    return false;
  }

  uint8_t available = wire.available();
  if (available != 1) {
    DEBUG_SERIAL_PRINT("BMP280::readReg() - Wrong data available: ");
    DEBUG_SERIAL_PRINTLN(available);
    return false;
  }

  *value = wire.read();
  return true;
}

bool BMP280::readRegs(uint8_t reg, uint8_t* data, uint8_t length) {
  wire.beginTransmission(BMP280_I2C_ADDRESS);
  wire.write(reg);

  uint8_t result = wire.endTransmission();
  if (result != 0) {
    DEBUG_SERIAL_PRINT("BMP280::readRegs() - I2C transmission error: ");
    DEBUG_SERIAL_PRINTLN(result);
    return false;
  }

  uint8_t requested = wire.requestFrom((uint8_t)BMP280_I2C_ADDRESS, (uint8_t)length);
  if (requested != length) {
    DEBUG_SERIAL_PRINT("BMP280::readRegs() - Failed to request data, got: ");
    DEBUG_SERIAL_PRINT(requested);
    DEBUG_SERIAL_PRINT(" expected: ");
    DEBUG_SERIAL_PRINTLN(length);
    return false;
  }

  uint8_t available = wire.available();
  if (available != length) {
    DEBUG_SERIAL_PRINT("BMP280::readRegs() - Wrong data available: ");
    DEBUG_SERIAL_PRINT(available);
    DEBUG_SERIAL_PRINT(" expected: ");
    DEBUG_SERIAL_PRINTLN(length);
    return false;
  }

  for (uint8_t i = 0; i < length; i++) {
    data[i] = wire.read();
  }

  return true;
}

bool BMP280::readCalibrationData(CalibrationData* calData) {
  uint8_t cal_data[24];
  if (!readRegs(BMP280_REGISTER_DIG_T1, cal_data, 6)) {
    DEBUG_SERIAL_PRINTLN("BMP280::readCalibrationData() - Failed to read temperature calibration data!");
    return false;
  }

  calData->dig_T1 = (cal_data[1] << 8) | cal_data[0];
  calData->dig_T2 = (cal_data[3] << 8) | cal_data[2];
  calData->dig_T3 = (cal_data[5] << 8) | cal_data[4];

  if (!readRegs(BMP280_REGISTER_DIG_P1, cal_data, 18)) {
    DEBUG_SERIAL_PRINTLN("BMP280::readCalibrationData() - Failed to read pressure calibration data!");
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
  wire.beginTransmission(BMP280_I2C_ADDRESS);
  wire.write(reg);

  uint8_t result = wire.endTransmission();
  if (result != 0) {
    DEBUG_SERIAL_PRINT("BMP280::readRawData() - I2C transmission error: ");
    DEBUG_SERIAL_PRINTLN(result);
    return false;
  }

  uint8_t requested = wire.requestFrom(BMP280_I2C_ADDRESS, 3);
  if (requested != 3) {
    DEBUG_SERIAL_PRINT("BMP280::readRawData() - Failed to request data, got: ");
    DEBUG_SERIAL_PRINTLN(requested);
    return false;
  }

  uint8_t available = wire.available();
  if (available != 3) {
    DEBUG_SERIAL_PRINT("BMP280::readRawData() - Wrong data available: ");
    DEBUG_SERIAL_PRINTLN(available);
    return false;
  }

  *value = 0;
  *value = (uint32_t)wire.read() << 16;
  *value |= (uint32_t)wire.read() << 8;
  *value |= wire.read();

  return true;
}
