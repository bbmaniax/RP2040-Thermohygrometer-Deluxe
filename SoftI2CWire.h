// SoftI2CWire

#ifndef __SOFT_I2C_WIRE_H__
#define __SOFT_I2C_WIRE_H__

#include <Arduino.h>

#include "AbstractWire.h"

class SoftI2C;

class SoftI2CWire: public AbstractWire {
public:
  SoftI2CWire(SoftI2C& softI2C);

public:
  void begin() ;
  void end() ;
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

private:
  SoftI2C& softI2C;
};

#endif // __SOFT_I2C_WIRE_H__
