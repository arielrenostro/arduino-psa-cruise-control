#include "AppController.h"

bool AppController::_debug = false;

AppController::AppController()
{
    
}

void AppController::onLoop()
{
    if (Serial.available() > 0)
    {
        switch (Serial.read())
        {
        case 'd':
            _handleDebug();
            break;

        default:
            break;
        }
    }
}

bool AppController::isDebug()
{
    return _debug;
}

void AppController::setDebug(bool debug)
{
    _debug = debug;
}

void AppController::_handleDebug()
{
    switch (Serial.read())
    {
    case '1':
        Serial.println(F("d1"));
        _debug = true;
        break;

    case '0':
        Serial.println(F("d0"));
        _debug = false;
        break;

    default:
        Serial.println(F("Invalid debug instruction"));
        break;
    }

    Serial.read(); // read \n
}
