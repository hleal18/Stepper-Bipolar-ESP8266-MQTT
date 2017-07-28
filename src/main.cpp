#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Stepper.h>
#include "JsonStepper.h"
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
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  myStepper.setSpeed(120);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
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
  write["progreso"] = porcentaje;
  write["estado"] = "girando";
  client.publish(OUTSTEPPER, jsonStepper.encode_json(write).c_str());
  do {
    myStepper.step(sentidoPasos);
    vueltasActual++;
    porcentaje = calcular_porcentaje(vueltasActual, vueltas);
    write["progreso"] = porcentaje;
    if((porcentaje%5) == 0){
      client.publish(OUTSTEPPER, jsonStepper.encode_json(write).c_str());
    }
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
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
