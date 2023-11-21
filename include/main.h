#ifndef _MAIN_H_
#define _MAIN_H_

#include <Wire.h>
#include <SoftwareSerial.h>
#include <EncButton.h>
#include <DFRobotDFPlayerMini.h>

#include "alarmTime.h"
#include "sensorButton.h"
#include "menuTree.h"
#include "ws2812Effects.h"

/* Initialization of buttons for control */
EncButton<EB_TICK, LEFT_BUTTON_PIN>   left_btn   (INPUT_PULLUP);
EncButton<EB_TICK, RIGHT_BUTTON_PIN>  right_btn  (INPUT_PULLUP);
EncButton<EB_TICK, SET_BUTTON_PIN>    set_btn    (INPUT_PULLUP);
EncButton<EB_TICK, CANCEL_BUTTON_PIN> cancel_btn (INPUT_PULLUP);
SensorButton sensor_btn(SENSOR_MODULE_PIN);

/* Create softSerial and mp3player objects */
SoftwareSerial softSerial(DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
DFRobotDFPlayerMini mp3Player;

Blink blinkPointsTimer(DOTS_TIMER);
Timer checkTime(NUMBERS_TIMER);
Timer ringEffectShowTimer(EFFECT_SHOW_TIMER);
Timer ws2812Timer(WS_ALGORITHM_SPEED);

#ifdef RTC_DS3231
  extern DateTime dateTime;

  /* RTC Alarm interrupt function */
  void ISR_RTC_INT() { modeStatus = Mode::ALARM; }
#endif /* RTC_DS3231 */

extern RTCAlarmTime alarm1;

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

#endif /* _MAIN_H_*/