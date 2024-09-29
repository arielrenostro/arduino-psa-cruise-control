#include "BuzzerController.h"

BuzzerController::BuzzerController(uint8_t pin)
{
    _pin = pin;
    _firing = silence;
}

void BuzzerController::onLoop()
{
    if (millis() > _firesUntil && _firing != silence)
    {
        _firing = silence;
        noTone(_pin);
    }

    if (_firing == silence)
    {
        for (uint8_t i = 0; i < sizeof(_queue); i++)
        {
            if (_queue[i].validUntil > millis())
            {
                _firing = (AlertType)i;
                _firesUntil = millis() + 1000;
                _nextState = millis();
                _state = false;
                break;
            }
        }
    }

    _handleFiring();
}

void BuzzerController::fire(AlertType type)
{
    _queue[type].validUntil = millis() + 1000;
}

AlertType BuzzerController::getFiringAlert()
{
    return _firing;
}

void BuzzerController::_handleFiring()
{
    if (_nextState > millis())
    {
        return;
    }

    switch (_firing)
    {
    case overspeed:
        if (_state)
            tone(_pin, NOTE_C3);
        else
            noTone(_pin);
        _nextState = millis() + 100;
        _state = !_state;
        break;

    case speedLimitEnabled:
        if (_state)
            tone(_pin, NOTE_F3);
        else
            noTone(_pin);
        _nextState = millis() + 100;
        _state = !_state;
        break;

    case speedLimitDisabled:
        if (_state)
            tone(_pin, NOTE_G3);
        else
            noTone(_pin);
        _nextState = millis() + 100;
        _state = !_state;
        break;

    case klineSlow:
        if (_state)
            tone(_pin, NOTE_A3);
        else
            noTone(_pin);
        _nextState = millis() + 100;
        _state = !_state;
        break;

    case silence:
    default:
        break;
    }
}