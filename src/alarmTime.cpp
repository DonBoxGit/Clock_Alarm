#include "alarmTime.h"

/* Struct Date and Time */
DateTime dateTime;

MicroDS3231 *pDS3231 = new MicroDS3231(RTC_I2C_ADDR);

uint8_t bcd2dec(uint8_t bcd) { return ((bcd / 16) * 10) + (bcd % 16); }
uint8_t dec2bcd(uint8_t dec) { return ((dec / 10) * 16) + (dec % 10); }

uint8_t readRegisterDS3231(uint8_t addr, uint8_t reg) {
    Wire.begin();
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.requestFrom(addr, 1U);
    while (!Wire.available()) {}
    return Wire.read();
    Wire.endTransmission();
}

void writeRegisterDS3231(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire.begin();
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

void setAlarm_1(uint8_t hour, uint8_t minute, uint8_t second) {
  second = dec2bcd(second);
  minute = dec2bcd(minute);
  hour = dec2bcd(hour);
  uint8_t dydw = 0x00;

  second &= 0b01111111;
  minute &= 0b01111111;
  hour &= 0b01111111;
  dydw |= 0b10000000;

  Wire.beginTransmission(RTC_I2C_ADDR);
#if ARDUINO >= 100
  Wire.write(ALARM_1_REGISTER);
  Wire.write(second);
  Wire.write(minute);
  Wire.write(hour);
  Wire.write(dydw);
#else
  Wire.send(ALARM_1_REGISTER);
  Wire.send(second);
  Wire.send(minute);
  Wire.send(hour);
  Wire.send(dydw);
#endif
  Wire.endTransmission();

  uint8_t value = readRegisterDS3231(RTC_I2C_ADDR, STATUS_REGISTER);
  value &= 0b00000001;
  writeRegisterDS3231(RTC_I2C_ADDR, STATUS_REGISTER, value);
}

RTCAlarmTime getAlarm1() {
  uint8_t values[4];
  RTCAlarmTime alarmData;

  Wire.beginTransmission(RTC_I2C_ADDR);
#if ARDUINO >= 100
  Wire.write(ALARM_1_REGISTER);
#else
  Wire.send(ALARM_1_REGISTER);
#endif

  Wire.endTransmission();
  Wire.requestFrom(RTC_I2C_ADDR, 4);

  for (int i = 3; i >= 0; i--) {
#if ARDUINO >= 100
    values[i] = bcd2dec(Wire.read() & 0b01111111);
#else
    values[i] = bcd2dec(Wire.receive() & 0b01111111);
#endif
  }

  alarmData.day    = values[0];
  alarmData.hour   = values[1];
  alarmData.minute = values[2];
  alarmData.second = values[3];

  return alarmData;
}

uint8_t* getTimeDS3231() {
  static uint8_t timeDisplayArray[4];
  uint8_t hour, minute;
  hour = pDS3231->getHours();
  minute = pDS3231->getMinutes();
  timeDisplayArray[0] = hour / 10;
  timeDisplayArray[1] = hour % 10;
  timeDisplayArray[2] = minute / 10;
  timeDisplayArray[3] = minute % 10;
  return timeDisplayArray;
}

