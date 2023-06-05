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
#include <EEPROM.h>
//#include "ledDisplay.h"
#include "alarmTime.h"
//#include "timer_blink.h"
#include "sensorButton.h"
#include "menuTree.h"

Blink blinkPointsTimer(500);
Timer checkTime(1000);
Timer showTimerRingEffect(5000);
//Timer ws2812Timer(5);

extern DateTime dateTime;
RTCAlarmTime alarm1;

static int8_t ledBrightnessCounter = 0;
static int8_t interim_data = 0;
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
  /* Read the data from EEPROM memory */
  EEPROM.get(TM1637_BRIGHTNESS_ADDR, ledBrightnessCounter);

  /* Checking data and if they is incorrect then to change */
  if (!checkLedBrightness(ledBrightnessCounter))
    EEPROM.put(TM1637_BRIGHTNESS_ADDR, ledBrightnessCounter);

  /* Setup TM1637 display */
  displayTM1637.clear();
  displayTM1637.brightness(ledBrightnessCounter);

  /* Setup LED ring settings */
  FastLED.addLeds<WS2812, WS2812_DI_PIN, COLOR_ORDER>(leds, WS2812_LED_NUM);
  FastLED.setBrightness(WS2812_BRIGHTNESS);

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
  //setAlarm_1(9, 55);
  delay(100);
  alarm1 = getAlarm1();

  Serial.print(alarm1.hour);
  Serial.print(':');
  Serial.print(alarm1.minute);
  Serial.print(":");
  Serial.println(alarm1.second);

  /* Allowing an external interrupt on the SQW signal */
  pinMode(ISR_INPUT_PIN, INPUT_PULLUP); // Input needs to pull up to VCC
  attachInterrupt(0, ISR_RTC_INT, FALLING);  // INT0 attached

  if (!pDS3231->begin()) {
    Serial.println("--DS3231 not found--");
  } else {
    dateTime = pDS3231->getTime();
  }

  if (pDS3231->lostPower()) {
    pDS3231->setTime(COMPILE_TIME);
    Serial.println("--DS3231 lost power--");
  }
  
  displayTime();
  modeStatus = Mode::WORK;
  subMenuState = subMenu::SET_HOURS;
}

