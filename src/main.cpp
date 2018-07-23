#include "OTAUploader.h"
#include "MQTTClient.h"
#include "Blackout.h"
#include "WiFiConfigurator.h"
// Update these with values suitable for your network.

#define INSTEPPER "inStepper"
#define OUTSTEPPER "prrito"
#define CLOCKWISE "clockwise"
#define COUNTERCLOCKWISE "counterclockwise"

const char *ssid = "hola";
const char *password = "hola";
const char *mqtt_server = "192.168.0.34";
const int port = 1883;
const char *dns = "stepper-01";
const char *accesspoint = "stepper-01";

const int stepsPerRevolution = 200;
const int motorSpeed = 50;
unsigned long startTime = millis();
bool mensaje = false;

void callback(char *topic, byte *payload, unsigned int length);
void telnetHandle();

Blackout blind_roller("stepper-01", stepsPerRevolution, motorSpeed, 13, 12, 14, 16);
WiFiConfigurator configurator(ssid, password, dns, accesspoint);

WiFiClient wificlient;
MQTTClient client(mqtt_server, port, INSTEPPER, OUTSTEPPER, wificlient, callback);

OTAUploader uploader;

WiFiServer telnetServer(23);
WiFiClient serverClient;

void callback(char *topic, byte *payload, unsigned int length)
{
    blind_roller.handleRoller(topic, payload, length);
}

void setup()
{
    Serial.begin(115200);
    delay(15);
    configurator.initServices();
    delay(15);
    uploader.initService();

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    telnetServer.begin();
    telnetServer.setNoDelay(true);
    Serial.println("Please connect Telnet Client, exit with ^] and 'quit'");

    Serial.print("Free Heap[B]: ");
    Serial.println(ESP.getFreeHeap());
}

void loop()
{
    uploader.handle();
    telnetHandle();
    client.listen();
}

void telnetHandle()
{
    if (telnetServer.hasClient())
    {
        if (!serverClient || !serverClient.connected())
        {
            if (serverClient)
            {
                serverClient.stop();
                Serial.println("Telnet Client Stop");
            }
            serverClient = telnetServer.available();
            Serial.println("New Telnet client");
            serverClient
                .flush(); // clear input buffer, else you get strange characters
        }
    }

    while (serverClient.available())
    { // get data from Client
        Serial.write(serverClient.read());
    }

    if (!mensaje)
    { // run every 2000 ms
        startTime = millis();

        if (serverClient && serverClient.connected())
        { // send data to Client
            serverClient.println("Conectado por telnet. Sos re-groso che.");
            serverClient.println("Un saludo para los mortales.");
            if (WiFi.status() == WL_CONNECTED)
            {
                serverClient.println("Conectado a: ");
                serverClient.println(WiFi.localIP());
            }
            // if (client.connected())
            // {
            //   serverClient.println("Conectado a MQTT");
            // }
            mensaje = true;
        }
    }
    delay(10); // to avoid strange characters left in buffer
}
