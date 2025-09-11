/*
  AppView Display Library

  A library for rendering sensor data and charts
  on OLED displays.
*/

#ifndef __APP_VIEW_H__
#define __APP_VIEW_H__

#include <Arduino.h>

class Screen;
class History;

class AppView {
public:
  AppView(Screen& screen, size_t width, size_t height, uint8_t plotHorizontalSpacing);

public:
  bool begin();
  void render(int patternIndex, History& temperatureHistory, History& humidityHistory, History& pressureHistory);

private:
  void renderCurrentSensorData(History& temperatureHistory, History& humidityHistory, History& pressureHistory);
  void renderTemperatureChart(History& temperatureHistory);
  void renderHumidityChart(History& humidityHistory);
  void renderPressureChart(History& pressureHistory);
  void drawChart(History& history);
  void drawSensorValue(float value, const char* unit);

private:
  Screen& screen;
  size_t width;
  size_t height;
  uint8_t plotHorizontalStep;
};

#endif // __APP_VIEW_H__
