/************************************************************************
 *                      Clock Alarm Mp3Player                           *
 *                            ver. 1.0.0                                *
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

/* Initialization of buttons for control */
EncButton<EB_TICK, LEFT_BUTTON_PIN>   left_btn   (INPUT_PULLUP);
EncButton<EB_TICK, RIGHT_BUTTON_PIN>  right_btn  (INPUT_PULLUP);
EncButton<EB_TICK, SET_BUTTON_PIN>    set_btn    (INPUT_PULLUP);
EncButton<EB_TICK, CANCEL_BUTTON_PIN> cancel_btn (INPUT_PULLUP);
SensorButton sensor_btn(SENSOR_MODULE_PIN);

/* Create softSerial and mp3player objects */
SoftwareSerial softSerial(DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
DFRobotDFPlayerMini mp3Player;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(60000);
  softSerial.begin(9600);

  /* Read the data from EEPROM memory */
  EEPROM.get(TM1637_BRIGHTNESS_ADDR, ledBrightnessCounter);
  EEPROM.get(DFPLAYER_VOLUME_VALUE_ADDR, mp3Volume);
  EEPROM.get(WS_EFFECT_NUMBER_ADDR, effectNubmer);

  /* Checking data and if they is incorrect then to change */
  if (!checkLedBrightness(ledBrightnessCounter)) {
    EEPROM.put(TM1637_BRIGHTNESS_ADDR, ledBrightnessCounter);
    eeprom_busy_wait();
    _delay_ms(10);
  }

  /* Setup TM1637 display */
  displayTM1637.clear();
  displayTM1637.brightness(ledBrightnessCounter);

  /* Setup LED ring settings */
  FastLED.addLeds<WS2812, WS2812_DI_PIN, COLOR_ORDER>(leds, WS2812_LED_NUM);
  FastLED.setBrightness(WS2812_BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  uint8_t value = readRegisterDS3231(RTC_I2C_ADDR, CONTROL_REGISTER);
  if (value != 0x00) value = 0x00;

  /* Allow interrupt to INT/SQW by Alarm 1 */
  value |= (1 << A1IE_BIT) | (1 << INTCN_BIT);
  writeRegisterDS3231(RTC_I2C_ADDR, CONTROL_REGISTER, value);

  // value = readRegisterDS3231(RTC_I2C_ADDR, CONTROL_REGISTER);
  alarm1 = getAlarm1();

  /* Allowing an external interrupt on the SQW signal */
  pinMode(ISR_INPUT_PIN, INPUT_PULLUP); // Input needs to pull up to VCC
  attachInterrupt(0, ISR_RTC_INT, FALLING);  // INT0 attached

  /* Checking the equipment for the response */
  if (!mp3Player.begin(softSerial)) {
    modeStatus = Mode::ERROR;
    alarmClockError = AlarmClockErrors::DFPLAYER_SERIAL_ERROR;
  } else if (!pDS3231->begin()) {
    modeStatus = Mode::ERROR;
    alarmClockError = AlarmClockErrors::RTC_I2C_NOT_RESPONSE;
  } else {
    dateTime = pDS3231->getTime();
    modeStatus = Mode::WORK;
    subMenuState = subMenu::SET_HOURS;
  }

  /* Constrain and set volume of mp3 player */
  mp3Volume = constrain(mp3Volume, DFPLAYER_MIN_VOLUME, DFPLAYER_MAX_VOLUME);
  mp3Player.volume(mp3Volume);
  mp3Player.EQ(DFPLAYER_EQ_NORMAL);
  mp3Player.outputDevice(DFPLAYER_DEVICE_SD);

  /* If the battety too is low level and the power is lost */
  if (pDS3231->lostPower()) {
    pDS3231->setTime(COMPILE_TIME);
    modeStatus = Mode::EDIT;
    menuState = Menu::SET_CLOCK;
    subMenuState = subMenu::SET_HOURS;
    displayTM1637.point(true);
    interim_data = pDS3231->getHours();
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
          mp3Player.play(3);
          mp3FlagPlayer = true;
          displayTM1637.displayByte(_empty, _empty, _empty, _empty);
          displayTM1637.point(false);
          displayTM1637.display(0, mp3Volume / 10);
          displayTM1637.display(1, mp3Volume % 10);
        } else {
          mp3Player.stop();
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
          func[effectNubmer]();

          /* Show time while LED Ring works */
          if (checkTime.ready()) displayTime();
          displayTM1637.point(blinkPointsTimer.getStatus());

          /* Tracing the sensor button click */
          set_btn.tick();
          if (sensor_btn.press()) {
            if (++effectNubmer > (QUANTITY_EFFECTS - 1)) effectNubmer = 0;
            ringEffectShowTimer.resetCounter();
          }
        }
        
        FastLED.clear();
        FastLED.show();
        ledRingflag = false;
        EEPROM.put(WS_EFFECT_NUMBER_ADDR, effectNubmer);
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
          mp3Player.volume(mp3Volume);
          buttonsVolumeFlag = true;
          displayTM1637.displayByte(_empty, _empty, _empty, _empty);
        }

        if (right_btn.press()) {
          if (++mp3Volume > DFPLAYER_MAX_VOLUME) mp3Volume = DFPLAYER_MAX_VOLUME;
          mp3Player.volume(mp3Volume);
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
          mp3Player.stop();
          mp3FlagPlayer = false;
        }
        displayTime();
      }

      if (!mp3FlagPlayer) {
        if (checkTime.ready()) displayTime();
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
          interim_data = 1;               // Reset intermediate data

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
      displayTime();

       /* Reset the millis counter */
      extern volatile unsigned long timer0_millis;
      extern volatile unsigned long  timer0_overflow_count;
      cli();
      timer0_millis = 0;
      timer0_overflow_count = 0;
      sei();

      /* Allocate memmory for variable */
      uint32_t* _tmr = new uint32_t(millis());

      mp3Player.play(3);

      while (true) {
        func[effectNubmer]();

        /* Show time while LED Ring works */
        if (checkTime.ready()) displayTime();
        displayTM1637.point(blinkPointsTimer.getStatus());

        /* Tracing the sensor button click */
        if (sensor_btn.press() ||
            (millis() - *_tmr) > (1000UL * 60UL * (uint32_t)alarmTimeOut)) {
          modeStatus = Mode::WORK;
          mp3Player.stop();
          break;
        }
      }

      delete _tmr;
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

      while (true) {}
      break; /* End of case ERROR */
  }
}
