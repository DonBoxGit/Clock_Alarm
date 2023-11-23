/************************************************************************
 *                      Clock Alarm Mp3Player                           *
 *                            ver. 1.0.2                                *
 *    Used RTC DS3231 on I2C wire, 4-Digit LED Display, WS2812B LED     *
 *  Ring 16 leds and DFPlayer Mini.                                     *
 *    Libraries are used EncButton, GyverTM1637 and microDS3231 by      *
 *  AlexGyver, DFRobotDFPlayerMini by DFRobot and Fast LED by           *
 *  Daniel Garcia.                                                      *
 *    Also used libraries for EEPROM  original copyright                *
 *  David A. Mellis (New version by Christopher Andrews 2015), and      *
 *  TwoWire.h library for Arduino & Wiring copyright Nicholas Zambetti. *
 *                                                                      *
 *                                         Autor: Roman Yakubovskiy     *
 ************************************************************************/

#include "main.h"

void setup() {
#ifdef DEBUG_ALARM_CLOCK
  Serial.begin(9600);
#endif /* DEBUG_ALARM_CLOCK */
#ifdef DF_MP3_PLAYER
  softSerial.begin(9600);
#endif /* DF_MP3_PLAYER */

  /* Read the data from EEPROM memory */
  EEPROM.get(TM1637_BRIGHTNESS_ADDR, ledBrightnessCounter);
  EEPROM.get(DFPLAYER_VOLUME_VALUE_ADDR, mp3Volume);
  EEPROM.get(WS_EFFECT_NUMBER_ADDR, effectNumber);
#ifdef RTC_DS1307
  EEPROM.get(RTC_ALARM_SECONDS, alarm1.second);
  EEPROM.get(RTC_ALARM_MINUTES, alarm1.minute);
  EEPROM.get(RTC_ALARM_HOURS, alarm1.hour);
  EEPROM.get(RTC_ALARM_DAY, alarm1.day);

  /* Checking alarm clock data and correct */
  if (alarm1.second > 59) alarm1.second = 0;
  if (alarm1.minute > 59) alarm1.minute = 0;
  if (alarm1.hour > 23) alarm1.hour = 0;
  #ifdef DEBUG_ALARM_CLOCK
    char strAlarm[15];
    sprintf(strAlarm, "Alarm: %d:%d:%d", alarm1.hour, alarm1.minute, alarm1.second);
    Serial.println(strAlarm);
  #endif /* DEBUG_ALARM_CLOCK */
#endif /* RTC_DS1307 */

  /* Checking data and if they is incorrect then to change */
  if (!checkLedBrightness(ledBrightnessCounter)) {
    EEPROM.put(TM1637_BRIGHTNESS_ADDR, ledBrightnessCounter);
    eeprom_busy_wait();
    _delay_ms(10);
  }
#ifdef DEBUG_ALARM_CLOCK
  Serial.print("Brightness: ");
  Serial.println(ledBrightnessCounter);
  Serial.print("Volume: ");
  Serial.println(mp3Volume);
  Serial.print("Number of effect: ");
  Serial.println(effectNumber);
#endif /* DEBUG_ALARM_CLOCK */

  /* Setup TM1637 display */
  displayTM1637.clear();
  displayTM1637.brightness(ledBrightnessCounter);
 
  /* Setup LED ring settings */
  FastLED.addLeds<WS2812, WS2812_DI_PIN, COLOR_ORDER>(leds, WS2812_LED_NUM);
  FastLED.setBrightness(WS2812_BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

#ifdef RTC_DS3231
  uint8_t value = readRegisterDS3231(RTC_I2C_ADDR, CONTROL_REGISTER);
  if (value != 0x00) value = 0x00;

  /* Allow interrupt to INT/SQW by Alarm 1 */
  value |= (1 << A1IE_BIT) | (1 << INTCN_BIT);
  writeRegisterDS3231(RTC_I2C_ADDR, CONTROL_REGISTER, value);
  alarm1 = getAlarm_1();

  /* Allowing an external interrupt on the SQW signal */
  pinMode(ISR_INPUT_PIN, INPUT_PULLUP); // Input needs to pull up to VCC
  attachInterrupt(0, ISR_RTC_INT, FALLING);  // INT0 attached
#endif /* RTC_DS3231 */

#ifdef DF_MP3_PLAYER
  /* Checking the equipment for the response */
  if (!mp3Player.begin(softSerial)) {
#ifdef DEBUG_ALARM_CLOCK
    Serial.println("MP3player Error.");
#endif /* DEBUG_ALARM_CLOCK */
    modeStatus = Mode::ERROR;
    alarmClockError = AlarmClockErrors::DFPLAYER_SERIAL_ERROR;
  } else {
    Serial.println("MP3player OK.");
    /* Constrain and set volume of mp3 player */
    mp3Volume = constrain(mp3Volume, DFPLAYER_MIN_VOLUME, DFPLAYER_MAX_VOLUME);
    mp3Player.volume(mp3Volume);
    mp3Player.EQ(DFPLAYER_EQ_NORMAL);
    mp3Player.outputDevice(DFPLAYER_DEVICE_SD);
  }
#endif /* DF_MP3_PLAYER */

  if (!pRTC->begin()) {
#ifdef DEBUG_ALARM_CLOCK
    Serial.println("RTC error.");
#endif /* DEBUG_ALARM_CLOCK */
    modeStatus = Mode::ERROR;
    alarmClockError = AlarmClockErrors::RTC_I2C_NOT_RESPONSE;
  } else {
#ifdef DEBUG_ALARM_CLOCK
    Serial.println("RTC is OK!");
#endif /* DEBUG_ALARM_CLOCK */
#ifdef RTC_DS1307  
    pRTC->start();
#endif /* RTC_DS1307 */
    dateTime = pRTC->getTime();
#ifdef DEBUG_ALARM_CLOCK
    char strTime[14];
    sprintf(strTime, "Time: %d:%d:%d", dateTime.hour, dateTime.minute, dateTime.second);
    Serial.println(strTime);
#endif /* DEBUG_ALARM_CLOCK */
    modeStatus = Mode::WORK;
    subMenuState = subMenu::SET_HOURS;
  }

  /* If the battery is too low level and the power is lost */
  if (pRTC->lostPower()) {
#ifdef DEBUG_ALARM_CLOCK
    Serial.println("RTC is LOW battary.");
#endif /* DEBUG_ALARM_CLOCK */

#ifdef RTC_DS3231    
    pRTC->setTime(COMPILE_TIME);
#endif /* RTC_DS3231 */

#ifdef RTC_DS1307
    pRTC->setCompTime();
#endif /* RTC_DS1307 */

    modeStatus = Mode::EDIT;
    menuState = Menu::SET_CLOCK;
    subMenuState = subMenu::SET_HOURS;
    displayTM1637.point(true);
    interim_data = pRTC->getHours();
  }

  displayTime();
}

void loop() {
  /* Buttons polling by the EncButton library */
  left_btn.tick();
  right_btn.tick();
  set_btn.tick();
  cancel_btn.tick();

  switch (modeStatus) {
/*------------------------------| Mode WORK |--------------------------------*/
    case Mode::WORK:
    static bool mp3FlagPlayer = false;
    static bool buttonsVolumeFlag = false;
    if (set_btn.press()) {
      if (!mp3FlagPlayer) {
#ifdef DF_MP3_PLAYER
        mp3Player.play(SONG_NUMBER);
#endif /* DF_MP3_PLAYER */
        mp3FlagPlayer = true;
        displayTM1637.displayByte(_empty, _empty, _empty, _empty);
        displayTM1637.point(false);
        displayTM1637.display(0, mp3Volume / 10);
        displayTM1637.display(1, mp3Volume % 10);
      } else {
#ifdef DF_MP3_PLAYER
        mp3Player.stop();
#endif /* DF_MP3_PLAYER */
        mp3FlagPlayer = false;
        EEPROM.put(DFPLAYER_VOLUME_VALUE_ADDR, mp3Volume);
        eeprom_busy_wait();
        _delay_ms(10);
        displayTime();
      }
    }

    if (sensor_btn.press() && !ledRingflag && !mp3FlagPlayer) {
      ringEffectShowTimer.resetCounter();
      ledRingflag = true;
    }

    if (ledRingflag) {
      while (!ringEffectShowTimer.ready()) {
        func[effectNumber]();

        /* Show time while LED Ring works */
        if (checkTime.ready()) displayTime();
        displayTM1637.point(blinkPointsTimer.getStatus());

        /* Tracing the sensor button click */
        set_btn.tick();
        if (sensor_btn.press()) {
          if (++effectNumber > (QUANTITY_EFFECTS - 1)) effectNumber = 0;
          ringEffectShowTimer.resetCounter();
        }
      }
        
      FastLED.clear();
      FastLED.show();
      ledRingflag = false;
      EEPROM.put(WS_EFFECT_NUMBER_ADDR, effectNumber);
      eeprom_busy_wait();
      _delay_ms(10);
      }

      if (!mp3FlagPlayer) {
        if (left_btn.press() || right_btn.press()) {
          modeStatus = Mode::EDIT;
          menuState = Menu::SELECTION_MENU;
          interim_data = 1;
          displayTM1637.point(false);
        }
      } else { /* When the music play */
        if (left_btn.press()) {
          if (--mp3Volume < DFPLAYER_MIN_VOLUME) mp3Volume = DFPLAYER_MIN_VOLUME;
#ifdef DF_MP3_PLAYER
          mp3Player.volume(mp3Volume);
#endif /* DF_MP3_PLAYER */
          buttonsVolumeFlag = true;
          displayTM1637.displayByte(_empty, _empty, _empty, _empty);
        }

        if (right_btn.press()) {
          if (++mp3Volume > DFPLAYER_MAX_VOLUME) mp3Volume = DFPLAYER_MAX_VOLUME;
#ifdef DF_MP3_PLAYER
          mp3Player.volume(mp3Volume);
#endif /* DF_MP3_PLAYER */
          buttonsVolumeFlag = true;
          displayTM1637.displayByte(_empty, _empty, _empty, _empty);
        }
      }
      
      if (cancel_btn.press()) {
        if (!mp3FlagPlayer) {
          displayTM1637.point(true);
          displayTM1637.displayByte(_empty, _empty, _empty, _empty);
          checkTime.resetCounter();
          while (!checkTime.ready()) {
            displayTM1637.display(alarm1.hour / 10,
                                  alarm1.hour % 10,
                                  alarm1.minute / 10,
                                  alarm1.minute % 10);
          }
        } else {
#ifdef DF_MP3_PLAYER
          mp3Player.stop();
#endif /* DF_MP3_PLAYER */
          mp3FlagPlayer = false;
        }
        displayTime();
      }

      if (!mp3FlagPlayer) {
        if (checkTime.ready()) {
          displayTime();
#ifdef DEBUG_ALARM_CLOCK
          char strTime[8];
          sprintf(strTime, "%d:%d:%d", dateTime.hour, dateTime.minute, dateTime.second);
          Serial.println(strTime);
#endif /* DEBUG_ALARM_CLOCK */
#ifdef RTC_DS1307
        /* Check the alarm time */
          if (alarm1.hour == dateTime.hour &&
              alarm1.minute == dateTime.minute &&
              alarm1.second == dateTime.second) {
            modeStatus = Mode::ALARM;
            resetMillis();
          }
#endif /* RTC_DS1307 */
        }
        displayTM1637.point(blinkPointsTimer.getStatus());
      } else {
        if (buttonsVolumeFlag) {
          displayTM1637.display(0, mp3Volume / 10);
          displayTM1637.display(1, mp3Volume % 10);
          buttonsVolumeFlag = false;
        }
      }
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
            interim_data = pRTC->getHours();
          }

          if (interim_data == 2) {
            menuState = Menu::SET_ALLARM;
            subMenuState = subMenu::SET_HOURS;
            interim_data = getAlarm_1().hour;
          }

          if (interim_data == 3) {
            menuState = Menu::SET_BRIGHTNESS;
            interim_data = ledBrightnessCounter;
            displayTM1637.displayByte(_empty, _empty, _empty, _empty);
          }

        /* Set Clock */
        } else if (menuState == Menu::SET_CLOCK &&
                   subMenuState == subMenu::SET_HOURS) {
          dateTime.hour = interim_data;      // Transmit data to struct.hour
          /* Update the minutes so that they don't get lost */ 
          dateTime.minute = pRTC->getMinutes(); 
          pRTC->setTime(dateTime);           // Set the DateTime struct in a RTC DS3231
          subMenuState = subMenu::SET_MINUTES;
          interim_data = pRTC->getMinutes(); // Updating interim for next editing
        } else if (menuState == Menu::SET_CLOCK && 
                   subMenuState == subMenu::SET_MINUTES) {
          dateTime.minute = interim_data; // Put into DateTime struct a minute
          dateTime.second = 0;            // Seconds don't need to setup
          pRTC->setTime(dateTime);        // Set the DateTime struct in a RTC DS3231
          menuState = Menu::SELECTION_MENU;
          subMenuState = subMenu::SET_HOURS;
          interim_data = 1;               // Reset intermediate data

        /* Set Allarm 1 */
        } else if (menuState == Menu::SET_ALLARM && subMenuState == subMenu::SET_HOURS) {
          alarm1.hour = interim_data;
          setAlarm_1(alarm1.hour, alarm1.minute);
          subMenuState = subMenu::SET_MINUTES;
          interim_data = getAlarm_1().minute;
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
          eeprom_busy_wait();
          _delay_ms(10);
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
    case Mode::ALARM: {
      static uint8_t circle_count = 0;
      static bool count_flag = true;
      static uint32_t _tmr = uint32_t(millis());

      if (sensor_btn.press() && !count_flag) modeStatus = Mode::WORK;

      if (checkTime.ready()) displayTime();
      displayTM1637.point(blinkPointsTimer.getStatus());

      if (!count_flag &&
          (millis() - _tmr) > (60000UL * (uint32_t)PAUSE_BETWEEN_ALARM)) {
        count_flag = true;
        _tmr = millis();
      }

      if (count_flag) {
#ifdef DF_MP3_PLAYER
        mp3Player.play(SONG_NUMBER);
#endif /* DF_MP3_PLAYER */
        while (true) {
          func[effectNumber]();

          /* Show time while LED Ring works */
          if (checkTime.ready()) displayTime();
          displayTM1637.point(blinkPointsTimer.getStatus());

          /* Tracing the sensor button click */
          if ((millis() - _tmr) > (60000UL * (uint32_t)ALARM_TIME_OUT)) {
            _tmr = millis();
            ++circle_count;
            count_flag = false;
#ifdef DF_MP3_PLAYER
            mp3Player.stop();
#endif /* DF_MP3_PLAYER */
            break;
          }
          
          if (sensor_btn.press() || circle_count == COUNT_OF_REPEAT) {
            modeStatus = Mode::WORK;
            circle_count = 0;
            break;
          }
        }
      }

      FastLED.clear();
      FastLED.show();
      }

      break; /* End of case ALARM */

/*------------------------------| Mode ERROR |-------------------------------*/
    case Mode::ERROR:
      uint8_t ledPossitionCursor = 3;
      displayTM1637.point(true);
      displayTM1637.displayByte(_E, _r, _0, _empty);

      switch (alarmClockError) {
        case AlarmClockErrors::RTC_I2C_NOT_RESPONSE:
          displayTM1637.display(ledPossitionCursor,
                                (uint8_t)AlarmClockErrors::RTC_I2C_NOT_RESPONSE);
          break;
        
        case AlarmClockErrors::DFPLAYER_SERIAL_ERROR:
          displayTM1637.display(ledPossitionCursor,
                                (uint8_t)AlarmClockErrors::DFPLAYER_SERIAL_ERROR);
          break;
      }

      while (true) {
        left_btn.tick();
        right_btn.tick();
        set_btn.tick();
        cancel_btn.tick();

        if (left_btn.press() || right_btn.press() ||
            set_btn.press() || cancel_btn.press()) {
          modeStatus = Mode::WORK;
          displayTM1637.displayByte(_empty, _empty, _empty, _empty);
          break;
        }
      }

      break; /* End of case ERROR */
  }
}
