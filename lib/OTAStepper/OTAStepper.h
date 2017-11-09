#ifndef OTAStepper_h

#define OTAStepper_h

#include <ArduinoOTA.h>
#include "ESP8266WiFi.h"

class OTAStepper{
private:

public:
  ~OTAStepper();
  void handle();
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_OTASTEPPER)
extern OTAStepper OTAStepper;
#endif

#endif
