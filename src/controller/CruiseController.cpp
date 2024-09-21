#include "CruiseController.h"

CruiseController::CruiseController(ThrottleController *throttleController)
{
    _throttleController = throttleController;
}

void CruiseController::onLoop()
{
    if (!_enabled)
    {
        return;
    }

    switch (_mode)
    {
    case cruise:
        _onCruiseLoop();
        break;
    case limit:
        _onLimitLoop();
        break;
    default:
        break;
    }
}

void CruiseController::changeMode()
{
    if (_enabled)
    {
        return;
    }

    if (_mode == cruise)
    {
        _mode = limit;
    }
    else
    {
        _mode = cruise;
    }
}

void CruiseController::addSpeed(uint8_t speed)
{
    _desiredSpeed += speed;

    if (_desiredSpeed > MAX_SPEED)
    {
        _desiredSpeed = MAX_SPEED;
    }
}

void CruiseController::subSpeed(uint8_t speed)
{
    if (_desiredSpeed > speed)
    {
        _desiredSpeed = MIN_SPEED;
    }
    else
    {
        _desiredSpeed -= speed;
    }

    if (_desiredSpeed < MIN_SPEED)
    {
        _desiredSpeed = MIN_SPEED;
    }
}

void CruiseController::disable()
{
    _enabled = false;
}

void CruiseController::enable()
{
    _enabled = true;
}

void CruiseController::_onLimitLoop()
{
    
}

void CruiseController::_onCruiseLoop()
{
}