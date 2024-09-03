#include <Arduino.h>
#include <Wire.h>
#include "controller/DACRelayController.h"

DACRelayController dacRelayController(0x60, 1, 2);

void setup()
{
  Serial.begin(115200);
  dacRelayController.setup();
}

void loop()
{
  dacRelayController.onLoop();
  delay(10);
}
