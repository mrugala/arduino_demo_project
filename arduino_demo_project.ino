#include <LiquidCrystal.h>
#include <Servo.h>

#include "project_defines.h"
#include "ControlModule.h"
#include "EepromConfig.h"
#include "WiFiModule.h"

ControlModule cm1(CONTROL_OVERRIDE_PIN, LIGHT_CONTROL_PIN_1, LIGHT_SWITCH_PIN_1);
ControlModule cm2(CONTROL_OVERRIDE_PIN, LIGHT_CONTROL_PIN_2, LIGHT_SWITCH_PIN_2);
EepromConfigManager eeprom(EEPROM_ALLOCATED_SIZE);

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
WiFiModule wifi;
#endif

String dev_name;

void updateControlModule(unsigned device_index, ControlModule& cm);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println("");

    Serial.print("[" + String(millis()/1000., 3) + "] Light switch control module #1 init... ");
    Serial.println((cm1.init() < 0) ? "FAILURE" : "SUCCESS");
    Serial.print("[" + String(millis()/1000., 3) + "] Light switch control module #2 init... ");
    Serial.println((cm2.init() < 0) ? "FAILURE" : "SUCCESS");

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
    eeprom.init();
    Serial.print("[" + String(millis()/1000., 3) + "] Reading config from EEPROM... ");
    auto config = eeprom.readConfiguration();
    dev_name = config.dev_name;
    Serial.println("DONE");
    
    Serial.print("[" + String(millis()/1000., 3) + "] WiFi init... ");
    if (!wifi.initWiFi(config.wifi_ssid, config.wifi_pwd))
    {
        Serial.println("SUCCESS");
        Serial.println("[" + String(millis()/1000., 3) + "] Connected to: '" + config.wifi_ssid + "' with IP address: "
                       + toString(wifi.getLocalIp()));
        Serial.println("[" + String(millis()/1000., 3) + "] WiFi signal RSSI: " + String(wifi.getRssi()));
    }
    else
    {
        Serial.println("FAILURE");
    }
    Serial.print("[" + String(millis()/1000., 3) + "] Setting up HTTP server... ");
    wifi.initWebServer(config.auth_token);
    wifi.addDevice(config.dev_name + String("1"), [&](bool state){ cm1.setState(state); });
    wifi.addDevice(config.dev_name + String("2"), [&](bool state){ cm2.setState(state); });
    Serial.println("DONE");
    wifi.initWebClient(config.domoticz_ip, config.domoticz_port);
    Serial.print("[" + String(millis()/1000., 3) + "] Updating HTTP status according to light switch #1... ");
    Serial.println(wifi.updateWebStatus(1, cm1.getDeviceState()) ? "SUCCESS" : "FAILURE");
    Serial.print("[" + String(millis()/1000., 3) + "] Updating HTTP status according to light switch #2... ");
    Serial.println(wifi.updateWebStatus(2, cm2.getDeviceState()) ? "SUCCESS" : "FAILURE");
#endif
    Serial.println("[" + String(millis()/1000., 3) + "] Startup finished");
}

bool printToSerial {false};

void loop() 
{
    delay(MAIN_LOOP_INTERVAL);
    
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
    wifi.handleConnection();
#endif
    if (Serial.available() > 0) 
    {
        auto read_byte = Serial.read();
        switch (read_byte)
        {
            case 'd':
                printToSerial = true;
                break;
            case 's':
                printToSerial = false;
                break;
            case 'h':
                printHelp();
                break;
            case '-':
                cm1.setState(false);
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
                if (!wifi.updateWebStatus(1, false))
                    Serial.println("CM/WARN: Failed to update HTTP status");
#endif
                if (printToSerial)
                    Serial.println("CM/Info: Light turned OFF");
                break;
            case '+':
                cm1.setState(true);
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
                if (!wifi.updateWebStatus(1, true))
                    Serial.println("CM/WARN: Failed to update HTTP status");
#endif
                if (printToSerial)
                    Serial.println("CM/Info: Light turned ON");
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
                if (printToSerial)
                    Serial.println("Error: Invalid input");
                break;
        }
    }

    updateControlModule(1, cm1);
    updateControlModule(2, cm2);
}

void updateControlModule(unsigned device_index, ControlModule& cm)
{
    auto prev_state = cm.getDeviceState();
    auto current_state = cm.updateState();
    if (prev_state != current_state)
    {
        if (printToSerial)
        {
            Serial.print("CM/Info: Light turned ");
            Serial.print(current_state ? "ON" : "OFF");
            Serial.println(" using a light switch");
        }
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
        if (!wifi.updateWebStatus(device_index, current_state))
            Serial.println("CM/WARN: Failed to update HTTP status");
#endif
    }
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
    Serial.println("\tDevice: " + dev_name + "1");
    Serial.println("\tSwitch state: " + String(cm1.getVoltageState()));
    Serial.println("\tLight state: " + String(cm1.getDeviceState()));
    Serial.println("\tDevice: " + dev_name + "2");
    Serial.println("\tSwitch state: " + String(cm2.getVoltageState()));
    Serial.println("\tLight state: " + String(cm2.getDeviceState()));
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
    Serial.print(wifi.getStatusString());
#endif
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
    Serial.println("\tdev_name: " + String(config.dev_name) + ", len=" + String(config.dev_name_len));
    Serial.println("\twifi_ssid: " + String(config.wifi_ssid) + ", len=" + String(config.wifi_ssid_len));
    Serial.println("\twifi_pwd: " + String(config.wifi_pwd) + ", len=" + String(config.wifi_pwd_len));
    Serial.println("\tdomoticz_ip: " + String(config.domoticz_ip) + ", len=" + String(config.domoticz_ip_len));
    Serial.println("\tdomoticz_port: " + String(config.domoticz_port));
    Serial.println("\tauth_token: " + String(config.auth_token) + ", len=" + String(config.auth_token_len));

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
    Serial.println("\tdev_name: " + String(config.dev_name) + ", len=" + String(config.dev_name_len));
    Serial.println("\twifi_ssid: " + String(config.wifi_ssid) + ", len=" + String(config.wifi_ssid_len));
    Serial.println("\twifi_pwd: " + String(config.wifi_pwd) + ", len=" + String(config.wifi_pwd_len));
    Serial.println("\tdomoticz_ip: " + String(config.domoticz_ip) + ", len=" + String(config.domoticz_ip_len));
    Serial.println("\tdomoticz_port: " + String(config.domoticz_port));
    Serial.println("\tauth_token: " + String(config.auth_token) + ", len=" + String(config.auth_token_len));
}

