#ifndef ANALOG_BUTTON_C
#define ANALOG_BUTTON_C

#include <Arduino.h>
#include "ButtonPressEvent.h"

#define BUTTON_ANALOG_UNPRESSED 1020
#define BUTTON_NOT_PRESSED 255

class AnalogButton
{
private:
  uint8_t _pin;
  uint16_t *_voltages;
  uint8_t _voltagesLength;

  uint8_t _currentButton = BUTTON_NOT_PRESSED;
  bool _holding = false;
  bool _pushed = false;
  unsigned long _pushTime = 0;
  unsigned long _calcAux = 0;

  bool _isPressed();
  bool _isSameButton();
  uint8_t _readCurrentButton();

public:
  AnalogButton(uint8_t pin, uint16_t *voltages, uint8_t length);
  bool setup();
  void onLoop(ButtonPressEvent *evt);
};

#endif