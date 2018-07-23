#ifndef TelnetDebugger_h
#define TelnetDebugger_h

#include <ESP8266WiFi.h>

class TelnetDebugger
{
    private:
        WiFiServer telnetServer;
        WiFiClient telnetClient;
        bool message;
        unsigned long startTime;
    public:
        TelnetDebugger(unsigned int port = 23, bool message = false, unsigned long startTime = millis());
        ~TelnetDebugger();
        void handle();
        void initService();


};

#endif