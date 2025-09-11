/*
  SD1306Display Library

  A library for interfacing with SSD1306-based OLED displays.
*/

#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <Arduino.h>

class AbstractDisplay;

class SD13056Display: public AbstractDisplay {
public:
  SD13056Display(size_t width, size_t height, uint8_t* buffer);
  ~SD13056Display();

public:
  size_t getWidth() const override;
  size_t getHeight() const override;
  uint8_t* getBuffer() const;
  bool begin() override;
  bool clearDisplay() override;
  bool display() override;

private:
  size_t width;
  size_t height;
  uint8_t* buffer;
};

#endif  // __SSD1306_DISPLAY_H__
