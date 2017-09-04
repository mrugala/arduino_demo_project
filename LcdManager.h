#pragma once

#include <Arduino.h>

template<typename T>
String to_string(T val)
{
    return String(val);
}
template<>
String to_string(double val)
{
    String ret;
    if (abs(val) < 10.) ret = String(val, 4);
    else if (abs(val) < 100.) ret = String(val, 3);
    else if (abs(val) < 1000.) ret = String(val, 2);
    else if (abs(val) < 10000.) ret = String(val, 1);
    else ret = String(val, 0);
    return ret;
}
template<>
String to_string(int val)
{
    return String(val, DEC);
}

class LcdManager
{
public:
    struct LcdPins
    {
        const unsigned rs;
        const unsigned enable;
        const unsigned d4;
        const unsigned d5;
        const unsigned d6;
        const unsigned d7;
    };

    //LcdManager(const LcdPins pins);
    //virtual ~LcdManager();

    //int init(const unsigned cols, const unsigned rows, const unsigned screens);
    
    template<typename... Args>
    void print(const unsigned screen, const unsigned row, Args... args)
    {
         display_content[screen][row] = getString(args...);
    }
    void goToScreen(const unsigned screen);

private:
    template<typename T, typename... Args>
    String getString(T val, Args... args)
    {
        return to_string(val) + getString(args...);
    }

    String getString()
    {
        return String("");
    }

    String** display_content;
    const unsigned cols;
    const unsigned rows;
    const unsigned screens;
};





