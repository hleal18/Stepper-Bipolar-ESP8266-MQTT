#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Stepper.h>
// Update these with values suitable for your network.

const char *ssid = "SEMARD";
const char *password = "SEMARD123";
const char *mqtt_server = "192.168.0.200";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

#define INSTEPPER "inStepper"
#define OUTSTEPPER "outStepper"

const int stepsPerRevolution = 200;
const int tip120 = 5;

Stepper myStepper(stepsPerRevolution, 13, 12, 14, 16);

void setup_wifi();
void callback(char *topic, byte *payload, unsigned int length);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1884);
  client.setCallback(callback);
  pinMode(tip120, OUTPUT);

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

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '+') {
    myStepper.step(stepsPerRevolution); // Turn the LED on (Note that LOW is the
                                        // voltage level
    client.publish(OUTSTEPPER, "Vuelta en sentido del reloj.");
    digitalWrite(tip120, HIGH);

  } else if ((char)payload[0] == '-') {
    myStepper.step(
        -stepsPerRevolution); // Turn the LED off by making the voltage HIGH
    client.publish(OUTSTEPPER, "Vuelta en sentido contrario al reloj.");
  }
  digitalWrite(tip120, LOW);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Pasito a pasito. Dale suavecito. Ba dum tss.", "semard",
                       "semard2017")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // ... and resubscribe
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
