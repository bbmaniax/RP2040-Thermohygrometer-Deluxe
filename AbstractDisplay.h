// AbstractDisplay

#ifndef __ABSTRACT_DISPLAY_H__
#define __ABSTRACT_DISPLAY_H__

#include <Arduino.h>

class AbstractDisplay {
public:
  virtual ~AbstractDisplay() = default;

  virtual size_t getWidth() const = 0;
  virtual size_t getHeight() const = 0;

  virtual void display(void) = 0;
  virtual void clearDisplay(void) = 0;
  virtual void setRotation(uint8_t rotation) = 0;
  virtual bool setTextSize(uint8_t size) = 0;
  virtual bool setTextColor(uint16_t color) = 0;
  virtual bool setCursor(int16_t x, int16_t y) = 0;
  virtual size_t print(const String& s) = 0;
  virtual size_t printf(const char* format, ...) = 0;
  virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) = 0;
  };

#endif  // __ABSTRACT_DISPLAY_H__
