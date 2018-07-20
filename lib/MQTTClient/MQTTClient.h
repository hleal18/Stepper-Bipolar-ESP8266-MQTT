#ifndef MQTTClient_h

#define MQTTClient_h

#include <PubSubClient.h>
#include <WiFiClient.h>

class MQTTClient
{
  private:
    const char *domain;
    uint16_t port;
    const char *inTopic;
    const char *outTopic;
    PubSubClient client;

  public:
    ~MQTTClient();
    MQTTClient(const char *domain, uint16_t port, const char *inTopic, const char *outTopic,WiFiClient wificlient, std::function<void(char *, uint8_t *, unsigned int)> callback);
    void listen();
    void reconnect();
    void publish(const char* message);
};

#endif