#include "ThrottleController.h"

ThrottleController::ThrottleController(DAC *track1, DAC *track2)
{
    _track1 = track1;
    _track2 = track2;
}

void ThrottleController::setPosition(uint8_t position)
{
    if (position > 100)
    {
        return;
    }
    _track1->setDesiredValue(map(position, 0, 100, TRACK_1_MIN, TRACK_1_MAX), false);
    _track2->setDesiredValue(map(position, 0, 100, TRACK_2_MIN, TRACK_2_MAX), false);
}
