#ifndef AppController_C
#define AppController_C

#include <Arduino.h>
#include "../Cruise/CruiseController.h"

class AppController
{
private:
    static bool _debug;
    static CruiseController *_cruiseController;
    static void _handleDebug();
    static void _handleKey();
    AppController();
public:
    static void onLoop();
    static bool isDebug();
    static void setup(bool debug, CruiseController *cruiseController);
};

#endif