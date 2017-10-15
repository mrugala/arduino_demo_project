#include "IRQ.h"
#include "project_defines.h"

#define PREVENT_BOUNCE(x) inline bool PREVENT_BOUNCE_##x() \
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

#define IRQ_FLAG(x) bool IRQ_FLAG_NAME(x) = false

#if (ARDUINO < 160) || defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MINI)
#pragma message "compiling IRQs for Arduino"
IRQ_FLAG(2);
IRQ_FLAG(3);
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
#elif defined(ARDUINO_ESP8266_WEMOS_D1MINI)
#pragma message "compiling IRQs for Wemos D1 Mini"
//IRQ_FLAG(16); //D0 doesn't have interrupt support
IRQ_FLAG(5);  //D1
IRQ_FLAG(4);  //D2
IRQ_FLAG(0);  //D3
IRQ_FLAG(2);  //D4
IRQ_FLAG(14); //D5
IRQ_FLAG(12); //D6
IRQ_FLAG(13); //D7
IRQ_FLAG(15); //D8

//Wemos will not use any real buttons, so no need to add any delay to interrupt check
//it will most definitely be used only to check if the light switch is ON or OFF 
//#define PREVENT_BOUNCE_16() true 
#define PREVENT_BOUNCE_5() true
#define PREVENT_BOUNCE_4() true
#define PREVENT_BOUNCE_0() true
#define PREVENT_BOUNCE_2() true
#define PREVENT_BOUNCE_14() true
#define PREVENT_BOUNCE_12() true
#define PREVENT_BOUNCE_13() true
#define PREVENT_BOUNCE_15() true

//IRQ(16)
IRQ(5)
IRQ(4)
IRQ(0)
IRQ(2)
IRQ(14)
IRQ(12)
IRQ(13)
IRQ(15)
#endif

