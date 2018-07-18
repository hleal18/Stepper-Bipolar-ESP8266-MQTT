#ifndef WiFiConfigurator_h
#define WiFiConfigurator_h

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

class WiFiConfigurator
{
  private:
    const char* ssid;
    const char* password;
    MDNSResponder mDNSHandler;

  public:
    ~WiFiConfigurator();
    void beginWiFiConnection(const char* ssid, const char* password);
    bool beginMDNSService(const char* hostname);
};

#endif