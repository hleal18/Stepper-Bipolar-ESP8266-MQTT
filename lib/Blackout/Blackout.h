#ifndef Blackout_h

#define Blackout_h

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Stepper.h>
#include "JsonStepper.h"

class Blackout
{
  private:
    const char *name;
    Stepper stepper;
    const int stepsPerRevolution;
    const int speed;

  public:
    static const char *COUNTERCLOCKWISE;
    static const char *CLOCKWISE;

    ~Blackout();
    Blackout(const char *name, const int stepsPerRevolution, const int speed, int pin1, int pin2, int pin3, int pin4);
    void handleRoller(char *topic, byte *payload, unsigned int length);
};

#endif