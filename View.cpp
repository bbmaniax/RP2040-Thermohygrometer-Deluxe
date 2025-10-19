// View - Manages the display of sensor data and charts.

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

#include "DebugSerial.h"
#include "History.h"
#include "Model.h"
#include "SSD1306Display.h"
#include "View.h"

View::View(Model& model, SSD1306Display& display1, SSD1306Display& display2, SSD1306Display& display3, size_t width, size_t height, uint8_t horizontalSpacing, bool flipped)
    : model(model), viewMode(VIEW_MODE_ALL_TEXT), display1(display1), display2(display2), display3(display3), width(width), height(height), plotHorizontalStep(horizontalSpacing + 1), flipped(flipped), initialFlipped(flipped) {
  normalDisplays[0] = &display1;
  normalDisplays[1] = &display2;
  normalDisplays[2] = &display3;
  flippedDisplays[0] = &display3;
  flippedDisplays[1] = &display2;
  flippedDisplays[2] = &display1;
}

void View::begin(uint8_t i2cAddress, bool displayOn) {
  DEBUG_SERIAL_PRINTLN("Initializing display: displayOn=" + String(displayOn));
  viewMode = VIEW_MODE_ALL_CHARTS;
  flipped = initialFlipped;
  if (!display1.begin()) { DEBUG_SERIAL_PRINTLN("Failed to initialize display1"); }
  if (!display2.begin()) { DEBUG_SERIAL_PRINTLN("Failed to initialize display2"); }
  if (!display3.begin()) { DEBUG_SERIAL_PRINTLN("Failed to initialize display3"); }
  if (displayOn) {
    display1.display();
    display2.display();
    display3.display();
  }
}

void View::switchToNextViewMode() {
  DEBUG_SERIAL_PRINTLN("Switching to next view mode: current mode=" + String(static_cast<int>(viewMode)));
  viewMode = static_cast<ViewMode>((static_cast<int>(viewMode) + 1) % VIEW_MODE_COUNT);
  DEBUG_SERIAL_PRINTLN("New view mode=" + String(static_cast<int>(viewMode)));
}

void View::flip() {
  DEBUG_SERIAL_PRINTLN("Flipping display: current state=" + String(flipped));
  flipped = !flipped;
  DEBUG_SERIAL_PRINTLN("New flipped=" + String(flipped));
}

SSD1306Display** View::getDisplays() {
  return flipped ? flippedDisplays : normalDisplays;
}

void View::render() {
  // DEBUG_SERIAL_PRINTLN("Rendering view: mode=" + String(static_cast<int>(viewMode)) + ", flipped=" + String(flipped));
  switch (viewMode) {
    case VIEW_MODE_ALL_CHARTS: renderAllCharts(); break;
    case VIEW_MODE_ALL_TEXT: renderAllText(); break;
    default: renderAllText(); break;
  }
}

void View::renderAllCharts() {
  // DEBUG_SERIAL_PRINTLN("Rendering all charts");
  SSD1306Display** displays = getDisplays();

  renderTemperatureChart(*displays[0]);
  renderHumidityChart(*displays[1]);
  renderPressureChart(*displays[2]);
}

void View::renderTemperatureChart(SSD1306Display& display) {
  // DEBUG_SERIAL_PRINTLN("Rendering temperature chart");
  display.clearDisplay();
  display.setRotation(flipped ? 2 : 0);
  drawChart(display, model.getTemperatureHistory());
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(String(model.getLatestTemperature() / 10.0f, 1) + "C");
  display.display();
}

void View::renderHumidityChart(SSD1306Display& display) {
  // DEBUG_SERIAL_PRINTLN("Rendering humidity chart");
  display.clearDisplay();
  display.setRotation(flipped ? 2 : 0);
  drawChart(display, model.getHumidityHistory());
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(String(model.getLatestHumidity() / 10.0f, 1) + "%");
  display.display();
}

void View::renderPressureChart(SSD1306Display& display) {
  // DEBUG_SERIAL_PRINTLN("Rendering pressure chart");
  display.clearDisplay();
  display.setRotation(flipped ? 2 : 0);
  drawChart(display, model.getPressureHistory());
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(String(model.getLatestPressure() / 10.0f, 1) + "hPa");
  display.display();
}

void View::drawChart(SSD1306Display& display, History& history) {
  int16_t minValue = history.getMinValue();
  int16_t maxValue = history.getMaxValue();

  int16_t range = maxValue - minValue;
  if (range < 1) { range = 1; }

  uint8_t step = plotHorizontalStep;
  for (uint8_t i = 0; i < width - 1; i++) {
    int16_t currentValue = history.getValue(i);
    int16_t nextValue = history.getValue(i + 1);

    uint8_t currentY = 17 + (uint8_t)((maxValue - currentValue) * (height - 1 - 17) / range);
    uint8_t nextY = 17 + (uint8_t)((maxValue - nextValue) * (height - 1 - 17) / range);

    if (currentY >= height) currentY = height - 1;
    if (nextY >= height) nextY = height - 1;

    display.drawLine(width - (i * step) - 1, currentY, width - ((i + 1) * step) - 1, nextY, SSD1306_WHITE);
  }
}

void View::renderAllText() {
  // DEBUG_SERIAL_PRINTLN("Rendering current sensor data");
  SSD1306Display** displays = getDisplays();

  displays[0]->clearDisplay();
  displays[0]->setRotation(flipped ? 2 : 0);
  displays[0]->setTextSize(1);
  displays[0]->setTextColor(SSD1306_WHITE);
  printRight(*displays[0], String(model.getLatestTemperature() / 10.0f, 1) + "C", 16);

  displays[1]->clearDisplay();
  displays[1]->setRotation(flipped ? 2 : 0);
  displays[1]->setTextSize(1);
  displays[1]->setTextColor(SSD1306_WHITE);
  printRight(*displays[1], String(model.getLatestHumidity() / 10.0f, 1) + "%", 16);

  displays[2]->clearDisplay();
  displays[2]->setRotation(flipped ? 2 : 0);
  displays[2]->setTextSize(1);
  displays[2]->setTextColor(SSD1306_WHITE);
  printRight(*displays[2], String(model.getLatestPressure() / 10.0f, 1) + "hPa", 16);

  displays[0]->display();
  displays[1]->display();
  displays[2]->display();
}

void View::printRight(SSD1306Display& display, const String& text, int16_t y) {
  int16_t x1, y1, x2, y2;
  int16_t w, h;
  // display.getTextBounds(text, 0, y, &x1, &y1, &x2, &y2, &w, &h);
  w = text.length() * 12 * display.getWidth() / 128;
  display.setCursor(display.getWidth() - w, y);
  display.print(text);
}
