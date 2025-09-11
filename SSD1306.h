/*
  SSD1306 OLED Display Library

  A library for interfacing with SSD1306 OLED displays
  over I2C communication.
*/

#ifndef __SSD1306_H__
#define __SSD1306_H__

#include <Arduino.h>

#include "screen.h"

class AbstractWire;

class SSD1306 {
public:
  SSD1306(AbstractWire& i2c, Screen& screen)
    : i2c(i2c), screen(screen) {}
  bool begin();
  bool display();

private:
  AbstractWire& i2c;
  Screen& screen;

  bool writeCommand(uint8_t cmd);
  bool writeData(uint8_t data);
};

#endif  // __SSD1306_H__
