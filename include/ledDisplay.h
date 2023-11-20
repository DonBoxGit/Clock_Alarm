#ifndef _LED_DISPLAY_H_
#define _LED_DISPLAY_H_

#include <GyverTM1637.h>
#include "alarmTime.h"

GyverTM1637 displayTM1637(TM1637_CLK_PIN, TM1637_DIO_PIN);
uint32_t Now, clocktimer;
boolean flag;

void displayTime(void) {
  if (!getTimeRTC()[0]) displayTM1637.displayByte(0, _empty);
  else displayTM1637.display(0, getTimeRTC()[0]);
  displayTM1637.display(1, getTimeRTC()[1]);
  displayTM1637.display(2, getTimeRTC()[2]);
  displayTM1637.display(3, getTimeRTC()[3]);
}

bool checkLedBrightness(int8_t &brightness) {
  if (brightness > TM1637_MAX_BRIGHTNESS || brightness < TM1637_MIN_BRIGHTNESS) {
    brightness = 0;
    return false;
  }
  return true;
}

#endif /* _LED_DISPLAY_H_ */