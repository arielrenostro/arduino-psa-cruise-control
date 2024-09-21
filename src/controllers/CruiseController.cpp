#include "CruiseController.h"

CruiseController::CruiseController(ThrottleController *throttleController, BuzzerController *buzzerController)
{
    _throttleController = throttleController;
    _buzzerController = buzzerController;

    double Kp = 0;  // Initial Proportional Gain
    double Ki = 10; // Initial Integral Gain
    double Kd = 0;  // Initial Differential Gain

    PID pid(&_actualSpeed, &_outputPid, &_desiredSpeed, Kp, Ki, Kd, DIRECT);
    _pid = &pid;
}

void CruiseController::onLoop()
{
    if (!_enabled)
    {
        _replayThrottle();
        return;
    }

    unsigned long lastSpeedRef = millis(); // TODO: get it from K-LINE Controller
    if (millis() - lastSpeedRef > TIME_UNTIL_DISABLE_BY_SLOW_K_LINE_RESPONSE)
    {
        _buzzerController->playDisabledBySlowKLine();
        disable();
        _replayThrottle();
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
    uint8_t speed = 0; // TODO -> Get speed from K-LINE

    // handle the overspeed buzzer
    if (speed > _desiredSpeed && speed - _desiredSpeed > OVERSPEED_TO_BUZZER)
    {
        _buzzerController->playOverspeed();
    }

    // handle temporary disabled
    if (_temporaryDisabled)
    {
        _replayThrottle();

        if (speed <= _desiredSpeed)
        {
            if (_removeTempDisabledTime == 0)
            {
                _removeTempDisabledTime = millis();
            }

            if (millis() - _removeTempDisabledTime >= TIME_UNTIL_REMOVE_TEMP_DISABLE)
            {
                _buzzerController->playSpeedLimitEnabled();
                _removeTempDisabledTime = 0;
                _temporaryDisabled = false;
            }
        }
        else
        {
            _removeTempDisabledTime = 0;
        }

        return;
    }

    if (speed > _desiredSpeed) // overspeed
    {
        uint16_t rposition = _throttleController->readPosition();
        uint16_t wposition = _throttleController->getWrotePosition();
        if (rposition > wposition && rposition - wposition > THROTTLE_DIFF_LIMIT_TO_TEMP_DISABLE) // user's asked for throttle
        {
            _buzzerController->playSpeedLimitDisabled();
            _temporaryDisabled = true;
            _replayThrottle();
        }
        else // user's not asked for throttle, just keep the speed limit
        {
            _onCruiseLoop(); // use the cruise control to keep at desired speed
        }
    }
    else // not in overspeed
    {
        _replayThrottle();
    }
}

void CruiseController::_onCruiseLoop()
{
    uint8_t speed = 0; // TODO -> Get speed from K-LINE

    // handle the overspeed buzzer
    if (speed > _desiredSpeed && speed - _desiredSpeed > OVERSPEED_TO_BUZZER)
    {
        _buzzerController->playOverspeed();
    }

    _pid->Compute();
}

void CruiseController::_replayThrottle()
{
    _throttleController->writePosition(_throttleController->readPosition());
}