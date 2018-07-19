#include "WiFiConfigurator.h"

WiFiConfigurator::~WiFiConfigurator()
{
}

WiFiConfigurator::WiFiConfigurator(const char *ssid, const char *password, const char *hostname, const char *accesspoint)
    : ssid(ssid), password(password), hostname(hostname), accesspoint(accesspoint)
{
}

int WiFiConfigurator::initServices()
{
    int attempts_count = 0;
    //Conexión con ssid pero sin password
    if (ssid && !password)
    {
        WiFi.begin(ssid);
    }
    //Conexion con ssid y password
    else if (ssid && password)
    {
        WiFi.begin(ssid, password);
    }

    while (attempts_count < 15 && WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        attempts_count++;
    }

    if (accesspoint && WiFi.status() != WL_CONNECTED)
    {
        WiFiManager manager;
        Serial.println("Conexión no lograda.");
        if (!manager.autoConnect(accesspoint))
        {
            Serial.println("failed to connect and hit timeout");
            // reset and try again, or maybe put it to deep sleep
            ESP.reset();
            delay(1000);
        }
    }

    //Servicio mMDNS
    if (hostname)
    {
        if (mDNSHandler.begin(hostname))
        {
            Serial.println("DNS iniciado");
        }
        else
        {
            Serial.println("DNS NO iniciado");
        }
    }
}