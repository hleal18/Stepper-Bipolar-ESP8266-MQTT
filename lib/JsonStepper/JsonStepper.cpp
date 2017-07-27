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

String JsonStepper::encode_json(JsonObject& json_object)
{
  String json;
  json_object.printTo(json);
  return json;
}

JsonObject& JsonStepper::create_json_object()
{
  StaticJsonBuffer<200> jsonBuffer;
  return jsonBuffer.createObject();
}
