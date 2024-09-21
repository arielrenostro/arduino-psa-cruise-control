#ifndef CruiseController_C
#define CruiseController_C

#include <Arduino.h>
#include <PID_v1.h>
#include "ThrottleController.h"
#include "BuzzerController.h"

#define MIN_SPEED 40
#define MAX_SPEED 120
#define OVERSPEED_TO_BUZZER 5
#define THROTTLE_DIFF_LIMIT_TO_TEMP_DISABLE 100
#define TIME_UNTIL_REMOVE_TEMP_DISABLE 5000
#define TIME_UNTIL_DISABLE_BY_SLOW_K_LINE_RESPONSE 2000

enum CruiseMode
{
    limit,
    cruise,
};

class CruiseController
{
private:
    ThrottleController *_throttleController;
    BuzzerController *_buzzerController;

    CruiseMode _mode = cruise;
    bool _enabled = false;

    bool _temporaryDisabled = false;
    unsigned long _removeTempDisabledTime = 0;

    uint8_t _actualSpeed = 0;
    uint8_t _desiredSpeed = 0;

    PID *_pid;
    double _pidInput = 0;
    double _pidSetPoint = 40;
    double _pidOutput = 0;

    void _onCruiseLoop();
    void _onLimitLoop();
    void _replayThrottle();
    void _readSpeed();

public:
    CruiseController(ThrottleController *throttleController, BuzzerController *buzzerController);
    void onLoop();
    void changeMode();
    void addSpeed(uint8_t speed);
    void subSpeed(uint8_t speed);
    void disable();
    void enable();
};

#endif