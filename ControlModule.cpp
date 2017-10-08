#include "ControlModule.h"
#include <Arduino.h>
#include "IRQ.h"

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
    pinMode(control_override_pin, OUTPUT);
    digitalWrite(control_override_pin, HIGH);

    pinMode(control_pin, OUTPUT);
    
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

    digitalWrite(control_pin, device_state ? HIGH : LOW);
}

void ControlModule::updateState()
{
    bool prev_voltage_state = voltage_state;
    voltage_state = *irq_occured;

    if (voltage_state != prev_voltage_state)
    {
        device_state = !device_state;
    }

    digitalWrite(control_pin, device_state ? HIGH : LOW);
}

void ControlModule::setState(bool state)
{
    device_state = state;

    digitalWrite(control_pin, device_state ? HIGH : LOW);
}

bool ControlModule::getDeviceState()
{
    return device_state;
}

bool ControlModule::getVoltageState()
{
    return voltage_state;
}

