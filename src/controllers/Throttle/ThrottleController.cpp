#include "ThrottleController.h"

ThrottleController::ThrottleController(DAC *trackOut1, DAC *trackOut2, uint8_t trackInPin1, uint8_t trackInPin2)
{
    _trackOut1 = trackOut1;
    _trackOut2 = trackOut2;
    _trackInPin1 = trackInPin1;
    _trackInPin2 = trackInPin2;
}

void ThrottleController::setup()
{
    pinMode(_trackInPin1, INPUT);
    pinMode(_trackInPin2, INPUT);
}

uint16_t ThrottleController::readPosition()
{
    uint16_t v1 = analogRead(_trackInPin1);
    v1 = max(min(v1, TRACK_1_MAX), TRACK_1_MIN);

    uint16_t v2 = analogRead(_trackInPin2);
    v2 = max(min(v1, TRACK_2_MAX), TRACK_2_MIN);

    v1 = map(v1, TRACK_1_MIN, TRACK_1_MAX, POSITION_MIN, POSITION_MAX);
    v1 += map(v2, TRACK_2_MIN, TRACK_2_MAX, POSITION_MIN, POSITION_MAX);

    return v1 / 2;
}

uint16_t ThrottleController::getWrotePosition()
{
    return _wrotePosition;
}

void ThrottleController::writePosition(uint16_t position)
{
    if (position > POSITION_MAX)
    {
        return;
    }

    _wrotePosition = position;
    _trackOut1->setDesiredValue(map(position, POSITION_MIN, POSITION_MAX, TRACK_1_MIN, TRACK_1_MAX), false);
    _trackOut2->setDesiredValue(map(position, POSITION_MIN, POSITION_MAX, TRACK_2_MIN, TRACK_2_MAX), false);
}
