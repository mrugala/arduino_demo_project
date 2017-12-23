#include <vector>
#include "DeviceManager.h"
#include "EepromConfig.h"
#include <Logger.h>
#include "project_defines.h"
#include <array>
#include <Arduino.h>

DeviceManager::DeviceManager()
{
}

DeviceManager& DeviceManager::instance()
{
    static DeviceManager manager;

    return manager;
}

void DeviceManager::init(EepromConfig& config)
{
    this->dev_name = config.dev_name;
    for (const auto& dev : config.dev_info)
    {
        this->control_modules.emplace(dev.id, ControlModule(dev.control_override_pin,
                                                            dev.control_pin,
                                                            dev.switch_pin));
    }

    for (auto& cm : this->control_modules)
    {
        Logging::init("Initializing light switch control module #" + String(cm.first) + "... \n");
        Logging::init("Light switch control module #" + String(cm.first) + " initialization " 
                      + String((cm.second.init() < 0) ? "FAILED" : "SUCCESSFUL") + "\n");
    }
    
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
    Logging::init("Connecting to WiFi... \n");
    if (!this->wifi.initWiFi(config.wifi_ssid, config.wifi_pwd))
    {
        Logging::init("WiFi connection SUCCESSFUL\n");
        Logging::init("Connected to: '" + String(config.wifi_ssid) + "' with IP address: "
                       + toString(this->wifi.getLocalIp()) + "\n");
        Logging::init("WiFi signal RSSI: " + String(this->wifi.getRssi()) + "\n");
    }
    else
    {
        Logging::init("WiFi connection FAILED\n");
    }
    Logging::init("Setting up HTTP server... \n");
    this->wifi.initWebServer(config.auth_token);
    for (auto& cm : this->control_modules)
    {
        this->wifi.addDevice(config.dev_name + String(cm.first), [&](bool state){ cm.second.setState(state); });
    }
    Logging::init("Finished registering devices to HTTP server\n");
    this->wifi.initWebClient(config.domoticz_ip, config.domoticz_port);
    for (const auto& cm : this->control_modules)
    {
        Logging::init("Updating HTTP status according to light switch #" + String(cm.first) + "... \n");
        Logging::init("HTTP client initialization status: " 
                      + String(this->wifi.updateWebStatus(cm.first, cm.second.getDeviceState()) ? "SUCCESS" : "FAILURE") + "\n");
    }
#endif
}



std::map<unsigned, ControlModule>& DeviceManager::getControlModules()
{
    return this->control_modules;
}

WiFiModule& DeviceManager::getWiFiModule()
{
    return this->wifi;
}

void DeviceManager::updateControlModules()
{
    for (auto& control_module : this->control_modules)
    {
        updateControlModule(control_module.first, control_module.second);
    }
}

void DeviceManager::updateControlModule(unsigned device_index, ControlModule& cm)
{
    auto prev_state = cm.getDeviceState();
    auto current_state = cm.updateState();
    if (prev_state != current_state)
    {
        Logging::info("CM", "Light #" + String(device_index) + " turned " + (current_state ? "ON" : "OFF") + " using a light switch\n");
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
        if (!this->wifi.updateWebStatus(device_index, current_state))
            Logging::warning("CM", "Failed to update HTTP status\n");
#endif
    }
}

void DeviceManager::setDeviceState(unsigned device_index, bool state)
{
    this->control_modules.at(device_index).setState(state);
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
    if (!this->wifi.updateWebStatus(device_index, state))
        Logging::warning("loop", "Failed to update HTTP status\n");
#endif
    Logging::info("loop", "Light #" + String(device_index) + " turned " + (state ? "ON" : "OFF") + "\n");
}

std::vector<String> DeviceManager::getStatus()
{
    std::vector<String> status;
    for (const auto& cm : this->control_modules)
    {
        status.push_back("\tDevice: " + this->dev_name + String(cm.first));
        status.push_back("\tSwitch state: " + String(cm.second.getVoltageState()));
        status.push_back("\tLight state: " + String(cm.second.getDeviceState()));
    }
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
    auto wifi_status = this->wifi.getStatus();
    status.insert(status.end(), wifi_status.begin(), wifi_status.end());
#endif
    return status;
}

