#ifndef _WS_2812_EFFECTS_H_
#define _WS_2812_EFFECTS_H_

#include <FastLED.h>
#include "config.h"
#include "timer_blink.h"

uint8_t ws2812_counter;
static uint8_t base = 0;
extern  Timer ws2812Timer;

/* Initialization of LED ring */
CRGB leds[WS2812_LED_NUM];

/*------------------------------| Effect - 1 |-------------------------------*/
void ws2812_raibow(void) {
  for (int i = 0; i < WS2812_LED_NUM; i++) {
  leds[i].setHue(ws2812_counter + i * 255 / WS2812_LED_NUM);
  }
  ws2812_counter++;
  FastLED.show();
}

/*------------------------------| Effect - 2 |-------------------------------*/
void ws2812_confetti(void) {
  fadeToBlackBy(leds, WS2812_LED_NUM, 2);
  int pos = random16(WS2812_LED_NUM);
  leds[pos] += CHSV(base++ + random8(64), 200, 255);
  FastLED.show();
}

/*------------------------------| Effect - 3 |-------------------------------*/
void ws2812_runningFire(void) {
  fadeToBlackBy(leds, WS2812_LED_NUM, 2);
  int pos = beatsin16(13, 0, WS2812_LED_NUM - 1);
  leds[pos] += CHSV(base++, 255, 192);
  FastLED.show();
}

/*------------------------------| Effect - 4 |-------------------------------*/
void ws2812_cyclone(void) {
  while (!ws2812Timer.ready()) {
  for (int i = 0; i < WS2812_LED_NUM; i++) {
      leds[i].nscale8(250);
      }
    for (int i = 0; i < WS2812_LED_NUM; i++) {
      leds[i] = CHSV(base++, 255, 255);
      FastLED.show();
    }
  }
}

/*------------------------------| Effect - 5 |-------------------------------*/
void ws2812_focus(void) {
  while (!ws2812Timer.ready()) {
    fadeToBlackBy(leds, WS2812_LED_NUM, 2);
    for (int i = 0; i < 8; i++) {
      leds[beatsin16(i + 7, 0, WS2812_LED_NUM - 1)] |= CHSV(base+=16, 200, 255);
    }
    FastLED.show();
  }
}

/*------------------------------| Effect - 6 |-------------------------------*/
void ws2812_rainbow2(void) {
  while (!ws2812Timer.ready()) {
    fill_rainbow( leds, WS2812_LED_NUM, base++, 7);
    if (random8() < 80) leds[random16(WS2812_LED_NUM)] += CRGB::White;
    FastLED.show();
  }
}

/*-------------------------| The array of effects |--------------------------*/
void (*func[6])(void) = {
  ws2812_raibow,
  ws2812_confetti,
  ws2812_runningFire,
  ws2812_cyclone,
  ws2812_focus,
  ws2812_rainbow2
};

#endif /* _WS_2812_EFFECTS_H_ */