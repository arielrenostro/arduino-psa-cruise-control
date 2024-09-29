#include "BuzzerController.h"

// Implements a queue with a expiration timeout for each element.
// It must be a priority queue because every alert will have a priority.
// Only the highest priority alert must fire and it must keep fireing until it expires, then the next alarm (if exists) must fires.

BuzzerController::BuzzerController()
{
    // TODO
}

void BuzzerController::onLoop()
{
    // TODO
}

void BuzzerController::playOverspeed()
{
    // TODO
}

void BuzzerController::playSpeedLimitEnabled()
{
    // TODO
}

void BuzzerController::playSpeedLimitDisabled()
{
    // TODO
}

void BuzzerController::playDisabledBySlowKLine()
{
    // TODO
}
