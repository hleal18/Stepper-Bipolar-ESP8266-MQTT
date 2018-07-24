#include "MQTTClient.h"

MQTTClient::MQTTClient(const char *domain, uint16_t port, const char *inTopic, const char *outTopic, std::function<void(char *, uint8_t *, unsigned int)> callback)
    : domain(domain), port(port), inTopic(inTopic), outTopic(outTopic), wificlient(), client(domain, port, callback, wificlient)
{
}

MQTTClient::~MQTTClient()
{
}

void MQTTClient::handle()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();
}

void MQTTClient::reconnect()
{
    unsigned int attempt_count = 0;
    WiFiManager wifimanager;
    WiFiManagerParameter custom_mqtt_server("server", "mqtt_server", "0.0.0.0", 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt_port", "1883", 5);

    while (!client.connected() && attempt_count <= 10)
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("stepper-01", "hleal", "semard"))
        {
            Serial.println("connected");
            client.subscribe(inTopic);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
        attempt_count++;
    }
    if (attempt_count > 3)
    {
        Serial.println("Se superaron los intentos: iniciando AP");
        wifimanager.addParameter(&custom_mqtt_server);
        wifimanager.addParameter(&custom_mqtt_port);
        if (!wifimanager.startConfigPortal("stepper-01"))
        {
            Serial.println("Failed to connect and hit timeout");
            delay(3000);
            ESP.reset();
            delay(5000);
        }
        Serial.println("Se finalizó la configuración");
        Serial.print("Valor del server mqtt");
        Serial.println(custom_mqtt_server.getValue());
        Serial.print("Valor del puerto mqtt");
        Serial.println(custom_mqtt_port.getValue());
        domain = custom_mqtt_server.getValue();
        port = (int)custom_mqtt_port.getValue();
        client.setServer(domain, port);
    }
}

void MQTTClient::publish(const char *message)
{
    client.publish(outTopic, message);
}