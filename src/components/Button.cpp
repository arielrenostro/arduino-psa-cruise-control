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

void Button::onLoop(ButtonPressEvent *evt)
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
                    evt->buttonIdx = 0;
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
                    evt->buttonIdx = 0;
                    evt->type = holding;
                    return;
                }
            }
            evt->buttonIdx = 0;
            evt->type = nothing;
            return;
        }
        else // button released
        {
            if (!_holding)
            {
                _pushTime = millis();
                _pushed = false;
                evt->buttonIdx = 0;
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
            evt->buttonIdx = 0;
            evt->type = nothing;
            return;
        }

        if (_isPressed())
        {
            _pushed = true;
            _pushTime = millis();
        }
    }

    evt->buttonIdx = 0;
    evt->type = nothing;
    return;
}

bool Button::_isPressed()
{
    return digitalRead(_pin) == LOW;
}