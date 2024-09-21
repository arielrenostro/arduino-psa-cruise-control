#include "Button.h"

Button::Button(uint8_t pin)
{
    _pin = pin;
}

bool Button::setup()
{
    Serial.print(F("Button: "));
    Serial.print(_pin);
    Serial.println(F(" setup"));
    pinMode(_pin, INPUT_PULLUP);
    return true;
}

ButtonPressEvent Button::onLoop()
{
    if (_pushed) // already pushed on last loop
    {
        if (_isPressed()) // still pressed
        {
            _calcAux = millis() - _pushTime;

            if (_holding)
            {
                if (_calcAux >= BUTTON_HOLD_REPEAT_DELAY)
                {
                    _pushTime = millis();
                    return holding;
                }
            }
            else
            {
                if (_calcAux >= BUTTON_HOLD_DELAY)
                {
                    _pushTime = millis();
                    _holding = true;
                    return holding;
                }
            }
            return nothing;
        }
        else // button released
        {
            if (!_holding)
            {
                _pushTime = millis();
                _pushed = false;
                return push;
            }
            _holding = false;
            _pushed = false;
        }
    }
    else // button not pressed on last loop
    {
        if (millis() - _pushTime < 200) // prevent ghost click
        {
            return nothing;
        }

        if (_isPressed())
        {
            _pushed = true;
            _pushTime = millis();
        }
    }

    return nothing;
}

bool Button::_isPressed()
{
    return digitalRead(_pin) == LOW;
}