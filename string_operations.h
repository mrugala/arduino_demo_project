#pragma once

#include <Arduino.h>

namespace StringOps
{

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

template<typename T, typename... Args>
String getString(T val, Args... args)
{
    return to_string(val) + getString(args...);
}
String getString()
{
    return String("");
}

}
