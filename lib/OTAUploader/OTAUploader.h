#ifndef OTAUploader_h

#define OTAUploader_h

#include <ArduinoOTA.h>

class OTAUploader
{
    private:

    public:
        OTAUploader();
        ~OTAUploader();
        void handleStart();
        void handleEnd();
        void handleProgress(unsigned int progress, unsigned int total);
        void handleError(ota_error_t error);
        void initService();
        void handle();
};


#endif 