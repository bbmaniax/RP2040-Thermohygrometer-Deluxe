// AbstractWire

#ifndef __ABSTRACT_WIRE_H__
#define __ABSTRACT_WIRE_H__

#include <Arduino.h>

class AbstractWire {
public:
  virtual ~AbstractWire() = default;

  virtual void beginTransmission(uint8_t address) = 0;
  virtual void beginTransmission(int address) = 0;
  virtual uint8_t endTransmission() = 0;
  virtual uint8_t endTransmission(uint8_t sendStop) = 0;
  virtual uint8_t requestFrom(uint8_t address, uint8_t quantity) = 0;
  virtual uint8_t requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop) = 0;
  virtual uint8_t requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop) = 0;
  virtual uint8_t requestFrom(int address, int quantity) = 0;
  virtual uint8_t requestFrom(int address, int quantity, int sendStop) = 0;
  virtual size_t write(uint8_t data) = 0;
  virtual size_t write(const uint8_t* data, size_t quantity) = 0;
  virtual size_t write(unsigned long n) = 0;
  virtual size_t write(long n) = 0;
  virtual size_t write(unsigned int n) = 0;
  virtual size_t write(int n) = 0;
  virtual int available() = 0;
  virtual int read() = 0;
  virtual int peek() = 0;
  virtual void flush() = 0;
};

#endif // __ABSTRACT_WIRE_H__
