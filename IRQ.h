#pragma once

typedef void (*IRQ)(void);

#if (ARDUINO >= 106 && ARDUINO < 110) || (ARDUINO >= 155)
#pragma message "compiling with digitalPinToInterrupt support"
#else
#pragma message "compiling without digitalPinToInterrupt support"
inline int digitalPinToInterrupt(int pin)
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

#define IRQ(x) void IRQ_##x()

#if (ARDUINO < 160) || defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MINI)
extern bool irq_flag_2;
extern bool irq_flag_3;

IRQ(2);
IRQ(3);
#endif
