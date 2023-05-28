#ifndef _SENSOR_BUTTON_H_
#define _SENSOR_BUTTON_H_

#include <Arduino.h>
#include "timer_blink.h"

class SensorButton {
  public:
    SensorButton(const uint8_t pin) : _pin(pin) {
      pinMode(_pin, INPUT);
    }

    bool press() {
      bool btnState = digitalRead(_pin);
      if (btnState && !_flag) {
        if (_timer->ready() && digitalRead(_pin)) {
          _flag = true;
          _timer->resetCount();
          return true;
        }
      }

      if (!btnState && _flag) {
        _flag = false;
        _timer->resetCount();
      }

      return false;
    }

  private:
    uint8_t _pin;
    bool _flag = false;
    Timer *_timer = new Timer(800);
};

#endif /* _SENSOR_BUTTON_H_ */