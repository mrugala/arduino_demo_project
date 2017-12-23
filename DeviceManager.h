#pragma once

#include "WiFiModule.h"
#include "ControlModule.h"
#include <map>
#include <vector>

struct EepromConfig;

class DeviceManager
{
public:
    static DeviceManager& instance();

    void init(EepromConfig& config);

    std::map<unsigned, ControlModule>& getControlModules();
    WiFiModule& getWiFiModule();
    
    void updateControlModules();
    void setDeviceState(unsigned device_index, bool state);

    std::vector<String> getStatus();

private:
    DeviceManager();

    void updateControlModule(unsigned device_index, ControlModule& cm);
    
    std::map<unsigned, ControlModule> control_modules;
    WiFiModule wifi; 
    String dev_name;
};

