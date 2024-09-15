#include <Arduino.h>
#include <Wire.h>

#include "components/Display.h"
#include "controller/DACRelayController.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDR 0x3C // OLED display addr

// Throttle pedal
#define THROTTLE_PEDAL_DAC_1 0x60
#define THROTTLE_PEDAL_INPUT_1 A0
#define THROTTLE_PEDAL_OUTPUT_1 A2

#define THROTTLE_PEDAL_DAC_2 0x61
#define THROTTLE_PEDAL_INPUT_2 A1
#define THROTTLE_PEDAL_OUTPUT_2 A3

#define THROTTLE_PEDAL_T1_MIN 87  // analogic 0-1023
#define THROTTLE_PEDAL_T1_MAX 810 // analogic 0-1023
#define THROTTLE_PEDAL_T2_MIN 43  // analogic 0-1023
#define THROTTLE_PEDAL_T2_MAX 405 // analogic 0-1023

// KWP

// Components
// Adafruit_MCP2515 mcp(CAN_CS_PIN);
Display display(SCREEN_ADDR, SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Controllers
DACRelayController dacRelayController1(THROTTLE_PEDAL_DAC_1, THROTTLE_PEDAL_INPUT_1, THROTTLE_PEDAL_OUTPUT_1);
DACRelayController dacRelayController2(THROTTLE_PEDAL_DAC_2, THROTTLE_PEDAL_INPUT_2, THROTTLE_PEDAL_OUTPUT_2);

// functions

void setup()
{
  Serial.begin(230400);
  if (display.setup())
  {
    display.setCursor(0, 16);
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.println(F("80km"));
    display.display();
  }
  if (dacRelayController1.setup())
  {
    dacRelayController1.setVoltage(0, true);
  }
  if (dacRelayController2.setup())
  {
    dacRelayController2.setVoltage(0, true);
  }
}

unsigned long x = 0;
uint8_t v = 40;

int i1;
int i2;
int o1;
int o2;

double vi1;
double vi2;
double vo1;
double vo2;

void loop()
{
  i1 = analogRead(THROTTLE_PEDAL_INPUT_1);
  i2 = analogRead(THROTTLE_PEDAL_INPUT_2);
  o1 = analogRead(THROTTLE_PEDAL_OUTPUT_1);
  o2 = analogRead(THROTTLE_PEDAL_OUTPUT_2);

  vi1 = (5.0 / 1024.0) * i1;
  vi2 = (5.0 / 1024.0) * i2;
  vo1 = (5.0 / 1024.0) * o1;
  vo2 = (5.0 / 1024.0) * o2;

  dacRelayController1.onLoop();
  dacRelayController2.onLoop();
  Serial.print("\n");

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.print('1');
  display.print(':');
  display.print(' ');
  display.print(' ');
  display.print(' ');
  display.print(' ');
  display.print('2');
  display.print(':');
  display.print('\n');
  display.print(vi1, 2);
  display.print(' ');
  display.print(' ');
  display.print(vi2, 2);
  display.print('\n');
  display.print(vo1, 2);
  display.print(' ');
  display.print(' ');
  display.print(vo2, 2);
  display.print('\n');

  i1 = dacRelayController1.getFix();
  i2 = dacRelayController2.getFix();
  if (abs(i1) < 100)
  {
    display.print(' ');
  }
  if (abs(i1) < 10)
  {
    display.print(' ');
  }
  if (i1 >= 0)
  {
    display.print(' ');
  }
  display.print((long)i1, 10);

  display.print(' ');
  display.print(' ');

  if (abs(i2) < 100)
  {
    display.print(' ');
  }
  if (abs(i2) < 10)
  {
    display.print(' ');
  }
  if (i2 >= 0)
  {
    display.print(' ');
  }
  display.print((long)i2, 10);

  display.display();

  // if (millis() - x > 100 || true)
  // {
  //   display.clearDisplay();
  //   display.setCursor(16, 22);
  //   display.setTextSize(3);
  //   display.setTextColor(WHITE);
  //   if (v < 100)
  //   {
  //     display.print(' ');
  //   }
  //   if (v < 10)
  //   {
  //     display.print(' ');
  //   }
  //   display.print(v, 10);
  //   display.setCursor(76, 22);
  //   display.println(F("km"));
  //   display.display();

  //   if (v >= 120)
  //   {
  //     v = 40;
  //   }
  //   v++;

  //   x = millis();
  // }

  delay(1);
}
