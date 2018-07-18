#include "WiFiConfigurator.h"

WiFiConfigurator::~WiFiConfigurator()
{
}

void WiFiConfigurator::beginWiFiConnection(const char *ssid, const char *password)
{
    WiFi.begin(ssid, password);

    //Wait until the connection has been stablished
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }

    WiFiConfigurator::ssid = ssid;
    WiFiConfigurator::password = password;
}

bool WiFiConfigurator::beginMDNSService(const char *hostname)
{
    if (WiFiConfigurator::mDNSHandler.begin(hostname))
    {
        return true;
    }
    return true;
}