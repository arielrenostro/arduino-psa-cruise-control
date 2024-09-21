#ifndef AppController_C
#define AppController_C

#include <Arduino.h>

class AppController
{
private:
    static bool _debug;
    static void _handleDebug();
    AppController();
public:
    static void onLoop();
    static bool isDebug();
    static void setDebug(bool debug);
};

#endif