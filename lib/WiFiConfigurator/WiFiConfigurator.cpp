#include "WiFiConfigurator.h"

WiFiConfigurator::~WiFiConfigurator()
{
}

void WiFiConfigurator::beginWiFi(const char *ssid, const char *password)
{
    WiFi.begin(ssid, password);

    //Wait until connection has been stablished
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }

    WiFiConfigurator::ssid = ssid;
    WiFiConfigurator::password = password;
}