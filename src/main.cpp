#include <Arduino.h>
#include <LowPower.h>
#include <Wire.h>
#include <AltSoftSerial.h>

#include "components/Display.h"
#include "components/DAC.h"
#include "components/AnalogButton.h"
#include "components/Button.h"
#include "controllers/App/AppController.h"
#include "controllers/Cruise/CruiseController.h"
#include "controllers/Throttle/ThrottleController.h"
#include "controllers/KLine/KLineController.h"

constexpr uint16_t voltageToADC(float voltage)
{
    return static_cast<uint16_t>((voltage / 5.0) * 1023);
}

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

// Analog Buttons
#define CONTROL_BUTTONS_PIN A7
#define CHANGE_MODE_BUTTON_IDX 0
#define UP_BUTTON_IDX 1
#define DOWN_BUTTON_IDX 2
#define OK_BUTTON_IDX 3
#define CANCEL_BUTTON_IDX 4
uint16_t BUTTONS_VOLTAGES[5] = {voltageToADC(0.901), voltageToADC(1.795), voltageToADC(2.5), voltageToADC(3.214), voltageToADC(4.545)};

// Buttons
#define CLUTCH_PEDAL_BUTTON_PIN 8
#define BREAK_PEDAL_BUTTON_PIN 9

// Buzzer
#define BUZZER_PIN 2

// Behaviours
#define SPEED_STEP 1
#define SPEED_HOLDING_STEP 5

// Components
Display display(SCREEN_ADDR, SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DAC dac1(THROTTLE_PEDAL_DAC_1, THROTTLE_PEDAL_OUTPUT_1);
DAC dac2(THROTTLE_PEDAL_DAC_2, THROTTLE_PEDAL_OUTPUT_2);
AnalogButton controlButtons(CONTROL_BUTTONS_PIN, &BUTTONS_VOLTAGES[0], 5);
Button clutchBt(CLUTCH_PEDAL_BUTTON_PIN);
Button breakBt(BREAK_PEDAL_BUTTON_PIN);

// Controllers
ThrottleController throttleController(&dac1, &dac2, THROTTLE_PEDAL_INPUT_1, THROTTLE_PEDAL_INPUT_2);
BuzzerController buzzerController(BUZZER_PIN);
KLineController klineController;
CruiseController cruiseController(&throttleController, &buzzerController, &klineController);

// Utils
ButtonPressEvent btEvent = {};
unsigned long lastLoopTime;
unsigned long lastDebugTime;

// functions
void buttonsLoop();
void debugLoop();
void lowPowerLoop();

void setup()
{
    Serial.begin(115200);
    Serial.setTimeout(1);

    AppController::setup(true, &cruiseController); // remove it in final compile

    if (!display.setup())
    {
        Serial.println("FAILURE TO START DISPLAY.");
        // while (true)
        //   delay(1);
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
    klineController.setup();
    controlButtons.setup();
    clutchBt.setup();
    breakBt.setup();

    dac1.setDesiredValue(0, true);
    dac2.setDesiredValue(0, true);
}

void loop()
{
    AppController::onLoop();
    buttonsLoop();
    klineController.onLoop();
    cruiseController.onLoop();
    dac1.onLoop();
    dac2.onLoop();
    buzzerController.onLoop();
    debugLoop();
    lowPowerLoop();
    lastLoopTime = millis();
}

void buttonsLoop()
{
    controlButtons.onLoop(&btEvent);

    if (btEvent.type != nothing)
    {
        switch (btEvent.buttonIdx)
        {
        case CHANGE_MODE_BUTTON_IDX:
            if (btEvent.type == holding || btEvent.type == push)
            {
                cruiseController.changeMode();
            }
            break;

        case UP_BUTTON_IDX:
            if (btEvent.type == holding)
            {
                cruiseController.addSpeed(SPEED_HOLDING_STEP);
            }
            else if (btEvent.type == push)
            {
                cruiseController.addSpeed(SPEED_STEP);
            }
            break;

        case DOWN_BUTTON_IDX:
            if (btEvent.type == holding)
            {
                cruiseController.subSpeed(SPEED_HOLDING_STEP);
            }
            else if (btEvent.type == push)
            {
                cruiseController.subSpeed(SPEED_STEP);
            }
            break;

        case OK_BUTTON_IDX:
            if (btEvent.type == push)
            {
                cruiseController.enable();
            }
            break;

        case CANCEL_BUTTON_IDX:
            if (btEvent.type == push)
            {
                cruiseController.disable();
                return;
            }
            break;

        default:
            break;
        }
    }

    clutchBt.onLoop(&btEvent);
    if (btEvent.type == push || btEvent.type == holding)
    {
        cruiseController.disable();
        klineController.clearRpm();
        return;
    }

    breakBt.onLoop(&btEvent);
    if (btEvent.type == push || btEvent.type == holding)
    {
        cruiseController.disable();
        return;
    }
}

void debugLoop()
{
    if (AppController::isDebug() && millis() - lastDebugTime > 350)
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
        Serial.print(F("KL C:"));
        Serial.print(klineController.isConnected());
        Serial.print(F(" S:"));
        Serial.print(klineController.getSpeed().value);
        Serial.print(F(" T:"));
        Serial.print((unsigned long)klineController.getSpeed().time);
        Serial.print(F(" R:"));
        Serial.print(klineController.getRpm().value);
        Serial.print(F(" T:"));
        Serial.print((unsigned long)klineController.getRpm().time);

        Serial.print(F(" | "));
        Serial.print(F("B T:"));
        Serial.print(buzzerController.getFiringAlert());

        Serial.print(F(" | "));
        Serial.print(F("T V:"));
        Serial.print(millis() - lastLoopTime, 10);

        Serial.print(F("\n"));

        lastDebugTime = millis();
    }
}

void lowPowerLoop()
{
    if (klineController.getCountConnectionTimeout() >= 10)
    {
        LowPower.powerDown(SLEEP_1S, ADC_ON, BOD_ON);
    }
}
