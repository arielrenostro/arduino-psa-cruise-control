#include "AppController.h"

bool AppController::_debug = false;
CruiseController *AppController::_cruiseController = NULL;

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

        case 'k':
            _handleKey();
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

void AppController::setup(bool debug, CruiseController *cruiseController)
{
    _debug = debug;
    _cruiseController = cruiseController;
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

void AppController::_handleKey()
{

    switch (Serial.read())
    {
    case '+':
        Serial.println(F("k+"));
        _cruiseController->addSpeed(1);
        break;

    case '-':
        Serial.println(F("k-"));
        _cruiseController->subSpeed(1);
        break;

    case 'M':
    case 'm':
        Serial.println(F("kM"));
        _cruiseController->changeMode();
        break;

    case 'S':
    case 's':
        Serial.println(F("kS"));
        _cruiseController->enable();
        break;

    case 'C':
    case 'c':
        Serial.println(F("kC"));
        _cruiseController->disable();
        break;

    default:
        Serial.println(F("Invalid debug instruction"));
        break;
    }

    Serial.read(); // read \n
}