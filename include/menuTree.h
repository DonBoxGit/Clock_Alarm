#ifndef _MENU_TREE_H_
#define _MENU_TREE_H_

#include <inttypes.h>
#include "ledDisplay.h"

enum class Menu : uint8_t {
  SET_CLOCK = 0,
  SET_ALLARM,
  SET_BRIGHTNESS
} menuStatus;

void menuPrint() {
    switch (menuStatus) {
        case Menu::SET_CLOCK:
            displayTM1637.clear();
            uint8_t data[4] = { _S, _t, _C, _l };
            displayTM1637.displayByte(data);
            break;

        case Menu::SET_ALLARM:
        break;

        case Menu::SET_BRIGHTNESS:
        break;
      }
}

#endif /* _MENU_TREE_H_ */