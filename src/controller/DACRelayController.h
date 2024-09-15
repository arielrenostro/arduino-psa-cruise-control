#include <Arduino.h>
#include <Adafruit_MCP4725.h>

class DACRelayController
{
private:
    Adafruit_MCP4725 _dac;
    uint8_t _addr;
    uint8_t _inputPin;
    uint8_t _outputPin;
    uint16_t _inValue;
    uint16_t _lastInValue;
    uint16_t _outValue;
    uint16_t _lastOutValue;
    int16_t _diff;
    int16_t _mappedValue;
    int16_t _fix;
    bool _updateVoltage;

public:
    DACRelayController(uint8_t addr, uint8_t inputPin, uint8_t ouputPin);
    bool setup();
    void onLoop();
    void setVoltage(uint16_t value, bool save);
    int16_t getFix();
};

DACRelayController::DACRelayController(uint8_t addr, uint8_t inputPin, uint8_t ouputPin)
{
    _addr = addr;
    _inputPin = inputPin;
    _outputPin = ouputPin;
}

bool DACRelayController::setup()
{
    Serial.print(F("DACRelay: "));
    Serial.print(_addr);
    Serial.print(F(" "));
    Serial.print(_inputPin);
    Serial.print(F(" "));
    Serial.print(_outputPin);
    Serial.println(F(" starting"));

    pinMode(_inputPin, INPUT_PULLUP);
    pinMode(_outputPin, INPUT);

    if (!_dac.begin(_addr))
    {
        Serial.print(F("DACRelay: "));
        Serial.print(_addr);
        Serial.print(F(" "));
        Serial.print(_inputPin);
        Serial.print(F(" "));
        Serial.print(_outputPin);
        Serial.println(F(" start failed"));
        return false;
    }
    _inValue = analogRead(_inputPin);
    _outValue = analogRead(_outputPin);
    _lastInValue = _inValue;
    _lastOutValue = _outValue;
    return true;
}

void DACRelayController::onLoop()
{
    _updateVoltage = false;

    _inValue = analogRead(_inputPin);
    _outValue = analogRead(_outputPin);

    _diff = _inValue - _outValue;
    if (_diff != 0) // in and out not the same
    {
        _updateVoltage = true;
        if (_diff > 0)
        {
            _fix += map(_diff, 0, 1023, 0, 4095);
        }
        else
        {
            _fix -= map(_diff * -1, 0, 1023, 0, 4095);
        }
    }

    _diff = _inValue - _lastInValue;
    if (_diff != 0) // input changed
    {
        _updateVoltage = true;
        if (_diff > 0)
        {
            _fix -= map(_diff, 0, 1023, 0, 4095);
        }
        else
        {
            _fix += map(_diff * -1, 0, 1023, 0, 4095);
        }
    }

    if (_fix > 100)
    {
        _fix = 100;
    }
    else if (_fix < -100)
    {
        _fix = -100;
    }

    if (_updateVoltage)
    {
        _mappedValue = map(_inValue, 0, 1023, 0, 4095) + _fix;
        if (_mappedValue > 4095)
        {
            _mappedValue = 4095;
        }
        else if (_mappedValue < 0)
        {
            _mappedValue = 0;
        }
        _dac.setVoltage(_mappedValue, false);
    }

    if (_lastInValue != _inValue || true)
    {
        Serial.print(_inputPin);
        Serial.print(" | V: ");
        Serial.print(_inValue);
        Serial.print(" | VD: ");
        Serial.print(_outValue);
        Serial.print(" | Diff: ");
        Serial.print(_diff);
        Serial.print(" | Fix: ");
        Serial.print(_fix);
        Serial.print(" | ");
    }
    _lastInValue = _inValue;
    _lastOutValue = _outValue;
}

void DACRelayController::setVoltage(uint16_t value, bool save)
{
    _dac.setVoltage(value, save);
}

int16_t DACRelayController::getFix()
{
    return _fix;
}