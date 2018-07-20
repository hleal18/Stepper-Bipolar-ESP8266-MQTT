#include "MQTTClient.h"

MQTTClient::MQTTClient(const char *domain, uint16_t port, const char *inTopic, const char *outTopic, WiFiClient wificlient, std::function<void(char *, uint8_t *, unsigned int)> callback)
    : domain(domain), port(port), inTopic(inTopic), outTopic(outTopic), client(wificlient)
{
    client.setServer(domain, port);
    client.subscribe(inTopic);
    client.setCallback(callback);
}

MQTTClient::~MQTTClient()
{
}

void MQTTClient::listen()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();
}

void MQTTClient::reconnect()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("Pasito a pasito. Dale suavecito. Ba dum tss.", "semard",
                           "semard2017"))
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
    }
}

void MQTTClient::publish(const char *message)
{
    client.publish(outTopic, message);
}