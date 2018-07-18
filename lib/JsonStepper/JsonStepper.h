#ifndef JsonStepper_h
#define JsonStepper_h

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

class JsonStepper
{
private:
public:
  ~JsonStepper();
  JsonObject &decode_json(const char *message);
  JsonObject &decode_json(const byte *message);
  String encode_json(JsonObject &json_object);
  JsonObject &create_json_object();
};

#endif
