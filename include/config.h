#ifndef _CONFIG_H_
#define _CONFIG_H_

#define DEBUG_ALARM_CLOCK
//define DF_MP3_PLAYER /* Раскомментировать если используется MP3 плеер */
//#define RTC_DS3231   /* Раскомментировать если используется RTC DS3231 */
#define RTC_DS1307     /* Раскомментировать если используется RTC DS1307 */

/*------------------------| Настройки будильника |---------------------------*/
#define ALARM_TIME_OUT       1       // Длительность сигнализации будильника в мин.
#define PAUSE_BETWEEN_ALARM  1       // Пауза между сигналами будильника в мин.
#define COUNT_OF_REPEAT      3       // Количество повторений сигнала будильника
#define SONG_NUMBER          1       // Номер трека на SD-карте
#define DOTS_TIMER           500     // Период мигания точек в миллисекундах
#define NUMBERS_TIMER        1000    // Период обновления цифр часов - 1 секунда
#define EFFECT_SHOW_TIMER    5000    // Время демонстрации эффектов LED кольца 5 сек.
#define WS_ALGORITHM_SPEED   10      // Задержа в алгоритме эффектов 10 миллисек.

/*------------------------| Настройки RTC DS3231 |---------------------------*/
#ifdef RTC_DS3231
  #define RTC_I2C_ADDR        0x68    // Адрес RTC на шаине I2C
  #define ALARM_1_REGISTER    0x07    // Регистр будильника 1
  #define ALARM_2_REGISTER    0x0B    // Регистр будильника 2
  #define CONTROL_REGISTER    0x0E    // Регистр котроля DS3231
  #define STATUS_REGISTER     0x0F    // Регистр статуса DS3231

  /* Биты регистра контроля RTC DS3231 */
  #define A1IE_BIT      0     // Вкл прерывание будильником 1
  #define A2IE_BIT      1     // Вкл прерывание будильником 2
  #define INTCN_BIT     2     // INTerrupt CoNtrol. 0 - SQW(by Meandr), 1 -INT(by Alarm)
  #define RS1_BIT       3     // RS1 и  RS2биты отвечающие за частоту меандра
  #define RS2_BIT       4     // RS1 = 0 and RS2 = 0 , SQW -> 1kHz
  /* Биты статусного регистра RTC DS3231 */
  #define A1F_BIT       0     // Флаг прерывания будильника 1
  #define A2F_BIT       1     // Флаг прерывания будильника 2
#endif /* RTC_DS3231 */

/*-------------------| Адреса значений в EEPROM памяти |---------------------*/
#define TM1637_BRIGHTNESS_ADDR       0x00   // Адрес в EEPROM значения яркости
#define DFPLAYER_VOLUME_VALUE_ADDR   0x01   // Адрес в EEPROM значения громкости
#define WS_EFFECT_NUMBER_ADDR        0x02   // Адрес в EEPROM номера эффекта LED ring
#ifdef RTC_DS1307
  #define RTC_ALARM_SECONDS          0x03   // Адрес в EEPROM - секунды будильника
  #define RTC_ALARM_MINUTES          0x04   // Адрес в EEPROM - минуты будильника
  #define RTC_ALARM_HOURS            0x05   // Адрес в EEPROM - часы будильника
  #define RTC_ALARM_DAY              0x06   // Адрес в EEPROM - день будильника
#endif /* RTC_DS1307 */

/*--------------------------| Настройки DFPlayer |---------------------------*/
#define DFPLAYER_RX_PIN         3   // RX пин плеера
#define DFPLAYER_TX_PIN         4   // TX пин плеера
#define DFPLAYER_MIN_VOLUME     0   // Минимальная громкость (звука нет)
#define DFPLAYER_MAX_VOLUME     30  // Максимальня громкость

/*---------------------| Настройки индикатора TM1637 |-----------------------*/
#define TM1637_CLK_PIN         5    // Пин CLK дисплея
#define TM1637_DIO_PIN         6    // Пин DIO дисплея
#define TM1637_MIN_BRIGHTNESS  0    // Минимальная яркость дисплея
#define TM1637_MAX_BRIGHTNESS  7    // Максимальня яркость дисплея

/*---------------|Настройки светодиодного кольца  WS2812B |------------------*/
#define WS2812_DI_PIN         9         // Пин DI LED кольца
#define WS2812_LED_NUM        16        // Количество диодов LED кольца
#define WS2812_BRIGHTNESS     40        // Яркость(общая) LED кольца
#define COLOR_ORDER           GRB       // Цветовая схема
#define MAX_POWER_MILLIAMPS   500       // Ограничение по току в mA
#define LED_TYPE              WS2812B   // Тип светодиодной ленты
#define QUANTITY_EFFECTS      6         // Количество эффектов

/*------------------------| Пины кнопок и сенсоров |-------------------------*/
#define LEFT_BUTTON_PIN     10      // Пин кнопки Влево
#define RIGHT_BUTTON_PIN    11      // Пин кнопки Вправо
#define SET_BUTTON_PIN      12      // Пин Кнопки Установить
#define CANCEL_BUTTON_PIN   8       // Пин Кнопки Отмена
#define SENSOR_MODULE_PIN   7       // Пин сенсорного модуля
#ifdef RTC_DS3231
  #define ISR_INPUT_PIN     2       // Пин входа прерывания(от RTC)
#endif /* RTC_DS3231 */
#define FR1_PIN             A0      // Пин фото резистора
#define POWER_SENSOR_PIN    A1      // Пин сенсора наличия питания(батарея/БП)

#endif /* _CONFIG_H_ */