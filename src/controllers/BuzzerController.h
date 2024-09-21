#ifndef BuzzerController_C
#define BuzzerController_C

class BuzzerController
{
private:
    
public:
    BuzzerController();
    void onLoop();
    void playOverspeed();
    void playSpeedLimitEnabled();
    void playSpeedLimitDisabled();
    void playDisabledBySlowKLine();
};

#endif
