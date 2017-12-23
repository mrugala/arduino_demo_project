#include <Logger.h>

#include "project_defines.h"
#include "EepromConfig.h"
#include "DeviceManager.h"

EepromConfigManager eeprom(EEPROM_ALLOCATED_SIZE);\

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println("");

    Logging::setVerbosity(Logging::Verbosity::ERROR);
    eeprom.init();
    Logging::init("Reading config from EEPROM... \n");
    auto config = eeprom.readConfiguration();
//    config.dev_1.id = 1;
//    config.dev_1.control_override_pin = CONTROL_OVERRIDE_PIN;
//    config.dev_1.control_pin = LIGHT_CONTROL_PIN_1;
//    config.dev_1.switch_pin = LIGHT_SWITCH_PIN_1;
//    config.dev_2.id = 2;
//    config.dev_2.control_override_pin = CONTROL_OVERRIDE_PIN;
//    config.dev_2.control_pin = LIGHT_CONTROL_PIN_2;
//    config.dev_2.switch_pin = LIGHT_SWITCH_PIN_2;
//    eeprom.writeConfiguration(config);
    Logging::init("Finished reading EEPROM\n");
    DeviceManager::instance().init(config);
    Logging::init("Startup finished\n");
}

void loop() 
{
    delay(MAIN_LOOP_INTERVAL);
    
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
    DeviceManager::instance().getWiFiModule().handleConnection();
#endif
    const unsigned index = DeviceManager::instance().getControlModules().begin()->first;
    if (Serial.available() > 0) 
    {
        auto read_byte = Serial.read();
        switch (read_byte)
        {
            case 'd':
                Logging::setVerbosity(Logging::Verbosity::DEBUG);
                break;
            case 's':
                Logging::setVerbosity(Logging::Verbosity::ERROR);
                break;
            case 'h':
                printHelp();
                break;
            case '-':
                DeviceManager::instance().setDeviceState(index, false);
                break;
            case '+':
                DeviceManager::instance().setDeviceState(index, true);
                break;
            case 'i':
                printStatus();
                break;
            case 'w':
                overrideConfiguration();
                break;
            case 'r':
                readConfiguration();
                break;    
            default:
                Logging::info("loop", "Invalid input\n");
                break;
        }
    }

    DeviceManager::instance().updateControlModules();
}

void printHelp()
{
    Serial.println("Available options:");
    Serial.println("\th - print this help message");
    Serial.println("\td - print debug messages");
    Serial.println("\ts - stop printing");
    Serial.println("\t- - turn light off");
    Serial.println("\t+ - turn light on");
    Serial.println("\ti - print device status");
    Serial.println("\tw - write configuration to EEPROM");
    Serial.println("\tr - read configuration from EEPROM");
}

void printStatus()
{
    Serial.println("Device status:");
    for (const auto& line : DeviceManager::instance().getStatus())
    {
        Serial.println(line);
    }
}

void overrideConfiguration()
{
    EepromConfig config;   
    Serial.setTimeout(1000*1000);
    Serial.println("Type device name");
    config.dev_name_len = Serial.readBytesUntil('\n', config.dev_name, 256); config.dev_name[config.dev_name_len] = 0;
    Serial.println("Type wifi ssid name");
    config.wifi_ssid_len = Serial.readBytesUntil('\n', config.wifi_ssid, 256); config.wifi_ssid[config.wifi_ssid_len] = 0;
    Serial.println("Type wifi password");
    config.wifi_pwd_len = Serial.readBytesUntil('\n', config.wifi_pwd, 256); config.wifi_pwd[config.wifi_pwd_len] = 0;
    Serial.println("Type domoticz IP address");
    config.domoticz_ip_len = Serial.readBytesUntil('\n', config.domoticz_ip, 256); config.domoticz_ip[config.domoticz_ip_len] = 0;
    Serial.println("Type domoticz port");
    config.domoticz_port = Serial.readStringUntil('\n').toInt();
    Serial.println("Type auth token");
    config.auth_token_len = Serial.readBytesUntil('\n', config.auth_token, 256); config.auth_token[config.auth_token_len] = 0;

    Serial.println("Configuration to be written to EEPROM:");
    Serial.print(toString(config));

    Serial.print("Override EEPROM? (y/N) ");
    String approval = Serial.readStringUntil('\n');
    if (approval == "y" || approval == "Y")
    {
        Serial.println(approval);
        eeprom.writeConfiguration(config);
        Serial.println("New configuration written to EEPROM");
    }
    else
    {
        Serial.println("N");
        Serial.println("New configuration discarded");
    }
    Serial.setTimeout(1000);
}

void readConfiguration()
{
    auto config = eeprom.readConfiguration();
    Serial.println("Current configuration read from EEPROM:");
    Serial.print(toString(config));
}

