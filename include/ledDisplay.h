#ifndef _LED_DISPLAY_H_
#define _LED_DISPLAY_H_

#include <GyverTM1637.h>
#include "alarmTime.h"

GyverTM1637 displayTM1637(TM1637_CLK_PIN, TM1637_DIO_PIN);
uint32_t Now, clocktimer;
boolean flag;

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

void displayTime(void) {
  displayTM1637.display(getTimeDS3231()[0], getTimeDS3231()[1],
                        getTimeDS3231()[2], getTimeDS3231()[3]);
}

void twists() {
  // скручивание массив ЦИФР
  byte digs[4] = {3, 5, 7, 1};
  displayTM1637.twist(digs, 50);     // скорость прокрутки 100
  delay(1000);

  // скручивание прицельно (ячейка, БАЙТ, скорость)
  displayTM1637.twistByte(0, _1, 50);
  delay(1000);

  // скручивание прицельно (ячейка, ЦИФРА, скорость)
  displayTM1637.twist(0, 8, 70);
  delay(1000);

  displayTM1637.clear();
  delay(200);
  for (byte i = 0; i < 10; i++) {
    displayTM1637.twist(3, i, 20);
    delay(200);
  }

  // скручивание массива БАЙТ
  byte troll[4] = {_t, _r, _o, _l};
  displayTM1637.twistByte(troll, 50);
  delay(1000);

  // прицельное скручивание БАЙТА (ячейка, байт, скорость)
  displayTM1637.twistByte(2, _G, 50);
  delay(1000);
}

void twistClock() {
  byte hrs = 21, mins = 55;
  uint32_t tmr;
  Now = millis();
  while (millis () - Now < 10000) {   // каждые 10 секунд
    if (millis() - tmr > 500) {       // каждые полсекунды
      tmr = millis();
      flag = !flag;
      displayTM1637.point(flag);   // выкл/выкл точки

      if (flag) {
        // ***** часы! ****
        mins ++;
        if (mins > 59) {
          mins = 0;
          hrs++;
          if (hrs > 24) hrs = 0;
        }
        // ***** часы! ****
        displayTM1637.displayClockTwist(hrs, mins, 35);    // выводим время
      }
    }
  }
  displayTM1637.point(0);   // выкл точки
}

void scrolls() {
  // прокрутка массив ЦИФР
  byte digs[4] = {3, 5, 7, 1};
  displayTM1637.scroll(digs, 100);     // скорость прокрутки 100
  delay(1000);

  // прокрутка прицельно (ячейка, ЦИФРА, скорость)
  displayTM1637.scroll(0, 8, 200);
  delay(1000);

  displayTM1637.clear();
  delay(1000);
  for (byte i = 0; i < 10; i++) {
    displayTM1637.scroll(3, i, 50);
    delay(400);
  }

  // прокрутка массива БАЙТ
  byte troll[4] = {_t, _r, _o, _l};
  displayTM1637.scrollByte(troll, 100);
  delay(1000);

  // прицельная прокрутка БАЙТА (ячейка, байт, скорость)
  displayTM1637.scrollByte(2, _G, 50);
  delay(1000);
}

void bytes() {
  // выводим байты из массива
  byte troll[4] = {_t, _r, _o, _l};
  displayTM1637.displayByte(troll);
  delay(1000);

  // выводим байты напрямую (4 в скобках)
  displayTM1637.displayByte(_L, _O, _L, _empty);
  delay(1000);

  // выводим байты "прицельно"
  displayTM1637.displayByte(3, _O);    // 3 ячейка, буква О
  delay(1000);

  // выводим цифры из массива
  byte hell[4] = {6, 6, 6, 6};
  displayTM1637.display(hell);
  delay(1000);

  // выводим цифры напрямую (4 в скобках)
  displayTM1637.display(1, 2, 3, 4);
  delay(1000);

  // выводим цифры "прицельно"
  displayTM1637.display(0, 9);    // 0 ячейка, цифра 9
  delay(1000);
}

void fadeBlink() {
  // пишем HELL
  displayTM1637.displayByte(_H, _E, _L, _L);

  Now = millis();
  while (millis () - Now < 3000) {    // 3 секунды
    for (int i = 7; i > 0; i--) {
      displayTM1637.brightness(i);   // меняем яркость
      delay(40);
    }
    for (int i = 0; i < 8; i++) {
      displayTM1637.brightness(i);   // меняем яркость
      delay(40);
    }
  }
}

void scrollClock() {
  byte hrs = 15, mins = 0;
  uint32_t tmr;
  Now = millis();
  while (millis () - Now < 10000) {   // каждые 10 секунд
    if (millis() - tmr > 500) {       // каждые полсекунды
      tmr = millis();
      flag = !flag;
      displayTM1637.point(flag);   // выкл/выкл точки

      if (flag) {
        // ***** часы! ****
        mins ++;
        if (mins > 59) {
          mins = 0;
          hrs++;
          if (hrs > 24) hrs = 0;
        }
        // ***** часы! ****
        displayTM1637.displayClockScroll(hrs, mins, 70);    // выводим время
      }
    }
  }
  displayTM1637.point(0);   // выкл точки
}

void normClock() {
  byte hrs = 15, mins = 0;
  uint32_t tmr;
  Now = millis();
  while (millis () - Now < 10000) {   // каждые 10 секунд
    if (millis() - tmr > 500) {       // каждые полсекунды
      tmr = millis();
      flag = !flag;
      displayTM1637.point(flag);   // выкл/выкл точки

      // ***** часы! ****
      mins ++;
      if (mins > 59) {
        mins = 0;
        hrs++;
        if (hrs > 24) hrs = 0;
      }
      // ***** часы! ****
      displayTM1637.displayClock(hrs, mins);   // выводим время функцией часов
    }
  }
  displayTM1637.point(0);   // выкл точки
}

void ints() {
  // тупо отправляем цифры
  displayTM1637.displayInt(-999);
  delay(500);
  displayTM1637.displayInt(-99);
  delay(500);
  displayTM1637.displayInt(-9);
  delay(500);
  displayTM1637.displayInt(0);
  delay(500);
  displayTM1637.displayInt(6);
  delay(500);
  displayTM1637.displayInt(66);
  delay(500);
  displayTM1637.displayInt(666);
  delay(500);
  displayTM1637.displayInt(6666);
  delay(500);
}

void runningText() {
  byte welcome_banner[] = {_H, _E, _L, _L, _O, _empty, _empty,
                           _e, _n, _j, _o, _y, _empty, _empty,
                           _1, _6, _3, _7, _empty, _d, _i, _S, _P, _l, _a, _y
                          };
  displayTM1637.runningString(welcome_banner, sizeof(welcome_banner), 200);  // 200 это время в миллисекундах!
}

#endif /* _LED_DISPLAY_ */