void loop() {
  left_btn.tick();
  right_btn.tick();
  set_btn.tick();
  cancel_btn.tick();

  switch (modeStatus) {
/*------------------------------| Mode WORK |--------------------------------*/
    case Mode::WORK:
      if (sensor_btn.press()) {
        showTimerRingEffect.resetCounter();
        while (!showTimerRingEffect.ready()) {
          ws2812_raibow();
        }
        
        FastLED.clear();
        FastLED.show();
      }

      if (left_btn.press() || right_btn.press()) {
        modeStatus = Mode::EDIT;
        menuState = Menu::SELECTION_MENU;
        interim_data = 1;
        displayTM1637.point(false);
      }
      
      if (cancel_btn.press()) {
        displayTM1637.point(true);
        displayTM1637.displayByte(_empty, _empty, _empty, _empty);
        checkTime.resetCounter();
        while (!checkTime.ready()) {
          displayTM1637.display(alarm1.hour / 10,
                                alarm1.hour % 10,
                                alarm1.minute / 10,
                                alarm1.minute % 10);
        }
        displayTime();
      }

      if (checkTime.ready()) displayTime();
      displayTM1637.point(blinkPointsTimer.getStatus());

      break; /* End of case WORK */
    
/*------------------------------| Mode EDIT |--------------------------------*/
    case Mode::EDIT:
      /* Left button activity */
      if (left_btn.press()) {
        if (menuState == Menu::SELECTION_MENU)
          if (--interim_data < 1) interim_data = 3;
        
        if (menuState == Menu::SET_CLOCK || menuState == Menu::SET_ALLARM) {
          if (subMenuState == subMenu::SET_HOURS && --interim_data < 0)
            interim_data = 23;
          if (subMenuState == subMenu::SET_MINUTES && --interim_data < 0)
            interim_data = 59;
        }

        if (menuState == Menu::SET_BRIGHTNESS) {
          if (--interim_data < TM1637_MIN_BRIGHTNESS)
            interim_data = TM1637_MAX_BRIGHTNESS;
          displayTM1637.brightness(interim_data);
        }
      }

      /* Right button activity */
      if (right_btn.press()) {
        if (menuState == Menu::SELECTION_MENU)
          if (++interim_data > 3) interim_data = 1;
        
        if (menuState == Menu::SET_CLOCK || menuState == Menu::SET_ALLARM) {
          if (subMenuState == subMenu::SET_HOURS && ++interim_data > 23)
            interim_data = 0;
          if (subMenuState == subMenu::SET_MINUTES && ++interim_data > 59)
            interim_data = 0;
        }

        if (menuState == Menu::SET_BRIGHTNESS) {
          if (++interim_data > TM1637_MAX_BRIGHTNESS) 
            interim_data = TM1637_MIN_BRIGHTNESS;
          displayTM1637.brightness(interim_data);
        }
      }

      /* Set button activity */
      if (set_btn.press()) {
        if (menuState == Menu::SELECTION_MENU) {
          if (interim_data == 1) {
            menuState = Menu::SET_CLOCK;
            subMenuState = subMenu::SET_HOURS;
            displayTM1637.point(true);
            interim_data = pDS3231->getHours();
          }

          if (interim_data == 2) {
            menuState = Menu::SET_ALLARM;
            subMenuState = subMenu::SET_HOURS;
            interim_data = getAlarm1().hour;
          }

          if (interim_data == 3) {
            menuState = Menu::SET_BRIGHTNESS;
            interim_data = ledBrightnessCounter;
            displayTM1637.displayByte(_empty, _empty, _empty, _empty);
          }

        /* Set Clock */
        } else if (menuState == Menu::SET_CLOCK &&
                   subMenuState == subMenu::SET_HOURS) {
          dateTime.hour = interim_data;   // Transmit data to struct.hour
          /* Update the  minutes so that they don't get lost */ 
          dateTime.minute = pDS3231->getMinutes(); 
          pDS3231->setTime(dateTime);     // Set the DateTime struct in a RTC DS3231
          subMenuState = subMenu::SET_MINUTES;
          interim_data = pDS3231->getMinutes(); // Updating interim for next editing
        } else if (menuState == Menu::SET_CLOCK && 
                   subMenuState == subMenu::SET_MINUTES) {
          dateTime.minute = interim_data; // Put into DateTime struct a minute
          dateTime.second = 0;            // Seconds don't need to setup
          pDS3231->setTime(dateTime);     // Set the DateTime struct in a RTC DS3231
          menuState = Menu::SELECTION_MENU;
          subMenuState = subMenu::SET_HOURS;
          interim_data = 1;                   // Reset intermediate data
        /* Set Allarm 1 */
        } else if (menuState == Menu::SET_ALLARM && subMenuState == subMenu::SET_HOURS) {
          alarm1.hour = interim_data;
          setAlarm_1(alarm1.hour, alarm1.minute);
          subMenuState = subMenu::SET_MINUTES;
          interim_data = getAlarm1().minute;
        } else if (menuState == Menu::SET_ALLARM && subMenuState == subMenu::SET_MINUTES) {
          alarm1.minute = interim_data;
          setAlarm_1(alarm1.hour, alarm1.minute, 0);
          menuState = Menu::SELECTION_MENU;
          subMenuState = subMenu::SET_HOURS;
          interim_data = 2;
        /* Set TM1637 brightness */
        } else if (menuState == Menu::SET_BRIGHTNESS) {
          menuState = Menu::SELECTION_MENU;
          EEPROM.put(TM1637_BRIGHTNESS_ADDR, interim_data);
          interim_data = 3;
        }
      }

      /* Cancel button activity */
      if (cancel_btn.press()) {
        if (menuState == Menu::SELECTION_MENU) {  // Exit to main screen
          modeStatus = Mode::WORK;
        }

        if (menuState == Menu::SET_CLOCK) {
          interim_data = 1;
          menuState = Menu::SELECTION_MENU;
        }
        if (menuState == Menu::SET_ALLARM) {
          interim_data = 2;
          menuState = Menu::SELECTION_MENU;
        }
        if (menuState == Menu::SET_BRIGHTNESS) {
          interim_data = 3;
          menuState = Menu::SELECTION_MENU;
        }
      }
      
      menuPrint(interim_data);
      break; /* End of case EDIT */
    
/*------------------------------| Mode ALARM |-------------------------------*/
    case Mode::ALARM:
      
      break; /* End of case ALARM */

/*------------------------------| Mode ERROR |-------------------------------*/
    case Mode::ERROR:
      
      break; /* End of case ERROR */
  }
}
