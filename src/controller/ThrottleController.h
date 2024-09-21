#ifndef ThrottleController_C
#define ThrottleController_C

#include <Arduino.h>
#include "../components/DAC.h"

#define TRACK_1_MIN 87  // analogic 0-1023
#define TRACK_1_MAX 810 // analogic 0-1023
#define TRACK_2_MIN 43  // analogic 0-1023
#define TRACK_2_MAX 405 // analogic 0-1023

class ThrottleController
{
private:
    DAC *_track1;
    DAC *_track2;
public:
    ThrottleController(DAC *track1, DAC *track2);
    void setPosition(uint8_t position);
};

#endif