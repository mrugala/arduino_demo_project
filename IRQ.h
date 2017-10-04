#pragma once

#include "Arduino.h"

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

#define PREVENT_BOUNCE(x) bool PREVENT_BOUNCE_##x() \
    {\
        delayMicroseconds(50);\
        return digitalRead(x);\
    }

#define IRQ(x) void IRQ_##x()\
    {\
        noInterrupts();\
        if (PREVENT_BOUNCE_##x())\
            irq_flag_##x = true;\
        interrupts();\
    }

#if (ARDUINO < 160) || defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MINI)
    bool irq_flag_2 = false;
    bool irq_flag_3 = false;
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

