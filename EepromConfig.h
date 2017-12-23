#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <array>
#include <WString.h>

static constexpr unsigned NUM_DEVICES = 2;

struct DeviceEepromConfig
{
    uint8_t id;
    uint8_t control_override_pin;
    uint8_t control_pin;
    uint8_t switch_pin;
};

struct EepromConfig
{
    char dev_name[256];
    size_t dev_name_len;
    char wifi_ssid[256];
    size_t wifi_ssid_len;
    char wifi_pwd[256];
    size_t wifi_pwd_len;
    char domoticz_ip[256];
    size_t domoticz_ip_len;
    unsigned short domoticz_port;
    char auth_token[256];
    size_t auth_token_len;
    std::array<DeviceEepromConfig, NUM_DEVICES> dev_info;
};

String toString(EepromConfig config);

class EepromConfigManager
{
public:
    EepromConfigManager(size_t eepromLength);

    int init();
    
    EepromConfig readConfiguration() const;
    void writeConfiguration(const EepromConfig& config);

private:
    void writeCharArrayToEeprom(const size_t address, const char* buf, const size_t len);
    size_t readCharArrayFromEeprom(const size_t address, char* buf, const size_t len) const;

    static constexpr size_t DEV_NAME_POS = 0;
    static constexpr size_t DEV_NAME_FIELD_LEN = 128;
    static constexpr size_t WIFI_SSID_POS = DEV_NAME_FIELD_LEN;
    static constexpr size_t WIFI_SSID_FIELD_LEN = 128;
    static constexpr size_t WIFI_PWD_POS = WIFI_SSID_POS + WIFI_SSID_FIELD_LEN;
    static constexpr size_t WIFI_PWD_FIELD_LEN = 128;
    static constexpr size_t DOMOTICZ_IP_POS = WIFI_PWD_POS + WIFI_PWD_FIELD_LEN;
    static constexpr size_t DOMOTICZ_IP_FIELD_LEN = 16;
    static constexpr size_t DOMOTICZ_PORT_POS = DOMOTICZ_IP_POS + DOMOTICZ_IP_FIELD_LEN;
    static constexpr size_t DOMOTICZ_PORT_FIELD_LEN = 2;
    static constexpr size_t AUTH_TOKEN_POS = DOMOTICZ_PORT_POS + DOMOTICZ_PORT_FIELD_LEN;
    static constexpr size_t AUTH_TOKEN_FIELD_LEN = 128;
    static constexpr size_t DEV_ID_1_POS = AUTH_TOKEN_POS + AUTH_TOKEN_FIELD_LEN;
    static constexpr size_t DEV_CONTROL_OVERRIDE_PIN_1_POS = DEV_ID_1_POS + 1;
    static constexpr size_t DEV_CONTROL_PIN_1_POS = DEV_ID_1_POS + 2;
    static constexpr size_t DEV_SWITCH_PIN_1_POS = DEV_ID_1_POS + 3;
    static constexpr size_t DEV_ID_2_POS = DEV_ID_1_POS + 4;
    static constexpr size_t DEV_CONTROL_OVERRIDE_PIN_2_POS = DEV_ID_2_POS + 1;
    static constexpr size_t DEV_CONTROL_PIN_2_POS = DEV_ID_2_POS + 2;
    static constexpr size_t DEV_SWITCH_PIN_2_POS = DEV_ID_2_POS + 3;
    static constexpr size_t USED_EEPROM_MEMORY = DEV_ID_2_POS + 4;

    const size_t eepromLength;
};



