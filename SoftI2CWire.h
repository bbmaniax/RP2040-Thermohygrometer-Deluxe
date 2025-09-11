/*
  SoftI2CWire Library
*/

#ifndef __SOFT_I2C_WIRE_H__
#define __SOFT_I2C_WIRE_H__

#include <Arduino.h>
#include <SoftI2C.h>
#include "AbstractWire.h"

class SoftI2CWire: public AbstractWire {
public:
  SoftI2CWire(uint8_t sdaPin, uint8_t sclPin);
  ~SoftI2CWire() override;

public:
  void begin() override;
  void begin(uint8_t address) override;
  void begin(int address) override;
  void end() override;
  void setClock(uint32_t clock) override;
  void setWireTimeout(uint32_t timeout = 25000, bool reset_with_timeout = false) override;
  bool getWireTimeoutFlag() override;
  void clearWireTimeoutFlag() override;
  void beginTransmission(uint8_t address) override;
  void beginTransmission(int address) override;
  uint8_t endTransmission() override;
  uint8_t endTransmission(uint8_t sendStop) override;
  uint8_t requestFrom(uint8_t address, uint8_t quantity) override;
  uint8_t requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop) override;
  uint8_t requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop) override;
  uint8_t requestFrom(int address, int quantity) override;
  uint8_t requestFrom(int address, int quantity, int sendStop) override;
  size_t write(uint8_t data) override;
  size_t write(const uint8_t* data, size_t quantity) override;
  size_t write(unsigned long n) override;
  size_t write(long n) override;
  size_t write(unsigned int n) override;
  size_t write(int n) override;
  int available() override;
  int read() override;
  int peek() override;
  void flush() override;
  void onReceive(void (*callback)(int)) override;
  void onRequest(void (*callback)()) override;

private:
  SoftI2C i2c;
};

#endif // __SOFT_I2C_WIRE_H__
