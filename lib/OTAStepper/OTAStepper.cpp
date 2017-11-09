#include "OTAStepper.h"

OTAStepper::~OTAStepper(){

}

void OTAStepper::handle(){
  ArduinoOTA.handle();
}
