#ifndef _MAIN_H_
#define _MAIN_H_

#include <Wire.h>
#include <EncButton.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include "alarmTime.h"
#include "sensorButton.h"
#include "menuTree.h"
#include "ws2812Effects.h"

Blink blinkPointsTimer(DOTS_TIMER);
Timer checkTime(NUMBERS_TIMER);
Timer ringEffectShowTimer(EFFECT_SHOW_TIMER);
Timer ws2812Timer(WS_ALGORITHM_SPEED);

extern DateTime dateTime;
RTCAlarmTime alarm1;

/* Static variables for interim data */
static int8_t ledBrightnessCounter = 0;
static int8_t interim_data = 0;
static bool ledRingflag = false;
static int8_t mp3Volume;
static uint8_t effectNumber;

/* Working modes of Alarm clock device */
enum class Mode : uint8_t {
  WORK = 0,
  EDIT,
  ALARM,
  ERROR
} modeStatus;

/* Enumeration of errors */
enum class AlarmClockErrors : uint8_t {
  RTC_I2C_NOT_RESPONSE = 0,
  DFPLAYER_SERIAL_ERROR
} alarmClockError;

/* RTC Alarm interrupt function */
void ISR_RTC_INT() { modeStatus = Mode::ALARM; }

#endif /* _MAIN_H_*/