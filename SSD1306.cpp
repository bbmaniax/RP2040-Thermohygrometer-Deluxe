// SSD1306

#include <Arduino.h>

#include "AbstractWire.h"
#include "DebugSerial.h"
#include "SSD1306.h"

#define SSD1306_ADDRESS 0x3C
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D

SSD1306::SSD1306(AbstractWire& i2c, size_t width, size_t height) : i2c(i2c), width(width), height(height) {}

bool SSD1306::begin() {
  if (!writeCommand(SSD1306_DISPLAYOFF)) return false;
  if (!writeCommand(SSD1306_SETDISPLAYCLOCKDIV)) return false;
  if (!writeCommand(0x80)) return false;
  if (!writeCommand(SSD1306_SETMULTIPLEX)) return false;
  if (!writeCommand(height - 1)) return false;
  if (!writeCommand(SSD1306_SETDISPLAYOFFSET)) return false;
  if (!writeCommand(0x00)) return false;
  if (!writeCommand(SSD1306_SETSTARTLINE | 0x00)) return false;
  if (!writeCommand(SSD1306_CHARGEPUMP)) return false;
  if (!writeCommand(0x14)) return false;
  if (!writeCommand(SSD1306_MEMORYMODE)) return false;
  if (!writeCommand(0x00)) return false;
  if (!writeCommand(SSD1306_SEGREMAP | 0x01)) return false;
  if (!writeCommand(SSD1306_COMSCANDEC)) return false;
  if (!writeCommand(SSD1306_SETCOMPINS)) return false;

  uint8_t comPinConfig = (height <= 32) ? 0x02 : 0x12;
  if (!writeCommand(comPinConfig)) return false;

  if (!writeCommand(SSD1306_SETCONTRAST)) return false;
  if (!writeCommand(0x8F)) return false;
  if (!writeCommand(SSD1306_SETPRECHARGE)) return false;
  if (!writeCommand(0xF1)) return false;
  if (!writeCommand(SSD1306_SETVCOMDETECT)) return false;
  if (!writeCommand(0x40)) return false;
  if (!writeCommand(SSD1306_DISPLAYALLON_RESUME)) return false;
  if (!writeCommand(SSD1306_NORMALDISPLAY)) return false;
  if (!writeCommand(SSD1306_DISPLAYON)) return false;
  delay(100);

  return true;
}

bool SSD1306::display(uint8_t* buffer) {
  if (!writeCommand(SSD1306_COLUMNADDR)) return false;
  if (!writeCommand(0)) return false;
  if (!writeCommand(width - 1)) return false;
  if (!writeCommand(SSD1306_PAGEADDR)) return false;
  if (!writeCommand(0)) return false;
  if (!writeCommand((height / 8) - 1)) return false;

  uint16_t bufferSize = width * height / 8;

  i2c.beginTransmission(SSD1306_ADDRESS);
  i2c.write(0x40);
  for (uint16_t i = 0; i < bufferSize; i++) { i2c.write(buffer[i]); }

  return i2c.endTransmission() == 0;
}

bool SSD1306::writeCommand(uint8_t cmd) {
  i2c.beginTransmission(SSD1306_ADDRESS);
  i2c.write(0x00);
  i2c.write(cmd);

  return i2c.endTransmission() == 0;
}

bool SSD1306::writeData(uint8_t data) {
  i2c.beginTransmission(SSD1306_ADDRESS);
  i2c.write(0x40);
  i2c.write(data);

  return i2c.endTransmission() == 0;
}
