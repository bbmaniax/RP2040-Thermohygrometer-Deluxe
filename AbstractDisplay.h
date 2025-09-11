/*
  AbstractDisplay Library

  Abstract interface for display devices.
*/

#ifndef __ABSTRACT_DISPLAY_H__
#define __ABSTRACT_DISPLAY_H__

#include <Arduino.h>
class AbstractDisplay {
public:
  AbstractDisplay(size_t width, size_t height) = default;
  virtual ~AbstractDisplay() = default;

public:
  virtual size_t getWidth() const = 0;
  virtual size_t getHeight() const = 0;
  virtual bool begin() = 0;
  virtual bool clearDisplay() = 0;
  virtual bool display() = 0;
  virtual void setCursor
};

#endif // __ABSTRACT_DISPLAY_H__
