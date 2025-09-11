// SD1306Display

#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <Arduino.h>

class SSD1306;

class SSD1306Display {
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
  void clearDisplay();
  void display();
  void setRotation(uint8_t rotation);
  bool setTextSize(uint8_t size);
  bool setTextColor(uint16_t color);
  bool setCursor(int16_t x, int16_t y);
  size_t print(const String& s);
  size_t printf(const char* format, ...);
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

private:
  SSD1306& ssd1306;
  size_t width;
  size_t height;
  uint8_t* buffer;
  Context ctx;
};

#endif  // __SSD1306_DISPLAY_H__
