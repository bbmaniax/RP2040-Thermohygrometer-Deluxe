/*
  SD1306Display Buffer Library

  Implementation of a framebuffer for drawing graphics
  and text to bitmap displays.
*/

#include <Arduino.h>

#include "Font.h"
#include "DebugSerial.h"

SD1306Display::SD1306Display(size_t width, size_t height, uint8_t* buffer) : width(width), height(height), buffer(buffer) {}

SD1306Display::~SD1306Display() {}

uint8_t SD1306Display::getWidth() const {
  return width;
}

uint8_t SD1306Display::getHeight() const {
  return height;
}

uint8_t* SD1306Display::getBuffer() const {
  return buffer;
}

bool SD1306Display::begin() {
  return buffer != nullptr;
}

void SD1306Display::clearDisplay() {
  memset(buffer, 0, width * height / 8);
}

void SD1306Display::display() {
  SERIAL_PRINTLN("Display buffer updated.");
}

void SD1306Display::drawString(uint8_t page, int16_t x, uint8_t size, const char* str) {
  if (size >= height / 8) return;
  if (page >= height / 8) return;

  uint8_t scale = size + 1;
  int16_t pos = x;
  uint8_t index = 0;
  char c;

  while ((c = str[index++]) != '\0' && pos < width) {
    uint8_t* fontData = getCharBytes(c);
    if (fontData) {
      for (uint8_t col = 0; col < 6; col++) {
        uint8_t fontByte = (col < 5) ? fontData[col] : 0x00;
        uint32_t expandedBits = 0;

        for (uint8_t bit = 0; bit < 8; bit++) {
          if (fontByte & (1 << bit)) {
            uint32_t mask = (1UL << scale) - 1;
            expandedBits |= mask << (bit * scale);
          }
        }

        for (uint8_t rep = 0; rep < scale; rep++) {
          int16_t targetX = pos + col * scale + rep;
          if (targetX >= 0 && targetX < width) {
            for (uint8_t pageOffset = 0; pageOffset < scale; pageOffset++) {
              uint8_t targetPage = page + pageOffset;
              if (targetPage >= height / 8) continue;

              uint8_t pageByte = (expandedBits >> (pageOffset * 8)) & 0xFF;
              uint16_t bufferIndex = targetX + targetPage * width;
              buffer[bufferIndex] = pageByte;
            }
          }
        }
      }
      pos += (6 * scale);
    }
  }
}

void SD1306Display::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
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
