#include "AnalogButton.h"
#include "Button.h"

AnalogButton::AnalogButton(uint8_t pin, uint16_t *voltages, uint8_t length)
{
    _pin = pin;
    _voltages = voltages;
    _voltagesLength = length;
}

bool AnalogButton::setup()
{
    Serial.print(F("AnalogButton: "));
    Serial.print(_pin);
    Serial.println(F(" setup"));
    pinMode(_pin, INPUT);
    return true;
}

void AnalogButton::onLoop(ButtonPressEvent *evt)
{
    if (_pushed) // already pushed on last loop
    {
        uint16_t currentButton = _readCurrentButton();
        if (currentButton != BUTTON_NOT_PRESSED && currentButton != _currentButton) // button changed, reset it
        {
            _currentButton = BUTTON_NOT_PRESSED;
            _pushTime = millis();
            _holding = false;
            _pushed = false;
            evt->buttonIdx = _currentButton;
            evt->type = nothing;
            return;
        }

        if (currentButton != BUTTON_NOT_PRESSED) // still pressed
        {
            _calcAux = millis() - _pushTime;

            if (_holding)
            {
                if (_calcAux >= BUTTON_HOLD_REPEAT_DELAY)
                {
                    _pushTime = millis();
                    evt->buttonIdx = _currentButton;
                    evt->type = holding;
                    return;
                }
            }
            else
            {
                if (_calcAux >= BUTTON_HOLD_DELAY)
                {
                    _pushTime = millis();
                    _holding = true;
                    evt->buttonIdx = _currentButton;
                    evt->type = holding;
                    return;
                }
            }
            evt->buttonIdx = BUTTON_NOT_PRESSED;
            evt->type = nothing;
            return;
        }
        else // button released
        {
            if (!_holding)
            {
                uint8_t buttonReleased = _currentButton;
                _currentButton = BUTTON_NOT_PRESSED;
                _pushTime = millis();
                _pushed = false;
                evt->buttonIdx = buttonReleased;
                evt->type = push;
                return;
            }
            _holding = false;
            _pushed = false;
        }
    }
    else // button not pressed on last loop
    {
        if (millis() - _pushTime < 200) // prevent ghost click
        {
            evt->buttonIdx = BUTTON_NOT_PRESSED;
            evt->type = nothing;
            return;
        }

        if (_isPressed())
        {
            _pushed = true;
            _pushTime = millis();
            _currentButton = _readCurrentButton();
        }
    }

    evt->buttonIdx = BUTTON_NOT_PRESSED;
    evt->type = nothing;
}

bool AnalogButton::_isPressed()
{
    return analogRead(_pin) < BUTTON_ANALOG_UNPRESSED;
}

uint8_t AnalogButton::_readCurrentButton()
{
    uint16_t value = analogRead(_pin);
    if (value < BUTTON_ANALOG_UNPRESSED)
    {
        for (uint8_t i = 0; i < _voltagesLength; i++)
        {
            if (_voltages[i] * 0.9 <= value && _voltages[i] * 1.1 >= value)
            {
                return i;
            }
        }
    }

    return BUTTON_NOT_PRESSED;
}