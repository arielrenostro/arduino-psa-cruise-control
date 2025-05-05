#include "KLineController.h"
#include "../App/AppController.h"

KLineController::KLineController()
{
}

bool KLineController::setup()
{
    Serial.print(F("KLine ISO1430_Fast: "));
    Serial.println(F("configuring serial pins"));
    pinMode(K_LINE_RX, INPUT_PULLUP);
    pinMode(K_LINE_TX, OUTPUT);
    _serial.begin(10400);
    _serial.end();
    return true;
}

void KLineController::onLoop()
{
    if (millis() < _nextHandle)
    {
        return;
    }

    switch (_state)
    {
    case disconnected:
        _init();
        break;

    case initializing_1:
    case initializing_2:
    case initializing_3:
    case initializing_4:
        _handleInitializing();
        break;

    case reading:
        _handleReading();
        break;

    case writing:
        _handleWriting();
        break;

    case idle:
        _handleIdle();
        break;

    default:
        break;
    }
}

DataEvent KLineController::getSpeed()
{
    return _speedData;
}

DataEvent KLineController::getRpm()
{
    return _rpmData;
}

bool KLineController::isConnected()
{
    return _connected;
}

void KLineController::clearRpm()
{
    _rpmData.time = 0;
    _rpmData.value = 0;
}

void KLineController::_init()
{
    _connected = false;
    _setState(initializing_1, 0);
}

void KLineController::_disconnect()
{
    _connected = false;
    _serial.end();
    _setState(disconnected, 0);
}

void KLineController::_handleInitializing()
{
    switch (_state)
    {
    case initializing_1:
        Serial.print(F("KLine ISO1430_Fast: "));
        Serial.println(F("starting"));
        _serial.end();
        digitalWrite(K_LINE_TX, HIGH);
        _setState(initializing_2, 1000);
        break;

    case initializing_2:
        digitalWrite(K_LINE_TX, LOW);
        _setState(initializing_3, 25);
        break;

    case initializing_3:
        digitalWrite(K_LINE_TX, HIGH);
        _setState(initializing_4, 25);
        break;

    case initializing_4:
        _serial.begin(10400);
        _setWriteData(ISO1430_FAST_INIT_HEADER, sizeof(ISO1430_FAST_INIT_HEADER), ISO1430_FAST_INIT);
        break;

    default:
        break;
    }
}

void KLineController::_handleIdle()
{
    if (_speedData.time == 0 || millis() - _speedData.time > 1000)
    {
        _requestSpeed();
    }
    else
    {
        _requestRpm();
    }
}

void KLineController::_handleWriting()
{
    _serial.write(_wBuff[_wIdx]);
    _wIdx++;
    if (_wLen > _wIdx)
        _setState(writing, K_LINE_READ_DELAY);
    else
        _setState(reading, K_LINE_REQUEST_DELAY);
}

void KLineController::_handleReading()
{
    if (_rLen == 0) // response length not defined yet
    {
        uint8_t result = _serial.available();
        if (result > 0)
        {
            _rLen = min(result, int(sizeof(_rBuff)));
        }
    }

    if (_rLen > 0) // response length defined
    {
        while (_serial.available() > 0 && _rLen > _rIdx) // read bytes
        {
            _rBuff[_rIdx] = _serial.read();
            _rIdx++;
        }

        if (_rLen > _rIdx) // still have bytes to read
        {
            _setState(reading, K_LINE_READ_DELAY);
            return;
        }

        if (_rLen == _rIdx) // read finished
        {
            _serial.flushInput(); // clear read buffer
            _processReadMessage();
            _countConnectionTimeout = 0;
            return;
        }
    }

    if (millis() - _nextHandle > K_LINE_READ_TIMEOUT) // timeout exceded
    {
        Serial.print(F("KLine ISO1430_Fast: "));
        Serial.println(F("read timeout"));
        _serial.flushInput(); // clear read buffer
        _processReadMessage();

        _countConnectionTimeout++;
        if (_countConnectionTimeout % K_LINE_READ_TIMEOUT_LIMIT == 0)
        {
            _disconnect();
        }
    }
}

