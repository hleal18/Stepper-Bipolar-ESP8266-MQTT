#ifndef WiFiConfigurator_h
#define WiFiConfigurator_h

#include <ESP8266WiFi.h>

class WiFiConfigurator
{
  private:
    const char* ssid;
    const char* password;

  public:
    ~WiFiConfigurator();
    void beginWiFi(const char* ssid, const char* password);
};

#endif