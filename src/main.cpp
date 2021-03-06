#include "OTAUploader.h"
#include "MQTTClient.h"
#include "Blackout.h"
#include "WiFiConfigurator.h"
#include "TelnetDebugger.h"

#define INSTEPPER "inStepper-01"
#define OUTSTEPPER "outStepper-01"
#define CLOCKWISE "clockwise"
#define COUNTERCLOCKWISE "counterclockwise"

const char *device_name = "stepper-01";
const char *ssid = "hola";
const char *password = "hola";
const char *mqtt_server = "192.168.0.34";
const int mqtt_port = 1883;
const char *dns = "stepper-01";
const char *accesspoint = "stepper-01";

const int steps_per_revolution = 200;
const int motor_speed = 50;

Blackout blind_roller(device_name, steps_per_revolution, motor_speed, 13, 12, 14, 16);
WiFiConfigurator configurator(ssid, password, dns, accesspoint);
MQTTClient client(mqtt_server, mqtt_port, INSTEPPER, OUTSTEPPER, Blackout::bindedHandleRoller(blind_roller));
OTAUploader uploader;

void setup()
{
    Serial.begin(115200);
    delay(15);
    configurator.initServices();
    delay(15);
    uploader.initService();
    delay(15);    
    Debugger.initService();
    delay(15);
}

void loop()
{
    uploader.handle();
    Debugger.handle();
    client.handle();
}