void KLineController::_processReadMessage()
{
    if (AppController::isDebug()) // dump read when debuging
    {
        Serial.print(F("KLine ISO1430_Fast: "));
        Serial.print(F("read dump: "));
        _dump(_rBuff, _rLen);
        Serial.print(F("\n"));
    }

    if (_isSame(_wBuff, ISO1430_FAST_INIT_HEADER, sizeof(ISO1430_FAST_INIT_HEADER))) // start communication
    {
        _handleStartCommunicationResponse();
    }
    else if (_isSame(_wBuff, ISO1430_FAST_LIVE_DATA_HEADER, sizeof(ISO1430_FAST_LIVE_DATA_HEADER))) // live data
    {
        _handleLiveDataResponse();
    }
    else
    {
        Serial.print(F("KLine ISO1430_Fast: "));
        Serial.print(F("unknow response: "));
        _dump(_rBuff, _rLen);
        Serial.print(F("\n"));
        _setState(idle, 0);
    }
}

void KLineController::_handleStartCommunicationResponse()
{
    Serial.print(F("KLine ISO1430_Fast: "));
    if (_rLen >= 9 && _rBuff[8] == 0xC1) // success!
    {
        Serial.println(F("started"));
        _setState(idle, 0);
        _connected = true;
        _countConnectionTimeout = 0;
    }
    else
    {
        Serial.print(F("failure: "));
        _dump(_rBuff, _rLen);
        Serial.print(F("\n"));
        _disconnect();
        _countConnectionTimeout++;
    }
}

void KLineController::_handleLiveDataResponse()
{
    if (_rLen >= 12 && _rBuff[10] == ISO1430_FAST_VEHICLE_SPEED_PID) // vehicle speed response
    {
        _speedData.value = _rBuff[11];
        _speedData.time = millis();
    }
    else if (_rLen >= 13 && _rBuff[10] == ISO1430_FAST_VEHICLE_RPM_PID) // vehicle rpm response
    {
        _rpmData.value = (_rBuff[11] * 256 + _rBuff[12]) / 4;
        _rpmData.time = millis();
    }
    else
    {
        Serial.print(F("KLine ISO1430_Fast: "));
        Serial.print(F("unknow live data response: "));
        _dump(_rBuff, _rLen);
        Serial.print(F("\n"));
    }
    _setState(idle, 0);
}

void KLineController::_setState(KLineState state, uint16_t time)
{
    _state = state;
    _nextHandle = millis() + time;
}

void KLineController::_setWriteData(const byte data[], int length, const byte pid)
{
    // reset read
    _rIdx = 0;
    _rLen = 0;

    // set read
    _wIdx = 0;
    _wLen = length + 2;
    _wBuff[length] = pid;
    memcpy(_wBuff, data, length); // copy to write buff
    _calculateChecksum();

    _setState(writing, 0);

    if (AppController::isDebug()) // dump write when debuging
    {
        Serial.print(F("KLine ISO1430_Fast: "));
        Serial.print(F("write dump: "));
        _dump(_wBuff, _wLen);
        Serial.print(F("\n"));
    }
}

void KLineController::_calculateChecksum()
{
    uint8_t checksumIdx = _wLen - 1; // checksum is the last byte
    _wBuff[checksumIdx] = 0;
    for (uint8_t i = 0; i < checksumIdx; i++)
    {
        _wBuff[checksumIdx] += _wBuff[i];
    }
    _wBuff[checksumIdx] %= 256;
}

void KLineController::_requestSpeed()
{
    _setWriteData(ISO1430_FAST_LIVE_DATA_HEADER, sizeof(ISO1430_FAST_LIVE_DATA_HEADER), ISO1430_FAST_VEHICLE_SPEED_PID);
}

void KLineController::_requestRpm()
{
    _setWriteData(ISO1430_FAST_LIVE_DATA_HEADER, sizeof(ISO1430_FAST_LIVE_DATA_HEADER), ISO1430_FAST_VEHICLE_RPM_PID);
}

void KLineController::_dump(const byte data[], uint8_t length)
{
    for (uint8_t i = 0; i < length; i++)
    {
        Serial.print(data[i], HEX);
        Serial.print(F(" "));
    }
}

bool KLineController::_isSame(const byte b1[], const byte b2[], uint8_t length)
{
    for (uint8_t i = 0; i < length; i++)
    {
        if (b1[i] != b2[i])
        {
            return false;
        }
    }
    return true;
}

uint8_t KLineController::getCountConnectionTimeout()
{
    return _countConnectionTimeout;
}