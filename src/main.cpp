/******************************************************************
 *                      Clock Alarm Mp3Player                     *
 *                            ver. 1.0.0                          *
 *    Used RTC DS3231 on I2C wire, 4-Digit LED Display, WS2812B   *
 *  LED Ring 16 leds and DFPlayer Mini.                           *
 *                                                                *
 *                                     Autor: Roman Yakubovskiy   *
 ******************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <EncButton.h>
#include <FastLED.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
//#include "ledDisplay.h"
#include "alarmTime.h"
//#include "timer_blink.h"
#include "sensorButton.h"
#include "menuTree.h"

Blink blinkPointsTimer(500);
Timer checkTime(1000);
Timer ws2812Timer(5);

RTCAlarmTime alarm1;
//static int8_t ledBrightnessCounter = 0;
uint8_t ws2812_counter;

enum class Mode : uint8_t {
  WORK = 0,
  EDIT,
  ALARM,
  ERROR
} modeStatus;

/* RTC Alarm interrupt function */
void ISR_RTC_INT() { Serial.println("RTC Alarm trigered!"); }

/* Initialization of buttons for control */
EncButton<EB_TICK, LEFT_BUTTON_PIN>   left_btn   (INPUT_PULLUP);
EncButton<EB_TICK, RIGHT_BUTTON_PIN>  right_btn  (INPUT_PULLUP);
EncButton<EB_TICK, SET_BUTTON_PIN>    set_btn    (INPUT_PULLUP);
EncButton<EB_TICK, CANCEL_BUTTON_PIN> cancel_btn (INPUT_PULLUP);
SensorButton sensor_btn(SENSOR_MODULE_PIN);

/* Initialization of LED ring */
CRGB leds[WS2812_LED_NUM];

void ws2812_raibow(void) {
  for (int i = 0; i < WS2812_LED_NUM; i++) {
  leds[i].setHue(ws2812_counter + i * 255 / WS2812_LED_NUM);
  }
  ws2812_counter++;
  FastLED.show();
}

void setup() {
  Serial.begin(9600);

  /* Setup LED settings */
  FastLED.addLeds<WS2812, WS2812_DI_PIN, COLOR_ORDER>(leds, WS2812_LED_NUM);
  FastLED.setBrightness(WS2812_BRIGHTNESS);

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
  delay(100);
  alarm1 = getAlarm1();

  Serial.print(alarm1.hour + ":");
  Serial.println(alarm1.minute);

  /* Allowing an external interrupt on the SQW signal */
  pinMode(ISR_INPUT_PIN, INPUT_PULLUP); // Input needs to pull up to VCC
  attachInterrupt(0, ISR_RTC_INT, FALLING);  // INT0 attached

  if (!pDS3231->begin())
    Serial.println("--DS3231 not found--");

  if (pDS3231->lostPower())
    Serial.println("--DS3231 lost power--");

  displayTime();
  modeStatus = Mode::WORK;
}

void loop() {
  left_btn.tick();
  right_btn.tick();
  set_btn.tick();
  cancel_btn.tick();

  switch (modeStatus) {
    case Mode::WORK:
      if (sensor_btn.press()) Serial.println("Sensor btn pressed");

      if (left_btn.press()) Serial.println("Left pressed");
      if (right_btn.press()) Serial.println("Right pressed");
      if (set_btn.press()) {
        modeStatus = Mode::EDIT;
        menuStatus = Menu::SET_CLOCK;
      }
      if (cancel_btn.press()) Serial.println("Cancel pressed");

      if (checkTime.ready()) displayTime();
      displayTM1637.point(blinkPointsTimer.getStatus());

      break;

    case Mode::EDIT:
      if (set_btn.press()) menuStatus = Menu::SET_ALLARM;
      if (cancel_btn.press()) modeStatus = Mode::WORK;
      menuPrint();
      break;

    case Mode::ALARM:
      break;

    case Mode::ERROR:
      break;
  }
}
