#include "timer_blink.h"

/*-----------------------------| Class Timer |-------------------------------*/
Timer::Timer(uint32_t period)  : _period(period) {}

Timer::~Timer() {}

bool Timer::ready() {
  if (millis() - _tmr >= _period) {
    _tmr = millis();
    return true;
  }
      return false;
}

void Timer::setPeriod(uint32_t period) { _period = period; }
void Timer::resetCounter() { _tmr = millis(); }
uint32_t Timer::getPeriod() { return _period; }
uint32_t Timer::getTmr() { return _tmr; }

/*-----------------------------| Class Blink |-------------------------------*/
Blink::Blink(uint32_t period) : Timer(period) {}
  
Blink::~Blink() {}

bool Blink::getStatus() {
  if(ready()) _flag = !_flag;
  return _flag;
}

void Blink::resetStatus() { _flag = false; }
void Blink::toggle() { _flag = !_flag; }
