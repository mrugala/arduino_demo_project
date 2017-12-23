#include "EepromConfig.h"
#include <EEPROM.h>
#include <Arduino.h>
#include <Logger.h>

String toString(EepromConfig config)
{
    String config_string;
    config_string += "\tdev_name:      " + String(config.dev_name) + ", len=" + String(config.dev_name_len) + "\n";
    config_string += "\twifi_ssid:     " + String(config.wifi_ssid) + ", len=" + String(config.wifi_ssid_len) + "\n";
    config_string += "\twifi_pwd:      " + String(config.wifi_pwd) + ", len=" + String(config.wifi_pwd_len) + "\n";
    config_string += "\tdomoticz_ip:   " + String(config.domoticz_ip) + ", len=" + String(config.domoticz_ip_len) + "\n";
    config_string += "\tdomoticz_port: " + String(config.domoticz_port) + "\n";
    config_string += "\tauth_token:    " + String(config.auth_token) + ", len=" + String(config.auth_token_len) + "\n";
    for (const auto& dev : config.dev_info)
    {
        config_string += "\tdevice #" + String(dev.id) + ":\n";
        config_string += "\t\tcontrol_override_pin: " + String(dev.control_override_pin) + "\n";
        config_string += "\t\tcontrol_pin:          " + String(dev.control_pin) + "\n";
        config_string += "\t\tswitch_pin:           " + String(dev.switch_pin) + "\n";        
    }
    return config_string;
}

EepromConfigManager::EepromConfigManager(size_t eepromLength)
    : eepromLength(eepromLength)
{
}

int EepromConfigManager::init()
{
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
    EEPROM.begin(eepromLength);
#endif
    Logging::init("EEPROM initialized with length=" + String(eepromLength) + 
                   ", memory used for config=" + String(USED_EEPROM_MEMORY) + "\n");
    return 0;
}

EepromConfig EepromConfigManager::readConfiguration() const
{
    EepromConfig config;

    config.dev_name_len     = readCharArrayFromEeprom(DEV_NAME_POS,    config.dev_name,    DEV_NAME_FIELD_LEN);
    config.wifi_ssid_len    = readCharArrayFromEeprom(WIFI_SSID_POS,   config.wifi_ssid,   WIFI_SSID_FIELD_LEN);
    config.wifi_pwd_len     = readCharArrayFromEeprom(WIFI_PWD_POS,    config.wifi_pwd,    WIFI_PWD_FIELD_LEN);
    config.domoticz_ip_len  = readCharArrayFromEeprom(DOMOTICZ_IP_POS, config.domoticz_ip, DOMOTICZ_IP_FIELD_LEN);
    EEPROM.get(DOMOTICZ_PORT_POS, config.domoticz_port);
    config.auth_token_len   = readCharArrayFromEeprom(AUTH_TOKEN_POS,  config.auth_token,  AUTH_TOKEN_FIELD_LEN);
    EEPROM.get(DEV_ID_1_POS, config.dev_info[0]);
    EEPROM.get(DEV_ID_2_POS, config.dev_info[1]);

    return config;
}

void EepromConfigManager::writeConfiguration(const EepromConfig& config)
{
    writeCharArrayToEeprom(DEV_NAME_POS,    config.dev_name,    max(config.dev_name_len+1, DEV_NAME_FIELD_LEN));
    writeCharArrayToEeprom(WIFI_SSID_POS,   config.wifi_ssid,   max(config.wifi_ssid_len+1, WIFI_SSID_FIELD_LEN));
    writeCharArrayToEeprom(WIFI_PWD_POS,    config.wifi_pwd,    max(config.wifi_pwd_len+1, WIFI_PWD_FIELD_LEN));
    writeCharArrayToEeprom(DOMOTICZ_IP_POS, config.domoticz_ip, max(config.domoticz_ip_len+1, DOMOTICZ_IP_FIELD_LEN));
    EEPROM.put(DOMOTICZ_PORT_POS, config.domoticz_port);
    writeCharArrayToEeprom(AUTH_TOKEN_POS,  config.auth_token,  max(config.auth_token_len, AUTH_TOKEN_FIELD_LEN));
    EEPROM.put(DEV_ID_1_POS, config.dev_info[0]);
    EEPROM.put(DEV_ID_2_POS, config.dev_info[1]);
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
    EEPROM.commit();
#endif
}

void EepromConfigManager::writeCharArrayToEeprom(const size_t address, const char* buf, const size_t len)
{
    size_t mem_cell = address;
    for (int i = 0; i < len; ++i)
    {
         EEPROM.write(mem_cell+i, buf[i]);
    }
}

size_t EepromConfigManager::readCharArrayFromEeprom(const size_t address, char* buf, const size_t len) const
{
    size_t mem_cell = address;
    int i;
    for (i = 0; i < len; ++i)
    {
        buf[i] = EEPROM.read(mem_cell+i);
        if (buf[i] == '\0')
            break;
    }
    return i;
}

