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

Display::Display(uint8_t addr, uint8_t width, uint8_t height, TwoWire *wire, int oledReset)
{
  _addr = addr;
  _width = width;
  _height = height;
  _display = Adafruit_SSD1306(width, height, wire, oledReset);
}

bool Display::setup()
{
  Serial.println(F("Display: Starting"));
  if (_display.begin(SSD1306_SWITCHCAPVCC, _addr))
  {
    Serial.println(F("Display: Started"));
    clearDisplay();
    return true;
  }
  Serial.println(F("Display: Failed"));
  return false;
}

uint8_t Display::getWidth()
{
  return _width;
}

uint8_t Display::getHeight()
{
  return _height;
}

void Display::clearDisplay()
{
  _display.clearDisplay();
}

void Display::setTextSize(uint8_t i)
{
  _display.setTextSize(i);
}

void Display::setTextColor(int c)
{
  _display.setTextColor(c);
}

void Display::setCursor(int16_t x, int16_t y)
{
  _display.setCursor(x, y);
}

void Display::print(long v, int base) {
  _display.print(v, base);
}

void Display::print(double n, int digits) {
  _display.print(n, digits);
}

void Display::print(char v)
{
  _display.print(v);
}

void Display::println(const __FlashStringHelper *ifsh)
{
  _display.println(ifsh);
}

void Display::display()
{
  _display.display();
}

void Display::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  _display.drawRect(x, y, w, h, color);
}

void Display::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  _display.fillRect(x, y, w, h, color);
}

void Display::drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg)
{
  return _display.drawBitmap(x, y, bitmap, w, h, color, bg);
}

#endif