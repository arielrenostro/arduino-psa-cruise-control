#include <Arduino.h>
#include <Wire.h>
#include <AltSoftSerial.h>

#include "components/Display.h"
#include "components/DAC.h"
#include "controller/AppController.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDR 0x3C // OLED display addr

// Throttle pedal
#define THROTTLE_PEDAL_DAC_1 0x61
#define THROTTLE_PEDAL_INPUT_1 A0
#define THROTTLE_PEDAL_OUTPUT_1 A3

#define THROTTLE_PEDAL_DAC_2 0x60
#define THROTTLE_PEDAL_INPUT_2 A1
#define THROTTLE_PEDAL_OUTPUT_2 A2

#define THROTTLE_PEDAL_T1_MIN 87  // analogic 0-1023
#define THROTTLE_PEDAL_T1_MAX 810 // analogic 0-1023
#define THROTTLE_PEDAL_T2_MIN 43  // analogic 0-1023
#define THROTTLE_PEDAL_T2_MAX 405 // analogic 0-1023

// Components
Display display(SCREEN_ADDR, SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Controllers
DAC dac1(THROTTLE_PEDAL_DAC_1, THROTTLE_PEDAL_OUTPUT_1);
DAC dac2(THROTTLE_PEDAL_DAC_2, THROTTLE_PEDAL_OUTPUT_2);

// Utils
unsigned long lastTime;

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(1);

  if (display.setup())
  {
    display.setCursor(0, 16);
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.println(F("80km"));
    display.display();
  }
  if (dac1.setup())
  {
    dac1.setDesiredValue(0, true);
  }
  if (dac2.setup())
  {
    dac2.setDesiredValue(0, true);
  }
}

void loop()
{
  AppController::onLoop();

  

  dac1.onLoop();
  dac2.onLoop();

  if (AppController::isDebug())
  {
    Serial.print(F("DAC1 DV:"));
    Serial.print(dac1.getDesiredValue());
    Serial.print(F(" V:"));
    Serial.print(dac1.getValue());
    Serial.print(F(" F:"));
    Serial.print(dac1.getFix());
    Serial.print(F(" | "));
    Serial.print(F("DAC2 DV:"));
    Serial.print(dac2.getDesiredValue());
    Serial.print(F(" V:"));
    Serial.print(dac2.getValue());
    Serial.print(F(" F:"));
    Serial.print(dac2.getFix());
    Serial.print(F(" | "));
    Serial.print(F("T:"));
    Serial.print(millis() - lastTime, 10);
    Serial.print(F("\n"));

    lastTime = millis();
  }

  delay(1000);
}
