#ifndef _ALARM_TIME_H_
#define _ALARM_TIME_H_

#include <microDS3231.h>
#include "config.h"

struct RTCAlarmTime {
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

/* ponter's declaration of MicroDS3231 object */
extern MicroDS3231 *pDS3231;

/* Convert Binary to Decimal */
uint8_t bcd2dec(uint8_t bcd);

/* Conver Decimal to Binary */
uint8_t dec2bcd(uint8_t dec);

uint8_t readRegisterDS3231(uint8_t addr, uint8_t reg);
void writeRegisterDS3231(uint8_t addr, uint8_t reg, uint8_t value);

/* Return the pointer Array uint8_t data of time.
First two digits is hours and second two digits is minutes. */
uint8_t* getTimeDS3231(void);

/* Set and get alarm 1 hours and minutes */
void setAlarm_1(uint8_t hour, uint8_t minute, uint8_t second = 0);
RTCAlarmTime getAlarm1(void);

#endif /* _ALARM_TIME_H_ */