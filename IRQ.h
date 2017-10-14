#pragma once

typedef void (*IRQ)(void);

#define IRQ_FLAG_NAME(x) irq_flag_##x

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

#if (ARDUINO < 160) && defined(ARDUINO_ESP8266_WEMOS_D1MINI)
#pragma error "for Wemos D1 Mini have to use newer Arduino IDE version"
#endif

#define IRQ(x) void IRQ_##x()

#if (ARDUINO < 160) || defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MINI)
extern bool IRQ_FLAG_NAME(2);
extern bool IRQ_FLAG_NAME(3);

IRQ(2);
IRQ(3);
#elif defined(ARDUINO_ESP8266_WEMOS_D1MINI)
extern bool IRQ_FLAG_NAME(16); //D0
extern bool IRQ_FLAG_NAME(5);  //D1
extern bool IRQ_FLAG_NAME(4);  //D2
extern bool IRQ_FLAG_NAME(0);  //D3
extern bool IRQ_FLAG_NAME(2);  //D4
extern bool IRQ_FLAG_NAME(14); //D5
extern bool IRQ_FLAG_NAME(12); //D6
extern bool IRQ_FLAG_NAME(13); //D7
extern bool IRQ_FLAG_NAME(15); //D8

IRQ(16);
IRQ(5);
IRQ(4);
IRQ(0);
IRQ(2);
IRQ(14);
IRQ(12);
IRQ(13);
IRQ(15);
#endif
