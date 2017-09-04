#include "hc_sr04.h"
#include "Arduino.h"
#include "project_defines.h"

HC_SR04::HC_SR04(const unsigned trigger_pin, const unsigned echo_pin)
    : trigger_pin(trigger_pin), echo_pin(echo_pin)
{

}

int HC_SR04::init()
{
    pinMode(trigger_pin, OUTPUT);
    pinMode(echo_pin, INPUT);

    return 0;
}

double HC_SR04::getDistance_cm()
{
    return read() * 34. / 1000. / 2.;
}

unsigned HC_SR04::read()
{
    digitalWrite(trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger_pin, LOW);

    return pulseIn(echo_pin, HIGH);
}

