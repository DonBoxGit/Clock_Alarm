#ifndef TIMER_BLINK_H
#define TIMER_BLINK_H

#include <Arduino.h>

/*-----------------------------| Class Timer |-------------------------------*/
class Timer {
  public:
    Timer(uint16_t period = 1000);
    ~Timer();
    
    bool ready(void);
    void resetCounter(void);
    void setPeriod (uint16_t period);
    uint16_t getPeriod(void);
    uint32_t getTmr(void);

  private:
    uint32_t _tmr = 0;
    uint16_t _period;
};

/*------------------------------| Class Blink |------------------------------*/
class Blink : public Timer {
  public:
    Blink(uint16_t period = 500);
    ~Blink();

    bool getStatus  (void);
    void resetStatus(void);
    void toggle     (void);
    
  private:
    bool _flag = false;
};

#endif /* TIMER_BLINK_H */
