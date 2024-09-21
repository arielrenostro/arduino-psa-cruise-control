#ifndef CruiseController_C
#define CruiseController_C

#include <Arduino.h>
#include "ThrottleController.h"

#define MIN_SPEED 40
#define MAX_SPEED 120

enum CruiseMode
{
    limit,
    cruise,
};

class CruiseController
{
private:
    ThrottleController *_throttleController;
    CruiseMode _mode = cruise;
    uint8_t _desiredSpeed = 40;
    bool _enabled = false;

    void _onCruiseLoop();
    void _onLimitLoop();
public:
    CruiseController(ThrottleController *throttleController);
    void onLoop();
    void changeMode();
    void addSpeed(uint8_t speed);
    void subSpeed(uint8_t speed);
    void disable();
    void enable();
};

#endif