#ifndef BUTTON_PRESS_EVENT_C
#define BUTTON_PRESS_EVENT_C

enum PressType
{
    push,
    holding,
    nothing
};

struct ButtonPressEvent
{
    uint8_t buttonIdx;
    PressType type;
};

#endif