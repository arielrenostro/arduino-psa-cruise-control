#ifndef Button_C
#define Button_C

#include <Arduino.h>

#define BUTTON_HOLD_DELAY 600
#define BUTTON_HOLD_REPEAT_DELAY 300

enum ButtonPressEvent { push, holding, nothing };

class Button
{
private:
  uint8_t _pin;
  bool _holding = false;
  bool _pushed = false;
  unsigned long _pushTime = 0;
  unsigned long _calcAux = 0;

  bool _isPressed();
public:
  Button(uint8_t pin);
  bool setup();
  ButtonPressEvent onLoop();
};

#endif