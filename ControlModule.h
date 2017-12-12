#pragma once
#include <WString.h>

class HTTPClient;

class ControlModule
{
public:
    ControlModule(const unsigned control_override_pin, const unsigned control_pin, const unsigned voltage_detector_pin);
    virtual ~ControlModule();
    
    int init();
    /*
     * Updare state according to zero detection interrupt from light switch
     * @return state to which it was updated
     */
    bool updateState();
    void setState(bool state);
    bool getDeviceState();
    bool getVoltageState();
    void setHttpClient(HTTPClient* client, String host, unsigned port);
    bool updateHttpStatus(bool state);

private:
    bool voltage_state = false;
    bool device_state = false;

    const unsigned control_override_pin;
    const unsigned control_pin;
    const unsigned voltage_detector_pin;

    bool* irq_occured;
    unsigned irq_count = 0;

    HTTPClient* client;
    String httpHost;
    unsigned httpPort;
};

