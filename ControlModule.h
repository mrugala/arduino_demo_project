#pragma once

class ControlModule
{
public:
    ControlModule(const unsigned control_override_pin, const unsigned control_pin, const unsigned voltage_detector_pin);
    virtual ~ControlModule();
    
    int init();
    void updateState();
    void setState(bool state);
    bool getDeviceState();
    bool getVoltageState();

private:
    bool voltage_state = false;
    bool device_state = false;

    const unsigned control_override_pin;
    const unsigned control_pin;
    const unsigned voltage_detector_pin;

    bool* irq_occured;
};

