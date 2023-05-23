/**************************************************************
 *                     Clock Alarm Mp3Player                  *
 *                         ver. 1.0.0                         *
 *     Used RTC DS3231 on I2C wire, 4-Digit LED Display,      *
 *  WS2812B LED Ring 16 leds and DFPlayer Mini.               *
 *                                                            
 *                                   Autor: Roman Ykubovskiy  *
 **************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <EncButton.h>
#include <FastLED.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include "ledDisplay.h"
#include "alarmTime.h"
#include "timer_blink.h"

Blink blinkPointsTimer(500);
Timer checkTime(1000);
RTCAlarmTime alarm1;
static int8_t ledBrightnessCounter = 0;

enum class Mode : uint8_t {
  WORK = 0,
  EDIT,
  ALARM,
  ERROR
} modeStatus;

enum class Menu : uint8_t {

};

void ISR_RTC_INT() { Serial.println("RTC Alarm trigered!"); }
/* Initialization of buttons for control */
EncButton<EB_TICK, LEFT_BUTTON_PIN>  left_btn  (INPUT_PULLUP);
EncButton<EB_TICK, RIGHT_BUTTON_PIN> right_btn (INPUT_PULLUP);
//EncButton<EB_TICK, SENSOR_MODULE_PIN>sensor_mod(INPUT_PULLUP);

void setup() {
  Serial.begin(9600);

  displayTM1637.clear();
  displayTM1637.brightness(MIN_BRIGHTNESS);

  uint8_t value = readRegisterDS3231(RTC_I2C_ADDR, CONTROL_REGISTER);
  Serial.println(value);
  if (value != 0x00) {
    value = 0x00;
  }
  /* Allow interrupt to INT/SQW by Alarm 1 */
  value |= (1 << A1IE_BIT) | (1 << INTCN_BIT);
  writeRegisterDS3231(RTC_I2C_ADDR, CONTROL_REGISTER, value);

  value = readRegisterDS3231(RTC_I2C_ADDR, CONTROL_REGISTER);
  Serial.println(value);
  // setAlarm_1(20, 5, 0);
  delay(100);
  // alarm1 = getAlarm1();

  // Serial.print(alarm1.hour + ":");
  // Serial.println(alarm1.minute);

  /* Allowing an external interrupt on the SQW signal */
  pinMode(ISR_INPUT_PIN, INPUT_PULLUP); // Input needs to pull up to VCC
  attachInterrupt(0, ISR_RTC_INT, FALLING);  // INT0 attached

  pinMode(SENSOR_MODULE_PIN, INPUT);

  if (!pDS3231->begin())
    Serial.println("--DS3231 not found--");

  if (pDS3231->lostPower())
    Serial.println("--DS3231 lost power--");

  displayTime();
  modeStatus = Mode::WORK;
}

void loop() {
   if (checkTime.ready()) displayTime();

   displayTM1637.point(blinkPointsTimer.getStatus());
}
