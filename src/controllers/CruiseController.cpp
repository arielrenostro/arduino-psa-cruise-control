#include "CruiseController.h"

CruiseController::CruiseController(ThrottleController *throttleController, BuzzerController *buzzerController)
{
    _throttleController = throttleController;
    _buzzerController = buzzerController;

    double Kp = 0;  // Initial Proportional Gain
    double Ki = 10; // Initial Integral Gain
    double Kd = 0;  // Initial Differential Gain

    PID pid(&_pidInput, &_pidOutput, &_pidSetPoint, Kp, Ki, Kd, DIRECT);
    _pid = &pid;
    _pid->SetOutputLimits(POSITION_MIN, POSITION_MAX);
}

void CruiseController::onLoop()
{
    if (!_enabled)
    {
        _replayThrottle();
        return;
    }

    _readSpeed();
    unsigned long lastSpeedRef = millis(); // TODO: get it from K-LINE controller
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

    // handle the overspeed buzzer
    if (_actualSpeed > _desiredSpeed && _actualSpeed - _desiredSpeed > OVERSPEED_TO_BUZZER)
    {
        _buzzerController->playOverspeed();
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

    _pidSetPoint = _desiredSpeed;
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

    _pidSetPoint = _desiredSpeed;
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
    // handle temporary disabled
    if (_temporaryDisabled)
    {
        _replayThrottle();

        if (_actualSpeed <= _desiredSpeed)
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

    if (_actualSpeed >= _desiredSpeed) // overspeed
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
    _pid->Compute(); // TODO: implement a delay because of the slow vehicle response.
    _throttleController->writePosition(_pidOutput);
}

void CruiseController::_replayThrottle()
{
    _throttleController->writePosition(_throttleController->readPosition());
}

void CruiseController::_readSpeed()
{
    _actualSpeed = 0; // TODO: Get it from KLINE
    _pidInput = _actualSpeed;
}