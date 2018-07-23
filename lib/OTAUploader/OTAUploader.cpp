#include "OTAUploader.h"

OTAUploader::OTAUploader()
{
    //Ligadura de metodos de clase como parametro para su correcto uso
    //Extraído de: https://stackoverflow.com/questions/7582546/using-generic-stdfunction-objects-with-member-functions-in-one-class
    ArduinoOTA.onStart(std::bind(&OTAUploader::handleStart, this));
    ArduinoOTA.onEnd(std::bind(&OTAUploader::handleEnd, this));
    ArduinoOTA.onProgress(std::bind(&OTAUploader::handleProgress, this, std::placeholders::_1, std::placeholders::_2));
    ArduinoOTA.onError(std::bind(&OTAUploader::handleError, this, std::placeholders::_1));
}

OTAUploader::~OTAUploader()
{
}

void OTAUploader::handleStart()
{
    Serial.println("Start");
}

void OTAUploader::handleEnd()
{
    Serial.println("\nEnd");
}

void OTAUploader::handleProgress(unsigned int progress, unsigned int total)
{
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
}

void OTAUploader::handleError(ota_error_t error)
{
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
        Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
        Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
        Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
        Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
        Serial.println("End Failed");
}

void OTAUploader::initService()
{
    ArduinoOTA.begin();
}

void OTAUploader::handle()
{
    ArduinoOTA.handle();
}