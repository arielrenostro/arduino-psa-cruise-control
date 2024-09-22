#include <Arduino.h>
#include <Wire.h>
#include <AltSoftSerial.h>

#include "components/Display.h"
#include "components/DAC.h"
#include "components/Button.h"
#include "controllers/AppController.h"
#include "controllers/CruiseController.h"
#include "controllers/ThrottleController.h"

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

// Buttons
#define CHANGE_MODE_BUTTON_PIN 3
#define UP_BUTTON_PIN 4
#define DOWN_BUTTON_PIN 5
#define OK_BUTTON_PIN 6
#define CANCEL_BUTTON_PIN 7
#define CLUTCH_PEDAL_BUTTON_PIN 8
#define BREAK_PEDAL_BUTTON_PIN 9

// Behaviours
#define SPEED_STEP 1
#define SPEED_HOLDING_STEP 5

// Components
Display display(SCREEN_ADDR, SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DAC dac1(THROTTLE_PEDAL_DAC_1, THROTTLE_PEDAL_OUTPUT_1);
DAC dac2(THROTTLE_PEDAL_DAC_2, THROTTLE_PEDAL_OUTPUT_2);
Button changeModeBt(CHANGE_MODE_BUTTON_PIN);
Button upBt(UP_BUTTON_PIN);
Button downBt(DOWN_BUTTON_PIN);
Button okBt(OK_BUTTON_PIN);
Button cancelBt(CANCEL_BUTTON_PIN);
Button clutchBt(CLUTCH_PEDAL_BUTTON_PIN);
Button breakBt(BREAK_PEDAL_BUTTON_PIN);

// Controllers
ThrottleController throttleController(&dac1, &dac2, THROTTLE_PEDAL_INPUT_1, THROTTLE_PEDAL_INPUT_2);
BuzzerController buzzerController;
CruiseController cruiseController(&throttleController, &buzzerController);

// Utils
ButtonPressEvent btEvent = nothing;
unsigned long lastTime;

// functions
void buttonsLoop();
void debugLoop();

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(1);

  AppController::setDebug(true); // remove it in final compile

  if (!display.setup())
  {
    Serial.println("FAILURE TO START DISPLAY.");
    while (true)
      delay(1);
  }

  if (!dac1.setup() || !dac2.setup())
  {
    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println(F("! FAILED !"));

    display.setTextSize(3);
    display.setCursor(26, 26);
    display.println(F("DAC"));

    display.display();

    while (true)
      delay(1);
  }

  throttleController.setup();
  changeModeBt.setup();
  upBt.setup();
  downBt.setup();
  okBt.setup();
  cancelBt.setup();
  clutchBt.setup();
  breakBt.setup();

  dac1.setDesiredValue(0, true);
  dac2.setDesiredValue(0, true);
}

void loop()
{
  AppController::onLoop();
  buttonsLoop();
  cruiseController.onLoop();
  dac1.onLoop();
  dac2.onLoop();
  debugLoop();

  delay(350);
}

void buttonsLoop()
{
  btEvent = changeModeBt.onLoop();
  if (btEvent == holding || btEvent == push)
  {
    cruiseController.changeMode();
  }

  btEvent = upBt.onLoop();
  if (btEvent == holding)
  {
    cruiseController.addSpeed(SPEED_HOLDING_STEP);
  }
  else if (btEvent == push)
  {
    cruiseController.addSpeed(SPEED_STEP);
  }

  btEvent = downBt.onLoop();
  if (btEvent == holding)
  {
    cruiseController.subSpeed(SPEED_HOLDING_STEP);
  }
  else if (btEvent == push)
  {
    cruiseController.subSpeed(SPEED_STEP);
  }

  btEvent = cancelBt.onLoop();
  if (btEvent == push)
  {
    cruiseController.disable();
    return;
  }

  btEvent = clutchBt.onLoop();
  if (btEvent == push || btEvent == holding)
  {
    cruiseController.disable();
    return;
  }

  btEvent = breakBt.onLoop();
  if (btEvent == push || btEvent == holding)
  {
    cruiseController.disable();
    return;
  }

  btEvent = okBt.onLoop();
  if (btEvent == push)
  {
    cruiseController.enable();
  }
}

void debugLoop()
{
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
    Serial.print(F("TC RP:"));
    Serial.print(throttleController.readPosition());
    Serial.print(F(" WP:"));
    Serial.print(throttleController.getWrotePosition());

    Serial.print(F(" | "));
    Serial.print(F("T V:"));
    Serial.print(millis() - lastTime, 10);

    Serial.print(F("\n"));

    lastTime = millis();
  }
}
