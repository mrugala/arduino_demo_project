#pragma once

class JoyStick
{
public:
    JoyStick(const unsigned horizontal_pin, const unsigned vertical_pin, const unsigned select_pin);
    virtual ~JoyStick();

    int init();
    double getHorizontalDeviation();
    double getVerticalDeviation();
    double getAngle_deg();
    bool testSelect();

private:
    bool* select_state;

    const unsigned horizontal_pin;
    const unsigned vertical_pin;
    const unsigned select_pin;

    unsigned read(const unsigned pin);
};





