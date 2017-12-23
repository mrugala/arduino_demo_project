#pragma once
#include <WString.h>
#include <IPAddress.h>
#include <vector>
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
#include <map>
#include <functional>
using CallbackFunction = std::function<void(bool)>;
#else
#include <ArduinoSTL.h>
#include <map>
#include <Function.h>
using CallbackFunction = vl::Func<void(bool)>;
#endif

template<class WebClient, class WebServer>
class WiFiModuleClass
{
public:
    WiFiModuleClass();
    virtual ~WiFiModuleClass();
    
    int initWiFi(String ssid, String password);
    int initWebClient(String ip, unsigned port);
    int initWebServer(String authenticationToken);

    void addDevice(String deviceName, CallbackFunction callback);

    void handleConnection();
    bool updateWebStatus(unsigned device_index, bool state);

    void updateDevice();

    std::vector<String> getStatus();

    IPAddress getLocalIp();
    int32_t getRssi();

private:
    String wifiSsid;
    String wifiPwd;

    WebClient* client;
    String httpHost;
    unsigned httpPort;

    WebServer* server;
    String authToken;
    std::map<String, CallbackFunction> deviceCallbacks;

    bool connectedToWiFi();
};

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
class HTTPClient;
class ESP8266WebServer;

String toString(IPAddress);

using WiFiModule = class WiFiModuleClass<HTTPClient, ESP8266WebServer>;
#endif

