#ifndef DAC_C
#define DAC_C

#include <Arduino.h>
#include <Adafruit_MCP4725.h>

class DAC
{
private:
    Adafruit_MCP4725 _dac;
    uint8_t _addr;
    uint8_t _refPin;
    uint16_t _desiredValue;
    uint16_t _refValue;
    uint16_t _lastRefValue;
    
    // loop variables
    int16_t _diff;
    int16_t _fix;
    uint16_t _mappedValue;

    uint16_t _mapAnalogToDac(uint16_t value);

public:
    DAC(uint8_t addr, uint8_t refPin);
    bool setup();
    void onLoop();
    int16_t getFix();
    uint16_t getValue();
    uint16_t getDesiredValue();
    void setDesiredValue(uint16_t value, bool save);
};

#endif
