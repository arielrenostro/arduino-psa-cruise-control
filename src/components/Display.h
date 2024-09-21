#ifndef Display_C
#define Display_C

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Display
{
private:
  Adafruit_SSD1306 _display;
  uint8_t _addr;
  uint8_t _width;
  uint8_t _height;

public:
  Display(uint8_t addr, uint8_t width, uint8_t height, TwoWire *wire, int oledReset);

  bool setup();
  uint8_t getWidth();
  uint8_t getHeight();
  void clearDisplay();
  void setTextSize(uint8_t i);
  void setTextColor(int c);
  void setCursor(int16_t x, int16_t y);
  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);
  void print(char v);
  void print(long v, int base);
  void print(double n, int digits);
  void println(const __FlashStringHelper *ifsh);
  void display();
};

#endif