#include "WiFiConfigurator.h"

const char *WiFiConfigurator::str_status[] =
    {
        "WL_IDLE_STATUS",
        "WL_NO_SSID_AVAIL",
        "WL_SCAN_COMPLETED",
        "WL_CONNECTED",
        "WL_CONNECT_FAILED",
        "WL_CONNECTION_LOST",
        "WL_DISCONNECTED"};

const char *WiFiConfigurator::str_mode[] =
    {
        "WIFI_OFF",
        "WIFI_STA",
        "WIFI_AP",
        "WIFI_AP_STA"};

WiFiConfigurator::~WiFiConfigurator()
{
}

WiFiConfigurator::WiFiConfigurator(const char *ssid, const char *password, const char *hostname, const char *accesspoint)
    : ssid(ssid), password(password), hostname(hostname), accesspoint(accesspoint)
{
}

void WiFiConfigurator::initServices()
{
    int attempt_count = 0;
    WiFiManager manager;
    // WiFiManagerParameter custom_mqtt_server("server", "mqtt server", "192.168.0.34", 40);
    // WiFiManagerParameter custom_mqtt_port("port", "mqtt port", "1883", 5);
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

    while (attempt_count < 15 && WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        attempt_count++;
    }

    if (accesspoint && WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Conexión no lograda.");
        // manager.addParameter(&custom_mqtt_server);
        // manager.addParameter(&custom_mqtt_port);
        manager.setConnectTimeout(30);
        if (!manager.autoConnect(accesspoint))
        {
            Serial.println("failed to connect and hit timeout");
            delay(3000);
            // reset and try again, or maybe put it to deep sleep
            ESP.reset();
            delay(1000);
        }
    }

    Serial.println("Conectado a WiFi.");
    // Serial.print("Valores de servidor MQTT: ");
    // Serial.println(custom_mqtt_server.getValue());
    // Serial.print("Valor del puerto: ");
    // Serial.println(custom_mqtt_port.getValue());
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