#ifndef _MENU_TREE_H_
#define _MENU_TREE_H_

#include <inttypes.h>
#include "ledDisplay.h"

/* For blink of setting numbers */
extern Blink blinkPointsTimer;
extern Timer checkTime;

enum class Menu : uint8_t {
  SELECTION_MENU = 0,
  SET_CLOCK,
  SET_ALLARM,
  SET_BRIGHTNESS
} menuState;

enum class subMenu: uint8_t {
  SET_HOURS = 0,
  SET_MINUTES
} subMenuState;

void menuPrint(int8_t value) {
  switch (menuState) {
    case Menu::SELECTION_MENU:
    displayTM1637.point(POINT_OFF);
      switch (value) {
        case static_cast<int8_t>(Menu::SET_CLOCK):
          displayTM1637.displayByte(_C, _L, _C, _empty);
          break;
        
        case static_cast<int8_t>(Menu::SET_ALLARM):
          displayTM1637.displayByte(_A, _L, _r, _empty);
          break;
        
        case static_cast<int8_t>(Menu::SET_BRIGHTNESS):
          displayTM1637.displayByte(_B, _r, _t, _empty);
          break;
      }
      break;

    case Menu::SET_CLOCK:
      switch (subMenuState) {
        case subMenu::SET_HOURS:
          if (blinkPointsTimer.getStatus()) {
            displayTM1637.display(0, value / 10);
            displayTM1637.display(1, value % 10);
          } else {
            displayTM1637.displayByte(0, _empty);
            displayTM1637.displayByte(1, _empty);
          }
          displayTM1637.display(2, getTimeRTC()[2]);
          displayTM1637.display(3, getTimeRTC()[3]);
          break;

        case subMenu::SET_MINUTES:
          displayTM1637.display(0,getTimeRTC()[0]);
          displayTM1637.display(1, getTimeRTC()[1]);
          if (blinkPointsTimer.getStatus()) {
            displayTM1637.display(2, value / 10);
            displayTM1637.display(3, value % 10);
          } else {
            displayTM1637.displayByte(2, _empty);
            displayTM1637.displayByte(3, _empty);
          }
          break;
      }
      break;

    case Menu::SET_ALLARM:
      switch (subMenuState) {
        case subMenu::SET_HOURS:
          if (blinkPointsTimer.getStatus()) {
            displayTM1637.display(0, value / 10);
            displayTM1637.display(1, value % 10);
          } else {
            displayTM1637.displayByte(0, _empty);
            displayTM1637.displayByte(1, _empty);
          }
          displayTM1637.display(2, getAlarm1().minute / 10);
          displayTM1637.display(3, getAlarm1().minute % 10);
          break;
        
        case subMenu::SET_MINUTES:
          displayTM1637.display(0, getAlarm1().hour / 10);
          displayTM1637.display(1, getAlarm1().hour % 10);
          if (blinkPointsTimer.getStatus()) {
            displayTM1637.display(2, value / 10);
            displayTM1637.display(3, value % 10);
          } else {
            displayTM1637.displayByte(2, _empty);
            displayTM1637.displayByte(3, _empty);
          }
          break;
      }

      break;

    case Menu::SET_BRIGHTNESS:
      displayTM1637.display(0, 0);
      displayTM1637.display(1, value);
    break;
    }
}

#endif /* _MENU_TREE_H_ */