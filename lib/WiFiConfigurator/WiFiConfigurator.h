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
    const char *hostname;
    const char *accesspoint;
    MDNSResponder mDNSHandler;

  public:
    ~WiFiConfigurator();
    WiFiConfigurator(const char *ssid, const char *password = nullptr, const char *hostname = nullptr, const char *accesspoint = nullptr);
    int initServices();
};

#endif