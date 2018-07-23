#include "OTAUploader.h"
#include "MQTTClient.h"
#include "Blackout.h"
#include "WiFiConfigurator.h"
#include "TelnetDebugger.h"

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

void callback(char *topic, byte *payload, unsigned int length);

Blackout blind_roller("stepper-01", stepsPerRevolution, motorSpeed, 13, 12, 14, 16);
WiFiConfigurator configurator(ssid, password, dns, accesspoint);

WiFiClient wificlient;
MQTTClient client(mqtt_server, port, INSTEPPER, OUTSTEPPER, wificlient, callback);

OTAUploader uploader;
TelnetDebugger debugger;

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

    debugger.initService();

    Serial.print("Free Heap[B]: ");
    Serial.println(ESP.getFreeHeap());
}

void loop()
{
    uploader.handle();
    debugger.handle();
    client.listen();
}