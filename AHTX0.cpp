/*
  AHTX0 Temperature and Humidity Sensor Library

  Implementation for interfacing with AHTX0 sensors
  over I2C communication.
*/

#include <Arduino.h>

#include "AbstractWire.h"
#include "AHTX0.h"
#include "DebugSerial.h"

#define AHTX0_I2C_ADDRESS 0x38
#define AHTX0_CMD_SOFTRESET 0xBA
#define AHTX0_CMD_CALIBRATE 0xBE
#define AHTX0_CMD_TRIGGER 0xAC

AHTX0::AHTX0(AbstractWire& i2c) : i2c(i2c) {}

bool AHTX0::begin() {
  if (!writeCommand(AHTX0_CMD_SOFTRESET, 0x00, 0x00)) { return false; }
  delay(20);

  if (!writeCommand(AHTX0_CMD_CALIBRATE, 0x08, 0x00)) { return false; }
  delay(10);

  return true;
}

bool AHTX0::measure(float* temperature, float* humidity) {
  if (!temperature && !humidity) { return true; }

  if (!writeCommand(AHTX0_CMD_TRIGGER, 0x33, 0x00)) { return false; }
  delay(80);

  uint8_t data[6];
  if (!readData(data, 6)) { return false; }

  if (temperature) {
    uint32_t raw_temp = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];
    *temperature = ((float)raw_temp / 1048576.0) * 200.0 - 50.0;
  }

  if (humidity) {
    uint32_t raw_hum = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
    *humidity = ((float)raw_hum * 100) / 1048576.0;
  }

  return true;
}

bool AHTX0::writeCommand(uint8_t cmd, uint8_t data1, uint8_t data2) {
  i2c.beginTransmission(AHTX0_I2C_ADDRESS);
  i2c.write(cmd);
  i2c.write(data1);
  i2c.write(data2);

  return i2c.endTransmission() == 0;
}

bool AHTX0::readData(uint8_t* data, uint8_t length) {
  uint8_t requested = i2c.requestFrom((uint8_t)AHTX0_I2C_ADDRESS, (uint8_t)length);
  if (requested != length) { return false; }

  if (i2c.available() != length) { return false; }

  for (uint8_t i = 0; i < length; i++) { data[i] = i2c.read(); }

  return ((data[0] & 0x80) == 0) && !(data[0] == 0xFF && data[1] == 0xFF && data[2] == 0xFF);
}
