#ifndef WiFiConfigurator_h
#define WiFiConfigurator_h

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>

class WiFiConfigurator
{
  private:
    const char *ssid;
    const char *password;
    MDNSResponder mDNSHandler;

  public:
    ~WiFiConfigurator();
    void beginWiFiConnection(const char *ssid, const char *password);
    void beginWiFiConnection(const char *ssid, const char *password, const char *wifiManagerService);
    bool beginMDNSService(const char *hostname);
};

#endif