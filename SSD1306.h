// SSD1306

#ifndef __SSD1306_H__
#define __SSD1306_H__

#include <Arduino.h>

class AbstractWire;

class SSD1306 {
public:
  SSD1306(AbstractWire& i2c, size_t width, size_t height);

  bool begin();
  bool display(uint8_t* buffer);

  bool writeCommand(uint8_t cmd);
  bool writeData(uint8_t data);

private:
  AbstractWire& i2c;
  size_t width;
  size_t height;
};

#endif  // __SSD1306_H__
