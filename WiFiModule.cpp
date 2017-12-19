#include "WiFiModule.h"
#include <Arduino.h>

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
#pragma message "compiling with WiFi"
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

String toString(IPAddress address)
{
    return address.toString();
}

#else
class WiFiStub
{
    void begin(const char*, const char*) {}
    int status() { return -1; }
    int RSSI() { return 0; }
    IPAddress localIP() { return {}; }
};

static constexpr int HTTP_CODE_OK = 200;
static constexpr int WL_CONNECTED = 0;

String toString(IPAddress address)
{
    return String(address[0]) + '.' + String(address[1]) + '.' + String(address[2]) + '.' + String(address[3]);
}

WiFiStub WiFi;
#endif

template<class WebClient, class WebServer>
void WiFiModuleClass<WebClient, WebServer>::updateDevice()
{
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)  
    String id = server->arg("id");
    int state = server->arg("state").toInt();
    String token = server->arg("token");

    if (token != authToken)
    {
        Serial.println("Server/ERR: Authentication failed");
        Serial.println("Server/ERR: got token: " + token + ", expected: " + authToken);
        return;
    }

    if (id == devName)
    {
        changeDeviceStateCallback(state);
//        if (printToSerial)
            Serial.println("Server/INFO: Turned " + String(devName) + " switch " + (state ? "ON" : "OFF") );
    }
    else /*if (printToSerial)*/
    {
        Serial.println("Server/WARN: Device not present");
    }
#endif
}

template<class WebClient, class WebServer>
WiFiModuleClass<WebClient, WebServer>::WiFiModuleClass()
{
}

template<class WebClient, class WebServer>
WiFiModuleClass<WebClient, WebServer>::~WiFiModuleClass()
{
    if (server != nullptr)
        delete server;
}

template<class WebClient, class WebServer>
int WiFiModuleClass<WebClient, WebServer>::initWiFi(String ssid, String password)
{
    this->wifiSsid = ssid;
    this->wifiPwd = password;
    WiFi.begin(wifiSsid.c_str(), wifiPwd.c_str());
    return connectedToWiFi() ? 0 : -1;
}

template<class WebClient, class WebServer>
int WiFiModuleClass<WebClient, WebServer>::initWebClient(String host, unsigned port)
{
    this->client = new WebClient;
    this->client->setTimeout(1000);
    this->httpHost = host;
    this->httpPort = port;
    return 0;
}

template<class WebClient, class WebServer>
int WiFiModuleClass<WebClient, WebServer>::initWebServer(String deviceName, String authenticationToken, CallbackFunction callback)
{
    this->server = new WebServer(80);
    this->devName = deviceName;
    this->authToken = authenticationToken;
    this->changeDeviceStateCallback = callback;
    this->server->on("/switch", [this] {
            this->updateDevice();
        });
    this->server->begin();
    return 0;
}

template<class WebClient, class WebServer>
bool WiFiModuleClass<WebClient, WebServer>::updateWebStatus(String device, bool state)
{
    if (!client)
    {
        return true;
    }

    bool status = false;
    
    client->begin(httpHost, httpPort, "/json.htm?type=command&param=udevice&idx=1&nvalue=" + String(state ? "1" : "0"));
    int httpCode = client->GET();
    if (httpCode == HTTP_CODE_OK) 
    {
//        Serial.println(client->getString());
        status = true;
    }
    client->end();
    return status;
}

template<class WebClient, class WebServer>
void WiFiModuleClass<WebClient, WebServer>::handleConnection()
{
    if (WiFi.status() != WL_CONNECTED)
    {
//        if (printToSerial)
            Serial.print("WiFi/INFO: WiFi disconnected - attempt reconnection...");
        WiFi.begin(wifiSsid.c_str(), wifiPwd.c_str());
//        if (printToSerial)
            Serial.println(connectedToWiFi() ? "SUCCESS" : "FAILURE");
    }

    server->handleClient();
}

template<class WebClient, class WebServer>
bool WiFiModuleClass<WebClient, WebServer>::connectedToWiFi()
{
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
    unsigned char num_retries {0};
    while (WiFi.status() != WL_CONNECTED && num_retries < 10) 
    {
        delay(500);
        num_retries++;
    }
    return num_retries < 10;
#else
    return false;
#endif
}

template<class WebClient, class WebServer>
String WiFiModuleClass<WebClient, WebServer>::getStatusString()
{
    String status;
    status += "\tWiFi: " + String(WiFi.status() == WL_CONNECTED ? "CONNECTED" : "DISCONNECTED") + ", RSSI: " 
            + String(WiFi.RSSI()) + "\n";
    status += "\tIP address: " + toString(WiFi.localIP()) + "\n";
    return status;
}

template<class WebClient, class WebServer>
IPAddress WiFiModuleClass<WebClient, WebServer>::getLocalIp()
{
    return WiFi.localIP();
}

template<class WebClient, class WebServer>
int32_t WiFiModuleClass<WebClient, WebServer>::getRssi()
{
    return WiFi.RSSI();
}

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
template class WiFiModuleClass<HTTPClient, ESP8266WebServer>;
#endif

