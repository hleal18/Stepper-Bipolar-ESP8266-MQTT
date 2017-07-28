#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Stepper.h>
#include "JsonStepper.h"
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
// Update these with values suitable for your network.

const char *ssid = "SEMARD";
const char *password = "SEMARD123";
const char *mqtt_server = "192.168.0.200";

WiFiClient espClient;
PubSubClient client(espClient);

#define INSTEPPER "inStepper"
#define OUTSTEPPER "outStepper"
#define CLOCKWISE "clockwise"
#define COUNTERCLOCKWISE "counterclockwise"

const int stepsPerRevolution = 200;

Stepper myStepper(stepsPerRevolution, 13, 12, 14, 16);

void setup_wifi();
void callback(char *topic, byte *payload, unsigned int length);
int calcular_porcentaje(int numerador, int denominador);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  delay(15);
  MDNS.begin("humberto");
  delay(15);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  myStepper.setSpeed(120);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length) {
  int vueltas = 0, index = 0, vueltasActual = 0;
  int sentidoPasos = stepsPerRevolution;
  char json[100];
  String sentido = "";
  char message[100];
  int porcentaje = ((vueltasActual*100)/vueltas);

  JsonStepper jsonStepper;
  StaticJsonBuffer<200> jsonWrite;
  JsonObject &write = jsonWrite.createObject();

  JsonObject &root = jsonStepper.decode_json(payload);
  vueltas = root["vueltas"].as<int>();
  sentido = root["sentido"].as<String>();

  if (!root.success()) {
    Serial.println(
        "Hay un error en la instrucción. Revise de nuevo el formato.");
    root.printTo(Serial);
    return;
  }

  if (sentido == CLOCKWISE) {
    write["sentido"] = CLOCKWISE;
  } else if (sentido == COUNTERCLOCKWISE) {
    sentidoPasos *= -1;
    write["sentido"] = COUNTERCLOCKWISE;
  }
  write["vueltas"] = vueltas;
  String numero = ((String)porcentaje) + "%";
  write["progreso"] = numero;
  write["estado"] = "girando";
  client.publish(OUTSTEPPER, jsonStepper.encode_json(write).c_str());
  do {
    myStepper.step(sentidoPasos);
    vueltasActual++;
    porcentaje = calcular_porcentaje(vueltasActual, vueltas);
    numero = ((String)porcentaje) + "%";
    write["progreso"] = numero;
    write["ota"] = "subido con ota";
    if((porcentaje%5) == 0){
      client.publish(OUTSTEPPER, jsonStepper.encode_json(write).c_str());
    }
    numero = "";
  } while (vueltasActual < vueltas);
  write["estado"] = "finalizado";
  Serial.println(jsonStepper.encode_json(write));
  client.publish(OUTSTEPPER, jsonStepper.encode_json(write).c_str());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Pasito a pasito. Dale suavecito. Ba dum tss.", "semard",
                       "semard2017")) {
      Serial.println("connected");
      client.subscribe(INSTEPPER);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

int calcular_porcentaje(int numerador, int denominador){
  return ((numerador*100)/denominador);
}

void loop() {
  ArduinoOTA.handle();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
