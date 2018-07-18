#include "WiFiConfigurator.h"

WiFiConfigurator::~WiFiConfigurator()
{
}

void WiFiConfigurator::beginWiFiConnection(const char *ssid, const char *password)
{
    WiFi.begin(ssid, password);
    //Wait until the connection has been stablished
    delay(5000);
}

void WiFiConfigurator::beginWiFiConnection(const char *ssid, const char *password, const char *wifiManagerService)
{
    beginWiFiConnection(ssid, password);

    WiFiManager wifiManager;
    if (WiFi.status() == WL_DISCONNECTED)
    {
        Serial.println("Conexión no lograda.");
        if (!wifiManager.autoConnect(wifiManagerService))
        {
            Serial.println("failed to connect and hit timeout");
            // reset and try again, or maybe put it to deep sleep
            ESP.reset();
            delay(1000);
        }
    }
}

bool WiFiConfigurator::beginMDNSService(const char *hostname)
{
    if (WiFiConfigurator::mDNSHandler.begin(hostname))
    {
        return true;
    }
    return true;
}