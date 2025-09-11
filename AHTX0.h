// AHTX0

#ifndef __AHTX0_H__
#define __AHTX0_H__

#include <Arduino.h>

class AbstractWire;

class AHTX0 {
public:
  AHTX0(AbstractWire& wire);

  bool begin();
  bool measure(float* temperature, float* humidity);

private:
  bool softReset();
  bool writeCommand(uint8_t cmd, uint8_t data1, uint8_t data2);
  bool readData(uint8_t* data, uint8_t length);
  uint8_t getStatus();

  AbstractWire& wire;
};

#endif  // __AHTX0_H__
