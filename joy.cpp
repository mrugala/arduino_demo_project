#include "joy.h"
#include "Arduino.h"
#include "project_defines.h"
#include <math.h> 

#if (ARDUINO >= 106 && ARDUINO < 110) || (ARDUINO >= 155)
#pragma message "compiling with digitalPinToInterrupt support"
#else
#pragma message "compiling without digitalPinToInterrupt support"
int digitalPinToInterrupt(int pin)
{
    switch(pin)
    {
#if (ARDUINO < 160) || defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MINI)
        case 2:
            return 0;
        case 3:
            return 1;
#endif
        default:
            return -1;
    }
}
#endif

JoyStick::JoyStick(const unsigned horizontal_pin, const unsigned vertical_pin, const unsigned select_pin)
    : horizontal_pin(horizontal_pin), vertical_pin(vertical_pin), select_pin(select_pin)
{
}

JoyStick::~JoyStick()
{
    detachInterrupt(digitalPinToInterrupt(select_pin));
}

namespace {

#define PREVENT_BOUNCE(x) bool PREVENT_BOUNCE_##x() \
    {\
        delayMicroseconds(50);\
        return digitalRead(x);\
    }

#define IRQ(x) void IRQ_##x()\
    {\
        noInterrupts();\
        if (PREVENT_BOUNCE_##x())\
            select_state_##x = true;\
        interrupts();\
    }

#if (ARDUINO < 160) || defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MINI)
    bool select_state_2 = false;
    bool select_state_3 = false;
#ifdef IRQ2_10PF_ADDED
#pragma message "10 pF condensator added for IRQ2 pin"
#define PREVENT_BOUNCE_2() true
#else
#pragma message "50 uS delay will be used for IRQ2 pin"
    PREVENT_BOUNCE(2)
#endif 
#ifdef IRQ3_10PF_ADDED
#pragma message "10 pF condensator added for IRQ3 pin"
#define PREVENT_BOUNCE_3() true
#else
#pragma message "50 uS delay will be used for IRQ3 pin"
    PREVENT_BOUNCE(3)
#endif 
    IRQ(2) 
    IRQ(3) 
#endif
};

int JoyStick::init()
{
    pinMode(select_pin, INPUT_PULLUP);
    IRQ irq;
    switch (select_pin)
    {
#if (ARDUINO < 160) || defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MINI)
       case 2:
          irq = &IRQ_2;
          select_state = &select_state_2;
          break;
       case 3:
          irq = &IRQ_3;
          select_state = &select_state_3;
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


