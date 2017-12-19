#pragma once
#include <WString.h>
#include <IPAddress.h>
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
#include <functional>
using CallbackFunction = std::function<void(bool)>;
#else
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
    int initWebServer(String deviceName, String authenticationToken, CallbackFunction callback);

    void handleConnection();
    bool updateWebStatus(String device, bool state);

    void updateDevice();

    String getStatusString();

    IPAddress getLocalIp();
    int32_t getRssi();

private:
    String wifiSsid;
    String wifiPwd;

    WebClient* client;
    String httpHost;
    unsigned httpPort;

    WebServer* server;
    String devName;
    String authToken;
    CallbackFunction changeDeviceStateCallback;

    bool connectedToWiFi();
};

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
class HTTPClient;
class ESP8266WebServer;

String toString(IPAddress);

using WiFiModule = class WiFiModuleClass<HTTPClient, ESP8266WebServer>;
#endif

