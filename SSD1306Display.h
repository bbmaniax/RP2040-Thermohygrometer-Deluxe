// SD1306Display

#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <Arduino.h>

#include "AbstractDisplay.h"

class SSD1306;

class SSD1306Display : public AbstractDisplay {
public:
  SSD1306Display(SSD1306& ssd1306, size_t width, size_t height, uint8_t* buffer);

  struct Context {
    uint8_t textSize;
    uint16_t textColor;
    int16_t cursorX;
    int16_t cursorY;
  };

  size_t getWidth() const;
  size_t getHeight() const;

  bool begin();
  void clearDisplay() override;
  void display() override;
  void setRotation(uint8_t rotation) override;
  bool setTextSize(uint8_t size) override;
  bool setTextColor(uint16_t color) override;
  bool setCursor(int16_t x, int16_t y) override;
  size_t print(const String& s) override;
  size_t printf(const char* format, ...) override;
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) override;

private:
  SSD1306& ssd1306;
  size_t width;
  size_t height;
  uint8_t* buffer;
  Context ctx;
};

#endif  // __SSD1306_DISPLAY_H__
