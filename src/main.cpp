#include "JsonStepper.h"
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <Stepper.h>
#include <WiFiManager.h>
// Update these with values suitable for your network.

//Variables usadas para la conexión la conexión a internet y MQTT.
const char *ssid = "sc-5efa";
const char *password = "4P252YEL9CXF";
const char *mqtt_server = "m12.cloudmqtt.com";
const int  mqtt_port = 12489;
const char *mqtt_username = "xbvmyoxh";
const char *mqtt_password = "nJzjyl5r-7GD";
const char *dns = "stepper-01";

boolean debug = false;
unsigned long startTime = millis();
const char *str_status[] = {"WL_IDLE_STATUS",    "WL_NO_SSID_AVAIL",
                            "WL_SCAN_COMPLETED", "WL_CONNECTED",
                            "WL_CONNECT_FAILED", "WL_CONNECTION_LOST",
                            "WL_DISCONNECTED"};

//Los modos en los que puede establecerse la red WiFi.
const char *str_mode[] = {"WIFI_OFF", "WIFI_STA", "WIFI_AP", "WIFI_AP_STA"};

WiFiClient espClient;
PubSubClient client(espClient);
MDNSResponder mdns;
bool dnsConnection = false;

#define INSTEPPER "inStepper"
#define OUTSTEPPER "outStepper"
#define CLOCKWISE "clockwise"
#define COUNTERCLOCKWISE "counterclockwise"

const int stepsPerRevolution = 200;

Stepper myStepper(stepsPerRevolution, 13, 12, 14, 16);

void setup_wifi();
void callback(char *topic, byte *payload, unsigned int length);
int calcular_porcentaje(int &numerador, int &denominador);
void publicar_inactividad();

WiFiServer telnetServer(23);
WiFiClient serverClient;

//Guarda el último numero de vueltas que se realizaron.
int ultimaVuelta = 0;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  delay(15);
  if (mdns.begin(dns, WiFi.localIP())) {
    dnsConnection = true;
  }
  delay(15);
  //Se establece el servidor MQTT.
  client.setServer(mqtt_server, mqtt_port);
  //Se establece el método que se ejecuta cada vez que se reciban peticiones.
  client.setCallback(callback);
  myStepper.setSpeed(60);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi mode: ");
    Serial.println(str_mode[WiFi.getMode()]);
    Serial.print("Status: ");
    Serial.println(str_status[WiFi.status()]);
    // signal WiFi connect
    delay(300); // ms
  } else {
    Serial.println("");
    Serial.println("WiFi connect failed, push RESET button.");
  }

  telnetServer.begin();
  telnetServer.setNoDelay(true);
  Serial.println("Please connect Telnet Client, exit with ^] and 'quit'");

  Serial.print("Free Heap[B]: ");
  Serial.println(ESP.getFreeHeap());
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println("Booting");
  // WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // WiFi.begin(ssid, password);
  WiFiManager wifiManager;
  //Resetea los datos guardados del WiFi. Si se activa reinicia las credenciales.
  //wifiManager.resetSettings();

  if (!wifiManager.autoConnect("Prrito")) {
    Serial.println("failed to connect and hit timeout");
    // reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  // while (WiFi.status() != WL_CONNECTED) {
  //  delay(500);
  //  Serial.print(".");
  //}

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
  int porcentaje = 50;
  int porcentajeActual = 0;
  float parametro = 0;

  //Se usan para crear el JSON que se enviará.
  JsonStepper jsonStepper;
  StaticJsonBuffer<200> jsonWrite;
  JsonObject &write = jsonWrite.createObject();

  //Se decodifica el payload, el cual contiene información del JSON recibido con
  //Cada uno de sus campos.
  JsonObject &root = jsonStepper.decode_json(payload);
  vueltas = root["vueltas"].as<int>();
  sentido = root["sentido"].as<String>();

  //Se guardan las vueltas a realizar en el registro.
  int ultimaVuelta = vueltas;

  //Dependiendo de la variable recibida, gira en un sentido o en otro.
  if (sentido == CLOCKWISE) {
    write["sentido"] = CLOCKWISE;
  } else if (sentido == COUNTERCLOCKWISE) {
    sentidoPasos *= -1;
    write["sentido"] = COUNTERCLOCKWISE;
  }

  //Se completa el JSON a enviar.
  write["vueltas"] = vueltas;
  write["progreso"] = porcentajeActual;
  write["estado"] = "girando";
  //Se codifica y se publica el primer JSON informando el estado del motor.
  client.publish(OUTSTEPPER, jsonStepper.encode_json(write).c_str());
  parametro = vueltas*(porcentaje/100.0);
  do {
    myStepper.step(sentidoPasos*parametro);
    porcentajeActual += porcentaje;
    write["progreso"] = porcentajeActual;
    client.publish(OUTSTEPPER, jsonStepper.encode_json(write).c_str());
  } while (porcentajeActual != 100);
  write["estado"] = "finalizado";
  serverClient.println(jsonStepper.encode_json(write));
  client.publish(OUTSTEPPER, jsonStepper.encode_json(write).c_str());
}

//Se encarga de gestionar la conexión al servidor MQTT.
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Pasito a pasito. Dale suavecito. Ba dum tss.", mqtt_username,
                       mqtt_password)) {
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

int calcular_porcentaje(int &numerador, int &denominador) {
  return ((numerador * 100) / denominador);
}

int tiempoActual = millis();
int periodo = 10000;
void loop() {
  ArduinoOTA.handle();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if(millis() > tiempoActual + periodo){
    publicar_inactividad();
    tiempoActual = millis();
  }
}

void publicar_inactividad(){
  JsonStepper mensaje_inactividad;
  StaticJsonBuffer<200> jsonWrite;
  JsonObject &write = jsonWrite.createObject();
  write["estado"] = "Esperando orden";
  write["vueltas"] = ultimaVuelta;
  client.publish(OUTSTEPPER, mensaje_inactividad.encode_json(write).c_str());

}
