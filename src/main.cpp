#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Stepper.h>
// Update these with values suitable for your network.

const char *ssid = "SEMARD";
const char *password = "SEMARD123";
const char *mqtt_server = "192.168.0.200";

StaticJsonBuffer<200> jsonWrite;
StaticJsonBuffer<200> jsonRead;
JsonObject &read = jsonRead.createObject();
JsonObject &write = jsonWrite.createObject();

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

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  ESP.wdtDisable();
  ESP.wdtEnable(80);
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
  int vueltas = 0, index = 0;
  char json[100];
  String sentido = "";
  char message[100];

  StaticJsonBuffer<200> jsonBuffer;

  for (int i = 0; i < length; i++) {
    message[i] = (char)*payload;
    payload++;
  }

  JsonObject &root = jsonBuffer.parseObject(message);

  vueltas = root["vueltas"].as<int>();
  sentido = root["sentido"].as<String>();

  if (!root.success()) {
    Serial.println(
        "Hay un error en la instrucción. Revise de nuevo el formato.");
    root.printTo(Serial);
  }

  if (sentido == CLOCKWISE) {
    do {
      myStepper.step(stepsPerRevolution);
      vueltas--;
    } while (vueltas > 0);
    write["vueltas"] = 1;
    write["sentido"] = CLOCKWISE;
  } else if (sentido == COUNTERCLOCKWISE) {
    do {
      myStepper.step(-stepsPerRevolution);
      vueltas--;
    } while (vueltas > 0);
    write["vueltas"] = 1;
    write["sentido"] = COUNTERCLOCKWISE;
  }

  write.printTo(json);
  Serial.println("Mensaje");
  Serial.println(json);
  client.publish(OUTSTEPPER, json);
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

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
