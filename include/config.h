#ifndef _CONFIG_H_
#define _CONFIG_H_

/*---------------------| Alarm clock device settings |-----------------------*/
#define DOTS_TIMER          500     // Период мигания точек в миллисекундах
#define NUMBERS_TIMER       1000    // Период обновления цифр часов - 1 секунда
#define EFFECT_SHOW_TIMER   5000    // Время демонстрации эффектов LED кольца
#define WS_ALGORITHM_SPEED  10      // Задержа в алгоритме эффекта

/*-------------------------| RTC DS3231 settings |---------------------------*/
#define RTC_I2C_ADDR        0x68    // Адрес RTC на шаине I2C
#define ALARM_1_REGISTER    0x07    // Регистр будильника 1
#define ALARM_2_REGISTER    0x0B    // Регистр будильника 2
#define CONTROL_REGISTER    0x0E    // Регистр котроля DS3231
#define STATUS_REGISTER     0x0F    // Регистр статуса DS3231

/* Биты регистра контроля */
#define A1IE_BIT      0     // Вкл прерывание будильником 1
#define A2IE_BIT      1     // Вкл прерывание будильником 2
#define INTCN_BIT     2     // INTerrupt CoNtrol. 0 - SQW(by Meandr), 1 -INT(by Alarm).
#define RS1_BIT       3     // Выбор часторы меандра
#define RS2_BIT       4     // RS1 = 0 and RS2 = 0 , SQW -> 1kHz.
/* Биты статусного регистра  */
#define A1F_BIT       0     // Флаг прерывания будильника 1
#define A2F_BIT       1     // Флаг прерывания будильника 2

/*----------------| Addresses of values in EEPROM memory |-------------------*/
#define TM1637_BRIGHTNESS_ADDR       0x00   // Адрес в EEPROM значения яркости
#define DFPLAYER_VOLUME_VALUE_ADDR   0x01   // Адрес в EEPROM значения громкости
#define WS_EFFECT_NUMBER_ADDR        0x02   // Адрес в EEPROM номера эффекта

/*------------------------| DFPlayer Mini settings |-------------------------*/
#define DFPLAYER_RX_PIN         3   // RX пин плеера
#define DFPLAYER_TX_PIN         4   // TX пин плеера
#define DFPLAYER_MIN_VOLUME     0   // Минимальная громкость (звука нет)
#define DFPLAYER_MAX_VOLUME     30  // Максимальня громкость

/*-----------------------| TM1637 Display settings |-------------------------*/
#define TM1637_CLK_PIN         5    // Пин CLK дисплея
#define TM1637_DIO_PIN         6    // Пин DIO дисплея
#define TM1637_MIN_BRIGHTNESS  0    // Минимальная яркость дисплея
#define TM1637_MAX_BRIGHTNESS  7    // Максимальня яркость дисплея

/*----------------------| WS2812B LED Ring settings |------------------------*/
#define WS2812_DI_PIN         8
#define WS2812_LED_NUM        16
#define WS2812_BRIGHTNESS     40
#define COLOR_ORDER           GRB
#define MAX_POWER_MILLIAMPS   500
#define LED_TYPE              WS2812B
#define QUANTITY_EFFECTS      6

/*------------------| Button and sensor connection pins |--------------------*/
#define LEFT_BUTTON_PIN     9
#define RIGHT_BUTTON_PIN    10
#define SET_BUTTON_PIN      11
#define CANCEL_BUTTON_PIN   12
#define SENSOR_MODULE_PIN   7
#define ISR_INPUT_PIN       2

#endif /* _CONFIG_H_ */