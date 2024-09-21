#include "DAC.h"

DAC::DAC(uint8_t addr, uint8_t refPin)
{
    _addr = addr;
    _refPin = refPin;
}

bool DAC::setup()
{
    Serial.print(F("DAC: "));
    Serial.print(_addr);
    Serial.print(F(" "));
    Serial.print(_refPin);
    Serial.println(F(" starting"));

    if (!_dac.begin(_addr))
    {
        Serial.print(F("DAC: "));
        Serial.print(_addr);
        Serial.print(F(" "));
        Serial.print(_refPin);
        Serial.println(F(" start failed"));
        return false;
    }

    pinMode(_refPin, INPUT);
    _refValue = analogRead(_refPin);
    _lastRefValue = _refValue;

    return true;
}

void DAC::onLoop()
{
    _refValue = analogRead(_refPin);
    _diff = _desiredValue - _refValue;

    if (_diff != 0) // out and desired is not the same
    {
        if (_diff > 0)
            _fix += _mapAnalogToDac(_diff);
        else
            _fix -= _mapAnalogToDac(_diff * -1);
        _fix = max(min(_fix, 100), -100);
    }

    if (_diff != 0)
    {
        _mappedValue = _mapAnalogToDac(_refValue) + _fix;
        _mappedValue = max(min(_mappedValue, 4095), 0);
        _dac.setVoltage(_mappedValue, false);
    }

    _lastRefValue = _refValue;
}

uint16_t DAC::getValue()
{
    return _refValue;
}

uint16_t DAC::getDesiredValue()
{
    return _desiredValue;
}

void DAC::setDesiredValue(uint16_t value, bool save)
{
    if (value < 0 || value > 1023 || _desiredValue == value)
        return;

    _dac.setVoltage(_mapAnalogToDac(value), save);
    _desiredValue = value;
    _fix = 0;
    _refValue = analogRead(_refPin);
    _lastRefValue = _refValue;
}

int16_t DAC::getFix()
{
    return _fix;
}

uint16_t DAC::_mapAnalogToDac(uint16_t value)
{
    return map(value, 0, 1023, 0, 4095);
}