#include "joy.h"
#include "project_defines.h"
#include "IRQ.h"
#include <math.h> 

JoyStick::JoyStick(const unsigned horizontal_pin, const unsigned vertical_pin, const unsigned select_pin)
    : horizontal_pin(horizontal_pin), vertical_pin(vertical_pin), select_pin(select_pin)
{
}

JoyStick::~JoyStick()
{
    detachInterrupt(digitalPinToInterrupt(select_pin));
}

int JoyStick::init()
{
    pinMode(select_pin, INPUT_PULLUP);
    IRQ irq;
    switch (select_pin)
    {
#if (ARDUINO < 160) || defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MINI)
       case 2:
          irq = &IRQ_2;
          select_state = &irq_flag_2;
          break;
       case 3:
          irq = &IRQ_3;
          select_state = &irq_flag_3;
          break;
#endif
       default:
          return -1;
    }
    
    int interrupt = digitalPinToInterrupt(select_pin);
    if (interrupt < 0)
        return interrupt;
    attachInterrupt(interrupt, irq, RISING);

    return 0;
}

double JoyStick::getHorizontalDeviation()
{
    return (static_cast<int>(read(horizontal_pin)) - 512) / 5.12;
}

double JoyStick::getVerticalDeviation()
{
    return (static_cast<int>(read(vertical_pin)) - 512) / 5.12;
}

double JoyStick::getAngle_deg()
{
    double hor = getHorizontalDeviation();
    double dev = getVerticalDeviation();
    
    if (hor < 5 && dev < 5)
        return 0.;
    
    return atan2(hor, -dev) / 3.14 * 180;
}

unsigned JoyStick::read(const unsigned pin)
{
    return analogRead(pin);
}

bool JoyStick::testSelect()
{
    bool ret = *select_state;
    *select_state = false;
    return ret;
}


