#include "ControlModule.h"
#include <Arduino.h>
#include "IRQ.h"

namespace
{
    const static unsigned ZERO_DETECTION_INTERRUPT_THRESHOLD = 5;
}

ControlModule::ControlModule(const unsigned control_override_pin, const unsigned control_pin, const unsigned voltage_detector_pin)
    : control_override_pin(control_override_pin), control_pin(control_pin), voltage_detector_pin(voltage_detector_pin)
{
}

ControlModule::~ControlModule()
{
    detachInterrupt(digitalPinToInterrupt(voltage_detector_pin));
}

int ControlModule::init()
{
    pinMode(voltage_detector_pin, INPUT_PULLUP);
    IRQ irq;
    switch (voltage_detector_pin)
    {
#if (ARDUINO < 160) || defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MINI)
       case 2:
          irq = &IRQ_2;
          irq_occured = &irq_flag_2;
          break;
       case 3:
          irq = &IRQ_3;
          irq_occured = &irq_flag_3;
          break;
#elif defined(ARDUINO_ESP8266_WEMOS_D1MINI)
       case D0:
          return -1;
          break;
       case D1:
          irq = &IRQ_5;
          irq_occured = &irq_flag_5;
          break;
       case D2:
          irq = &IRQ_4;
          irq_occured = &irq_flag_4;
          break;
       case D3:
          //TODO: not sure if should support D3 (GPIO0)?
          irq = &IRQ_0;
          irq_occured = &irq_flag_0;
          break;
       case D4:
          //TODO: not sure if should support D4 (GPIO2)?
          irq = &IRQ_2;
          irq_occured = &irq_flag_2;
          break;
       case D5:
          irq = &IRQ_14;
          irq_occured = &irq_flag_14;
          break;
       case D6:
          irq = &IRQ_12;
          irq_occured = &irq_flag_12;
          break;
       case D7:
          irq = &IRQ_13;
          irq_occured = &irq_flag_13;
          break;
       case D8:
          //TODO: not sure if should support D8 (GPIO15)?
          irq = &IRQ_15;
          irq_occured = &irq_flag_15;
          break;
#endif
       default:
          return -1;
    }

    delayMicroseconds(50);

    voltage_state = *irq_occured;
    *irq_occured =  false;
    if (voltage_state)
    {
        device_state = true;
    }

    int interrupt = digitalPinToInterrupt(voltage_detector_pin);
    if (interrupt < 0)
        return interrupt;
    attachInterrupt(interrupt, irq, FALLING);

    pinMode(control_pin, OUTPUT);
    digitalWrite(control_pin, device_state ? HIGH : LOW);
    
    pinMode(control_override_pin, OUTPUT);
    digitalWrite(control_override_pin, HIGH);
    
    return 0;
}

bool ControlModule::updateState()
{
    bool prev_voltage_state = voltage_state;
    if (*irq_occured)
    {
        irq_count++;
        if (irq_count > ZERO_DETECTION_INTERRUPT_THRESHOLD)
           voltage_state = true;
    }
    else
    {
        irq_count = 0;
        voltage_state = false;
    }
    *irq_occured = false;

    if (voltage_state != prev_voltage_state)
    {
        device_state = !device_state;
    }

    digitalWrite(control_pin, device_state ? HIGH : LOW);

    return getDeviceState();
}

void ControlModule::setState(bool state)
{
    device_state = state;

    digitalWrite(control_pin, device_state ? HIGH : LOW);
}

bool ControlModule::getDeviceState() const
{
    return device_state;
}

bool ControlModule::getVoltageState() const
{
    return voltage_state;
}

