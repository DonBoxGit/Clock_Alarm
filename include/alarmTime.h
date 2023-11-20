#ifndef _ALARM_TIME_H_
#define _ALARM_TIME_H_

#include "config.h"

#ifdef RTC_DS3231
  #include <microDS3231.h>
#endif /* RTC_DS3231 */

#ifdef RTC_DS1307
  #include <GyverTM1637.h>
#endif /* RTC_DS1307 */

struct RTCAlarmTime {
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

#ifdef RTC_DS1307
  DS1307 pRTC;
#endif /* RTC_DS1307 */

#ifdef RTC_DS3231
  /* ponter's declaration of MicroDS3231 object */
  extern MicroDS3231* pRTC;

  /* Convert Binary to Decimal */
  uint8_t bcd2dec(uint8_t bcd);

  /* Conver Decimal to Binary */
  uint8_t dec2bcd(uint8_t dec);

  uint8_t readRegisterDS3231(uint8_t addr, uint8_t reg);
  void writeRegisterDS3231(uint8_t addr, uint8_t reg, uint8_t value);
  
  /* Set and get alarm 1 hours and minutes */
  void setAlarm_1(uint8_t hour, uint8_t minute, uint8_t second = 0);
  RTCAlarmTime getAlarm1(void);
#endif /* RTC_DS3231 */

/* Return the pointer Array uint8_t data of time.
First two digits is hours and second two digits is minutes. */
uint8_t* getTimeRTC(void);

#ifdef RTC_DS1307
  /* get Alarm function */
  /* set Alarm function */
#endif /* RTC_DS1307 */

#endif /* _ALARM_TIME_H_ */