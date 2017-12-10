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

ControlModule cm(CONTROL_OVERRIDE_PIN, LIGHT_CONTROL_PIN, LIGHT_SWITCH_PIN);

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
HTTPClient http;
ESP8266WebServer server(LISTENING_PORT);
#endif

void updateSwitch();
bool connectToWiFi();

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println("");

    Serial.print("Light switch control module init... ");
    if (cm.init() < 0)
    {
        Serial.println("FAILURE");
    }
    else
    {
        Serial.println("SUCCESS");
    }

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
    wdt_disable();
    Serial.print("WiFi init... ");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println(connectToWiFi() ? "SUCCESS" : "FAILURE");

    Serial.print("Connected to: '");
    Serial.print(WIFI_SSID);
    Serial.print("' with IP address: ");
    Serial.println(WiFi.localIP());
    
    Serial.print("Setting up HTTP server... ");
    server.on("/switch", updateSwitch);
    server.begin();
    Serial.println("DONE");

    cm.setHttpClient(&http, SERVER_ADDRESS, DOMOTICZ_PORT);
    Serial.print("Updating HTTP status according to light switch... ");
    if (cm.updateHttpStatus(cm.getDeviceState()))
    {
        Serial.println("SUCCESS");
    }
    else
    {
        Serial.println("FAILURE");
    }
        
#endif
}

bool printToSerial {false};

void loop() 
{
    delay(MAIN_LOOP_INTERVAL);

    if (WiFi.status() != WL_CONNECTED)
    {
        if (printToSerial)
            Serial.print("WiFi/INFO: WiFi disconnected - attempt reconnection...");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        if (printToSerial)
            Serial.println(connectToWiFi() ? "SUCCESS" : "FAILURE");
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

bool connectToWiFi()
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

    if (token != AUTH_TOKEN)
    {
        Serial.println("Server/ERR: Authentication failed");
    }

    if (id == DEVICE_NAME)
    {
        cm.setState(state);
        if (printToSerial)
            Serial.println("Server/INFO: Turned " + String(DEVICE_NAME) + " switch " + (state ? "ON" : "OFF") );
    }
    else if (printToSerial)
    {
        Serial.println("Server/WARN: Device not present");
    }
}

