#ifndef _CONFIG_H_
#define _CONFIG_H_

/*-------------------------| RTC DS3231 settings |---------------------------*/
#define RTC_I2C_ADDR        0x68
#define ALARM_1_REGISTER    0x07
#define ALARM_2_REGISTER    0x0B
#define CONTROL_REGISTER    0x0E
#define STATUS_REGISTER     0x0F

/* Bits of Control register */
#define A1IE_BIT      0     // Alarm 1 Interrupt Enable.
#define A2IE_BIT      1     // Alarm 2 Interrupt Enable.
#define INTCN_BIT     2     // INTerrupt CoNtrol. 0 - SQW(by Meandr), 1 -INT(by Alarm).
#define RS1_BIT       3     // Rate Select. Controls the frequency of the meandr.
#define RS2_BIT       4     // RS1 = 0 and RS2 = 0 , SQW -> 1kHz.
/* Bits of Status register  */
#define A1F_BIT       0     // Alarm 1 Interrupt Flag.
#define A2F_BIT       1     // Alarm 2 Interrupt Flag.

/*----------------| Addresses of values in EEPROM memory |-------------------*/
#define TM1637_BRIGHTNESS_ADDR       0x00
#define DFPLAYER_VOLUME_VALUE_ADDR   0x01

/*------------------------| DFPlayer Mini settings |-------------------------*/
#define DFPLAYER_RX_PIN         3
#define DFPLAYER_TX_PIN         4
#define DFPLAYER_MIN_VOLUME     0
#define DFPLAYER_MAX_VOLUME     30

/*-----------------------| TM1637 Display settings |-------------------------*/
#define TM1637_CLK_PIN         5
#define TM1637_DIO_PIN         6
#define TM1637_MIN_BRIGHTNESS  0
#define TM1637_MAX_BRIGHTNESS  7

/*----------------------| WS2812B LED Ring settings |------------------------*/
#define WS2812_DI_PIN         8
#define WS2812_LED_NUM        16
#define WS2812_BRIGHTNESS     40
#define COLOR_ORDER           GRB
#define MAX_POWER_MILLIAMPS   500
#define LED_TYPE              WS2812B

/*------------------| Button and sensor connection pins |--------------------*/
#define LEFT_BUTTON_PIN     9
#define RIGHT_BUTTON_PIN    10
#define SET_BUTTON_PIN      11
#define CANCEL_BUTTON_PIN   12
#define SENSOR_MODULE_PIN   7
#define ISR_INPUT_PIN       2

#endif /* _CONFIG_H_ */