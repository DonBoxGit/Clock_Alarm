/**************************************************************
 *                     Clock Alarm Mp3Player                  *
 *                         ver. 1.0.0                         *
 *     Used RTC DS3231 on I2C wire, 4-Digit LED Display,      *
 * WS2812B LED Ring 16 leds and DFPlayer Mini.                *
 **************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <EncButton2.h>
#include <FastLED.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include "ledDisplay.h"
#include "alarmTime.h"
#include "timer_blink.h"

Blink blinkPointsTimer(500);
Timer checkTime(1000);
RTCAlarmTime alarm1;

void ISR_RTC_INT() { Serial.println("RTC Alarm trigered!"); }

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
  setAlarm_1(20, 5, 0);
  delay(1000);
  // alarm1 = getAlarm1();

  // Serial.print(alarm1.hour + ":");
  // Serial.println(alarm1.minute);

  /* Allowing an external interrupt on the SQW signal */
  pinMode(ISR_INPUT_PIN, INPUT_PULLUP); // Input needs to pull up to VCC
  attachInterrupt(0, ISR_RTC_INT, FALLING);  // INT0 attached

  if (!pDS3231->begin())
    Serial.println("--DS3231 not found--");

  if (pDS3231->lostPower())
    Serial.println("--DS3231 lost power--");

  displayTime();
}

void loop() {
   if (checkTime.ready()) displayTime();

   displayTM1637.point(blinkPointsTimer.getStatus());
}
