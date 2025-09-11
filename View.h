// View - Manages the display of sensor data and charts.

#ifndef __APP_VIEW_H__
#define __APP_VIEW_H__

#include <Arduino.h>

class History;
class Model;
class SSD1306Display;

class View {
public:
  View(Model& model, SSD1306Display& display1, SSD1306Display& display2, SSD1306Display& display3, size_t width, size_t height, uint8_t horizontalSpacing = 1, bool flipped = false);

  void begin(uint8_t i2cAddress, bool displayOn = true);
  void switchToNextViewMode();
  void flip();
  void render();

private:
  typedef enum {
    VIEW_MODE_ALL_TEXT = 0,
    VIEW_MODE_ALL_CHARTS,
    VIEW_MODE_COUNT,
  } ViewMode;

  SSD1306Display** getDisplays();

  void renderAllText();
  void printRight(SSD1306Display& display, const String& text, int16_t y);

  void renderAllCharts();
  void renderTemperatureChart(SSD1306Display& display);
  void renderHumidityChart(SSD1306Display& display);
  void renderPressureChart(SSD1306Display& display);
  void drawChart(SSD1306Display& display, History& history);

  Model& model;
  ViewMode viewMode;
  SSD1306Display& display1;
  SSD1306Display& display2;
  SSD1306Display& display3;
  SSD1306Display* normalDisplays[3];
  SSD1306Display* flippedDisplays[3];
  size_t width;
  size_t height;
  uint8_t plotHorizontalStep;
  bool flipped;
  bool initialFlipped;
};

#endif  // __APP_VIEW_H__
