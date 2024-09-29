#ifndef KLineController_C
#define KLineController_C

#include <Arduino.h>
#include <AltSoftSerial.h>
#include "../App/AppController.h"

#define K_LINE_RX 8
#define K_LINE_TX 9
#define K_LINE_BAUDRATE 10400
#define K_LINE_READ_DELAY 5
#define K_LINE_REQUEST_DELAY 50
#define K_LINE_READ_TIMEOUT 1000

#define ISO1430_FAST_INIT 0x81
#define ISO1430_FAST_VEHICLE_SPEED_PID 0x0D

const byte ISO1430_FAST_INIT_HEADER[3] = {0xC1, 0x33, 0xF1};
const byte ISO1430_FAST_LIVE_DATA_HEADER[4] = {0xC2, 0x33, 0xF1, 0x01};

struct SpeedData
{
    uint8_t speed;
    uint64_t time;
};

enum KLineState
{
    disconnected,   // 0
    reading,        // 1
    writing,        // 2
    idle,           // 3
    initializing_1, // 4
    initializing_2, // 5
    initializing_3, // 6
    initializing_4, // 7
};

class KLineController
{
private:
    AltSoftSerial _serial;
    KLineState _state = disconnected;
    bool _connected;
    SpeedData _speedData = SpeedData();
    uint64_t _nextHandle = 0;

    byte _wBuff[8];
    uint8_t _wIdx;
    uint8_t _wLen;

    byte _rBuff[16];
    uint8_t _rIdx;
    uint8_t _rLen;

    void _init();
    void _disconnect();
    void _handleInitializing();
    void _handleWriting();
    void _handleReading();
    void _handleStartCommunicationResponse();
    void _handleLiveDataResponse();
    void _setState(KLineState state, uint16_t time);
    void _setWriteData(const byte data[], int length, const byte pid);
    void _calculateChecksum();
    void _processReadMessage();
    void _requestSpeed();
    void _dump(const byte data[], uint8_t length);
    bool _isSame(const byte b1[], const byte b2[], uint8_t length);

public:
    KLineController();
    bool setup();
    void onLoop();
    SpeedData getSpeed();
    bool isConnected();
};

#endif