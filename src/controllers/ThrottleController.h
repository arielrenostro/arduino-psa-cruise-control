#ifndef ThrottleController_C
#define ThrottleController_C

#include <Arduino.h>
#include "../components/DAC.h"

#define TRACK_1_MIN 87  // analogic 0-1023
#define TRACK_1_MAX 810 // analogic 0-1023
#define TRACK_2_MIN 43  // analogic 0-1023
#define TRACK_2_MAX 405 // analogic 0-1023

#define POSITION_MAX 1000
#define POSITION_MIN 0

class ThrottleController
{
private:
    DAC *_trackOut1;
    DAC *_trackOut2;

    uint16_t _wrotePosition;
    uint8_t _trackInPin1;
    uint8_t _trackInPin2;

public:
    ThrottleController(DAC *trackOut1, DAC *trackOut2, uint8_t trackInPin1, uint8_t trackInPin2);
    void setup();
    uint16_t readPosition();
    uint16_t getWrotePosition();
    void writePosition(uint16_t position);
};

#endif