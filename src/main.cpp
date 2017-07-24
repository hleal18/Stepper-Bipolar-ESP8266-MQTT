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
long lastMsg = 0;
char msg[] = "hola";
int value = 0;

#define INSTEPPER "inStepper"
#define OUTSTEPPER "outStepper"

const int stepsPerRevolution = 200;

Stepper myStepper(stepsPerRevolution, 13, 12, 14, 16);

void setup_wifi();
void callback(char *topic, byte *payload, unsigned int length);

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
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  char message[100];
  int index = 0;

  for (int i = 0; i < length; i++) {
    message[i] = (char)*payload;
    payload++;
  }
  JsonObject &root = jsonRead.parseObject(message);
  // read = jsonRead.parseObject(message);
  int vueltas = 0;
  String sentido = "";
  vueltas = root["vueltas"].as<int>();
  sentido = root["sentido"].as<String>();
  if (root.success()) {
    root.printTo(Serial);
  } else {
    Serial.println("No se reconoció el JSON");
  }
  // Switch on the LED if an 1 was received as first character
  if (sentido == "clockwise") {
    do {
      myStepper.step(
          stepsPerRevolution); // Turn the LED on (Note that LOW is the
      vueltas--;
    } while (vueltas > 0);
    client.publish(OUTSTEPPER, "Vuelta en sentido del reloj.");
    write["vueltas"] = 1;
    write["sentido"] = "clockwise";
    write.printTo(Serial);
  } else if (sentido == "counterclockwise") {
    do {
      myStepper.step(
          -stepsPerRevolution); // Turn the LED on (Note that LOW is the
      vueltas--;
    } while (vueltas > 0);
    client.publish(OUTSTEPPER, "Vuelta en sentido contrario al reloj.");
    write["vueltas"] = 1;
    write["sentido"] = "counterclockwise";
    write.printTo(Serial);
  }
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
