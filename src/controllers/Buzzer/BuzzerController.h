#ifndef BuzzerController_C
#define BuzzerController_C

#include <Arduino.h>
#include "pitches.h"

enum AlertType
{
    overspeed,          // 0
    speedLimitEnabled,  // 1
    speedLimitDisabled, // 2
    klineSlow,          // 3
    silence,            // 4
};

struct AlertItem
{
    AlertType alertType;
    uint64_t validUntil;
};

class BuzzerController
{
private:
    AlertItem _queue[4] = {AlertItem{overspeed}, AlertItem{speedLimitEnabled}, AlertItem{speedLimitDisabled}, AlertItem{klineSlow}};
    uint8_t _pin;

    AlertType _firing;
    uint64_t _firesUntil;
    uint64_t _nextState;
    bool _state;

    void _handleFiring();

public:
    BuzzerController(uint8_t pin);
    void onLoop();
    void fire(AlertType type);
    AlertType getFiringAlert();
};

#endif
