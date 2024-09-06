#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_MCP2515.h>

#include "controller/DACRelayController.h"

#define GAS_PEDAL_DAC_1 0x60
#define GAS_PEDAL_INPUT_1 0
#define GAS_PEDAL_OUTPUT_1 2

#define GAS_PEDAL_DAC_2 0x61
#define GAS_PEDAL_INPUT_2 1
#define GAS_PEDAL_OUTPUT_2 3

#define CAN_BAUDRATE (250000)
#define CAN_CS_PIN 10

#define THROTTLE_PEDAL_T1_MIN 87  //analogic 0-1023
#define THROTTLE_PEDAL_T1_MAX 810 //analogic 0-1023
#define THROTTLE_PEDAL_T2_MIN 43  //analogic 0-1023
#define THROTTLE_PEDAL_T2_MAX 405 //analogic 0-1023

DACRelayController dacRelayController1(GAS_PEDAL_DAC_1, GAS_PEDAL_INPUT_1, GAS_PEDAL_OUTPUT_1);
DACRelayController dacRelayController2(GAS_PEDAL_DAC_2, GAS_PEDAL_INPUT_2, GAS_PEDAL_OUTPUT_2);
Adafruit_MCP2515 mcp(CAN_CS_PIN);

// functions
void readCan();

void setup()
{
  // Serial.begin(115200);
  Serial.begin(230400);
  if (dacRelayController1.setup()) {
    dacRelayController1.setVoltage(0, true);
  }
  if (dacRelayController2.setup()) {
    dacRelayController2.setVoltage(0, true);
  }

  if (!mcp.begin(CAN_BAUDRATE))
  {
    Serial.println("Error initializing MCP2515.");
    while (1)
      delay(10);
  }
  Serial.println("MCP2515 chip found");
}

unsigned long x = 0;

void loop()
{
  dacRelayController1.onLoop();
  dacRelayController2.onLoop();
  Serial.print("\n");
  delay(50);

  readCan();
}

void readCan()
{
  // try to parse packet
  int packetSize = mcp.parsePacket();

  if (packetSize)
  {
    // received a packet
    Serial.print("Received ");

    if (mcp.packetExtended())
    {
      Serial.print("extended ");
    }

    if (mcp.packetRtr())
    {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    }

    Serial.print("packet with id 0x");
    Serial.print(mcp.packetId(), HEX);

    if (mcp.packetRtr())
    {
      Serial.print(" and requested length ");
      Serial.println(mcp.packetDlc());
    }
    else
    {
      Serial.print(" and length ");
      Serial.println(packetSize);

      // only print packet data for non-RTR packets
      while (mcp.available())
      {
        Serial.print((char)mcp.read());
      }
      Serial.println();
    }

    Serial.println();
  }
}
