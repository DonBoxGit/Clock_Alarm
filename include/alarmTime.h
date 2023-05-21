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

uint8_t bcd2dec(uint8_t bcd);
uint8_t dec2bcd(uint8_t dec);

extern MicroDS3231 *pDS3231;

uint8_t readRegisterDS3231(uint8_t addr, uint8_t reg);
void writeRegisterDS3231(uint8_t, uint8_t, uint8_t);
void setAlarm_1(uint8_t hour, uint8_t minute, uint8_t second);
RTCAlarmTime getAlarm1(void);

#endif /* _ALARM_TIME_H_ */