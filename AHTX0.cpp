// AHTX0

#include <Arduino.h>

#include "AbstractWire.h"
#include "AHTX0.h"
#include "DebugSerial.h"

#define AHTX0_I2C_ADDRESS 0x38
#define AHTX0_CMD_SOFTRESET 0xBA
#define AHTX0_CMD_CALIBRATE 0xE1
#define AHTX0_CMD_TRIGGER 0xAC
#define AHTX0_STATUS_BUSY 0x80
#define AHTX0_STATUS_CALIBRATED 0x08

AHTX0::AHTX0(AbstractWire& wire) : wire(wire) {}

bool AHTX0::begin() {
  delay(20);

  if (!softReset()) {
    DEBUG_SERIAL_PRINTLN("AHTX0::begin() - Failed to soft reset");
    return false;
  }
  delay(40);

  uint8_t status = getStatus();
  while (status & AHTX0_STATUS_BUSY) {
    delay(10);
    status = getStatus();
  }

  if (!(status & AHTX0_STATUS_CALIBRATED)) {
    if (!writeCommand(AHTX0_CMD_CALIBRATE, 0x08, 0x00)) {
      DEBUG_SERIAL_PRINTLN("AHTX0::begin() - Failed to calibrate");
      return false;
    }
    delay(10);

    status = getStatus();
    while (status & AHTX0_STATUS_BUSY) {
      delay(10);
      status = getStatus();
    }
  }

  if (!(status & AHTX0_STATUS_CALIBRATED)) {
    DEBUG_SERIAL_PRINTLN("AHTX0::begin() - Still not calibrated");
    return false;
  }

  return true;
}

bool AHTX0::measure(float* temperature, float* humidity) {
  if (!temperature && !humidity) { return true; }

  if (!writeCommand(AHTX0_CMD_TRIGGER, 0x33, 0x00)) {
    DEBUG_SERIAL_PRINTLN("AHTX0::measure() - Failed to trigger measurement!");
    return false;
  }
  delay(80);

  uint8_t data[6];
  if (!readData(data, 6)) {
    DEBUG_SERIAL_PRINTLN("AHTX0::measure() - Failed to read data!");
    return false;
  }

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
  wire.beginTransmission(AHTX0_I2C_ADDRESS);
  wire.write(cmd);
  wire.write(data1);
  wire.write(data2);
  uint8_t result = wire.endTransmission();
  if (result != 0) {
    DEBUG_SERIAL_PRINT("AHTX0::writeCommand() - I2C error: ");
    DEBUG_SERIAL_PRINTLN(result);
  }
  return result == 0;
}

bool AHTX0::readData(uint8_t* data, uint8_t length) {
  uint8_t requested = wire.requestFrom((uint8_t)AHTX0_I2C_ADDRESS, (uint8_t)length);
  if (requested != length) {
    DEBUG_SERIAL_PRINTLN("AHTX0::readData() - Failed to request data!");
    return false;
  }

  if (wire.available() != length) {
    DEBUG_SERIAL_PRINTLN("AHTX0::readData() - Failed to read data!");
    return false;
  }

  for (uint8_t i = 0; i < length; i++) { data[i] = wire.read(); }

  bool isValid = ((data[0] & 0x80) == 0) && !(data[0] == 0xFF && data[1] == 0xFF && data[2] == 0xFF);
  if (!isValid) {
    DEBUG_SERIAL_PRINTLN("AHTX0::readData() - Invalid data received!");
  }
  return isValid;
}

uint8_t AHTX0::getStatus() {
  uint8_t requested = wire.requestFrom((uint8_t)AHTX0_I2C_ADDRESS, (uint8_t)1);
  if (requested != 1) {
    DEBUG_SERIAL_PRINT("AHTX0::getStatus() - Failed to request data, got: ");
    DEBUG_SERIAL_PRINTLN(requested);
    return 0xFF;
  }

  if (wire.available() != 1) {
    DEBUG_SERIAL_PRINT("AHTX0::getStatus() - Wrong data available: ");
    DEBUG_SERIAL_PRINTLN(wire.available());
    return 0xFF;
  }

  return wire.read();
}

bool AHTX0::softReset() {
  wire.beginTransmission(AHTX0_I2C_ADDRESS);
  wire.write(AHTX0_CMD_SOFTRESET);

  uint8_t result = wire.endTransmission();
  if (result != 0) {
    DEBUG_SERIAL_PRINT("AHTX0::softReset() - I2C error: ");
    DEBUG_SERIAL_PRINTLN(result);
    return false;
  }

  return true;
}
