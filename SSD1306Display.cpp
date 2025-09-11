// SD1306Display

#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "DebugSerial.h"
#include "Font.h"
#include "SSD1306.h"
#include "SSD1306Display.h"

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0

SSD1306Display::SSD1306Display(SSD1306& ssd1306, size_t width, size_t height, uint8_t* buffer) : ssd1306(ssd1306), width(width), height(height), buffer(buffer) {}

size_t SSD1306Display::getWidth() const {
  return width;
}

size_t SSD1306Display::getHeight() const {
  return height;
}

bool SSD1306Display::begin() {
  ctx.cursorX = 0;
  ctx.cursorY = 0;
  ctx.textSize = 1;
  ctx.textColor = SSD1306_WHITE;
  return buffer != nullptr && ssd1306.begin();
}

void SSD1306Display::clearDisplay() {
  memset(buffer, 0, width * height / 8);
}

void SSD1306Display::display() {
  ssd1306.display(buffer);
}

void SSD1306Display::setRotation(uint8_t rotation) {
  if (rotation) {
    ssd1306.writeCommand(SSD1306_SEGREMAP | 0x00);
    ssd1306.writeCommand(SSD1306_COMSCANINC);
  } else {
    ssd1306.writeCommand(SSD1306_SEGREMAP | 0x01);
    ssd1306.writeCommand(SSD1306_COMSCANDEC);
  }
}

bool SSD1306Display::setTextSize(uint8_t size) {
  ctx.textSize = size;
  return true;
}

bool SSD1306Display::setTextColor(uint16_t color) {
  ctx.textColor = color;
  return true;
}

bool SSD1306Display::setCursor(int16_t x, int16_t y) {
  ctx.cursorX = x;
  ctx.cursorY = y;
  return true;
}

size_t SSD1306Display::print(const String& s) {
  int16_t x = ctx.cursorX;
  int16_t y = ctx.cursorY;
  uint8_t size = ctx.textSize;
  uint16_t basePage = y / 8;
  uint8_t yOffset = y % 8;
  const char* str = s.c_str();

  DEBUG_SERIAL_PRINTLN("print: x=" + String(x) + ", y=" + String(y) + ", basePage=" + String(basePage));

  if (size >= height / 8) return false;
  if (basePage >= height / 8) return false;

  uint8_t scale = size + 1;
  int16_t pos = x;
  uint8_t index = 0;
  char c;

  while ((c = str[index++]) != '\0' && pos < width) {
    const uint8_t* fontData = getCharBytes(c);
    if (fontData) {
      for (uint8_t col = 0; col < 6; col++) {
        uint8_t fontByte = (col < 5) ? pgm_read_byte(&fontData[col]) : 0x00;

        for (uint8_t rep = 0; rep < scale; rep++) {
          int16_t targetX = pos + col * scale + rep;
          if (targetX >= 0 && targetX < width) {

            for (uint8_t fontBit = 0; fontBit < 8; fontBit++) {
              if (fontByte & (1 << fontBit)) {
                for (uint8_t scaleY = 0; scaleY < scale; scaleY++) {
                  int16_t pixelY = y + fontBit * scale + scaleY;
                  if (pixelY >= 0 && pixelY < height) {
                    uint8_t targetPage = pixelY / 8;
                    uint8_t targetBit = pixelY % 8;
                    uint16_t bufferIndex = targetX + targetPage * width;
                    DEBUG_SERIAL_PRINTLN("Drawing: pixelY=" + String(pixelY) + ", targetPage=" + String(targetPage) + ", targetBit=" + String(targetBit));
                    buffer[bufferIndex] |= (1 << targetBit);
                  }
                }
              }
            }
          }
        }
      }
      pos += (6 * scale);
    }
  }
  return true;
}
size_t SSD1306Display::printf(const char* format, ...) {
  char buffer[256];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  return print(String(buffer));
}

void SSD1306Display::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  int16_t dx = abs(x1 - x0);
  int16_t dy = abs(y1 - y0);
  int16_t sx = (x0 < x1) ? 1 : -1;
  int16_t sy = (y0 < y1) ? 1 : -1;
  int16_t err = dx - dy;

  int16_t x = x0;
  int16_t y = y0;

  while (true) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
      uint8_t page = y / 8;
      uint8_t bitPos = y % 8;
      uint16_t bufferIndex = x + page * width;
      buffer[bufferIndex] |= (1 << bitPos);
    }

    if (x == x1 && y == y1) break;

    int16_t e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x += sx;
    }
    if (e2 < dx) {
      err += dx;
      y += sy;
    }
  }
}
