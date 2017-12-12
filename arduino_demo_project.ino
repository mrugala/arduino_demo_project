#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
#pragma message "compiling with WiFi"
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#endif

#include <LiquidCrystal.h>
#include <Servo.h>

#include "project_defines.h"
#include "ControlModule.h"
#include "EepromConfig.h"

ControlModule cm(CONTROL_OVERRIDE_PIN, LIGHT_CONTROL_PIN, LIGHT_SWITCH_PIN);
EepromConfigManager eeprom(EEPROM_ALLOCATED_SIZE);

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
HTTPClient http;
ESP8266WebServer server(LISTENING_PORT);

String dev_name;
String auth_token;
String wifi_ssid;
String wifi_pwd;
#endif

void updateSwitch();
bool connectedToWiFi();

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println("");

    Serial.print("[" + String(millis()/1000., 3) + "] Light switch control module init... ");
    if (cm.init() < 0)
    {
        Serial.println("FAILURE");
    }
    else
    {
        Serial.println("SUCCESS");
    }

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
    eeprom.init();
    Serial.print("[" + String(millis()/1000., 3) + "] Reading config from EEPROM... ");
    auto config = eeprom.readConfiguration();
    wifi_ssid = config.wifi_ssid;
    wifi_pwd = config.wifi_pwd;
    dev_name = config.dev_name;
    auth_token = config.dev_name;
    Serial.println("DONE");
    
    Serial.print("[" + String(millis()/1000., 3) + "] WiFi init... ");
    WiFi.begin(wifi_ssid.c_str(), wifi_pwd.c_str());
    
    if (connectedToWiFi())
    {
        Serial.println("SUCCESS");
        Serial.print("[" + String(millis()/1000., 3) + "] Connected to: '");
        Serial.print(wifi_ssid);
        Serial.print("' with IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("[" + String(millis()/1000., 3) + "] WiFi signal RSSI: ");
        Serial.println(WiFi.RSSI());
    }
    else
    {
        Serial.println("FAILURE");
    }
    
    Serial.print("[" + String(millis()/1000., 3) + "] Setting up HTTP server... ");
    server.on("/switch", updateSwitch);
    server.begin();
    Serial.println("DONE");

    http.setTimeout(200);
    cm.setHttpClient(&http, config.domoticz_ip, config.domoticz_port);
    Serial.print("[" + String(millis()/1000., 3) + "] Updating HTTP status according to light switch... ");
    if (cm.updateHttpStatus(cm.getDeviceState()))
    {
        Serial.println("SUCCESS");
    }
    else
    {
        Serial.println("FAILURE");
    }
#endif
    Serial.println("[" + String(millis()/1000., 3) + "] Startup finished");
}

bool printToSerial {false};

void loop() 
{
    delay(MAIN_LOOP_INTERVAL);

    if (WiFi.status() != WL_CONNECTED)
    {
        if (printToSerial)
            Serial.print("WiFi/INFO: WiFi disconnected - attempt reconnection...");
        WiFi.begin(wifi_ssid.c_str(), wifi_pwd.c_str());
        if (printToSerial)
            Serial.println(connectedToWiFi() ? "SUCCESS" : "FAILURE");
    }

    server.handleClient();

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
                cm.setState(false);
                if (!cm.updateHttpStatus(false))
                    Serial.println("CM/WARN: Failed to update HTTP status");
                if (printToSerial)
                    Serial.println("CM/Info: Light turned OFF");
                break;
            case '+':
                cm.setState(true);
                if (!cm.updateHttpStatus(true))
                    Serial.println("CM/WARN: Failed to update HTTP status");
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
        if (!cm.updateHttpStatus(current_state))
            Serial.println("CM/WARN: Failed to update HTTP status");
    }
}

bool connectedToWiFi()
{
    unsigned char num_retries {0};
    while (WiFi.status() != WL_CONNECTED && num_retries < 10) 
    {
        delay(500);
        num_retries++;
    }
    return num_retries < 10;
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
    Serial.println("\tSwitch state: " + String(cm.getVoltageState()));
    Serial.println("\tLight state: " + String(cm.getDeviceState()));
    Serial.println("\tWiFi: " + String(WiFi.status() == WL_CONNECTED ? "CONNECTED" : "DISCONNECTED"));
    Serial.print("\tIP address: "); Serial.println(WiFi.localIP());
}

void updateSwitch()
{
    String id = server.arg("id");
    int state = server.arg("state").toInt();
    String token = server.arg("token");

    if (token != auth_token)
    {
        Serial.println("Server/ERR: Authentication failed");
    }

    if (id == dev_name)
    {
        cm.setState(state);
        if (printToSerial)
            Serial.println("Server/INFO: Turned " + String(dev_name) + " switch " + (state ? "ON" : "OFF") );
    }
    else if (printToSerial)
    {
        Serial.println("Server/WARN: Device not present");
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

