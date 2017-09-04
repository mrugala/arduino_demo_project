#pragma once

class HC_SR04
{
public:
    HC_SR04(const unsigned trigger_pin, const unsigned echo_pin);

    int init();
    double getDistance_cm();

private:
    const unsigned trigger_pin;
    const unsigned echo_pin;

    unsigned read();
};





