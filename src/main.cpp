#include "JsonStepper.h"
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include "MQTTClient.h"
#include <Stepper.h>
#include "WiFiConfigurator.h"
// Update these with values suitable for your network.

#define INSTEPPER "inStepper"
#define OUTSTEPPER "prrito"
#define CLOCKWISE "clockwise"
#define COUNTERCLOCKWISE "counterclockwise"

const char *ssid = "hola";
const char *password = "hola";
const char *mqtt_server = "192.168.0.34";
int port = 1883;
const char *dns = "stepper-01";
const char *accesspoint = "stepper-01";

boolean debug = false;
unsigned long startTime = millis();
const char *str_status[] = {"WL_IDLE_STATUS", "WL_NO_SSID_AVAIL",
                            "WL_SCAN_COMPLETED", "WL_CONNECTED",
                            "WL_CONNECT_FAILED", "WL_CONNECTION_LOST",
                            "WL_DISCONNECTED"};

// provide text for the WiFi mode
const char *str_mode[] = {"WIFI_OFF", "WIFI_STA", "WIFI_AP", "WIFI_AP_STA"};

void callback(char *topic, byte *payload, unsigned int length);
WiFiClient wificlient;
MQTTClient client(mqtt_server, port, INSTEPPER, OUTSTEPPER, wificlient, callback);

const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 13, 12, 14, 16);
//Servidor telnet
WiFiServer telnetServer(23);
WiFiClient serverClient;
//Objeto configurador para servicios conexión y administración a red 
//Wifi, wifimanager y mdns
WiFiConfigurator configurator(ssid, password, dns, accesspoint);

void callback(char *topic, byte *payload, unsigned int length);
int calcular_porcentaje(int &numerador, int &denominador);
void telnetHandle();

void setup()
{
  Serial.begin(115200);
  delay(15);
  configurator.initServices();
  delay(15);
  // client.setServer(mqtt_server, 1883);
  // client.setCallback(callback);
  myStepper.setSpeed(50);

  ArduinoOTA.onStart([]() { Serial.println("Start"); });
  ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  telnetServer.begin();
  telnetServer.setNoDelay(true);
  Serial.println("Please connect Telnet Client, exit with ^] and 'quit'");

  Serial.print("Free Heap[B]: ");
  Serial.println(ESP.getFreeHeap());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  int vueltas = 0, vueltasActual = 0;
  int sentidoPasos = stepsPerRevolution;
  String sentido = "";
  int porcentaje = ((vueltasActual * 100) / vueltas);

  JsonStepper jsonStepper;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &write = jsonBuffer.createObject();

  JsonObject &root = jsonStepper.decode_json(payload);
  vueltas = root["vueltas"].as<int>();
  sentido = root["sentido"].as<String>();

  if (!root.success())
  {
    serverClient.println(
        "Hay un error en la instrucción. Revise de nuevo el formato.");
    root.printTo(Serial);
    return;
  }

  if (sentido == CLOCKWISE)
  {
    write["sentido"] = CLOCKWISE;
  }
  else if (sentido == COUNTERCLOCKWISE)
  {
    sentidoPasos *= -1;
    write["sentido"] = COUNTERCLOCKWISE;
  }
  write["vueltas"] = vueltas;
  write["progreso"] = porcentaje;
  write["estado"] = "girando";
  client.publish(jsonStepper.encode_json(write).c_str());
  do
  {
    serverClient.println("Iniciando paso.");
    delay(15);
    myStepper.step(sentidoPasos);
    delay(15);
    serverClient.println("Terminando paso.");
    vueltasActual++;
    porcentaje = calcular_porcentaje(vueltasActual, vueltas);
    write["progreso"] = porcentaje;
    if ((porcentaje % 5) == 0)
    {
      client.publish(jsonStepper.encode_json(write).c_str());
    }
  } while (vueltasActual < vueltas);
  write["estado"] = "finalizado";
  serverClient.println(jsonStepper.encode_json(write));
  client.publish(jsonStepper.encode_json(write).c_str());
}

int calcular_porcentaje(int &numerador, int &denominador)
{
  return ((numerador * 100) / denominador);
}

void loop()
{
  ArduinoOTA.handle();
  telnetHandle();
  client.listen();
}
bool mensaje = false;
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
