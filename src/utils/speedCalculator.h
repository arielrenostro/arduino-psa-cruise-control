#ifndef SPEED_CALCULATOR_C
#define SPEED_CALCULATOR_C

#include <Arduino.h>

// Definitions for a Peugeot 206 1.6 16v

#define M1 3.417
#define M2 1.81
#define M3 1.27
#define M4 0.975
#define M5 0.767
#define DF 4.28

#define TIRE_W 185
#define TIRE_H 65
#define TIRE_R 14
#define TIRE_DIAMETER (TIRE_R * 25.4) + (2 * (TIRE_W * (TIRE_H / 100)))

const float TIRE_LENGTH_M = ((TIRE_DIAMETER)*PI) / 1000;

double findGear(uint8_t speed, uint16_t rpm)
{
    double approxGear = (rpm * TIRE_LENGTH_M * 0.06) / (speed * DF);

    double minDelta = 9999;
    double delta = 0;
    double gear = -1;

    delta = fabs(approxGear - M1);
    if (minDelta > delta)
    {
        minDelta = delta;
        gear = M1;
    }

    delta = fabs(approxGear - M2);
    if (minDelta > delta)
    {
        minDelta = delta;
        gear = M2;
    }

    delta = fabs(approxGear - M3);
    if (minDelta > delta)
    {
        minDelta = delta;
        gear = M3;
    }

    delta = fabs(approxGear - M4);
    if (minDelta > delta)
    {
        minDelta = delta;
        gear = M4;
    }

    delta = fabs(approxGear - M5);
    if (minDelta > delta)
    {
        minDelta = delta;
        gear = M5;
    }

    return gear;
}

double calculateSpeedFromRPM(uint8_t speed, uint16_t rpm)
{
    double gear = findGear(speed, rpm);
    return (rpm / DF / gear) // calculate the rotation in wheel
           * TIRE_LENGTH_M   // calculate the tire displacement in meters
           * 0.06;           // convert to hm/h (* 60 / 1000 )
}

#endif