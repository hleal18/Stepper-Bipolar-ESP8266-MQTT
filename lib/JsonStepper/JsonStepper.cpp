#include "JsonStepper.h"

JsonStepper::~JsonStepper()
{

}

JsonObject& JsonStepper::decode_json(const char* message)
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& input = jsonBuffer.parseObject(message);
  return input;
}

JsonObject& JsonStepper::decode_json(const byte* message)
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& input = jsonBuffer.parseObject(message);
  return input;
}

char* JsonStepper::encode_json(JsonObject& json_object)
{
  char json[100];
  char* json_ptr;
  json_object.printTo(json);
  json_ptr = json;

  return json_ptr;
}

JsonObject& JsonStepper::create_json_object()
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& object = jsonBuffer.createObject();
  return object;
}
