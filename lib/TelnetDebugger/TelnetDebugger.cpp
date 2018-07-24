#include "TelnetDebugger.h"

TelnetDebugger::TelnetDebugger(unsigned int port, bool message, unsigned long startTime)
    : telnetServer(port), telnetClient(), message(message), startTime(startTime)
{
}

TelnetDebugger::~TelnetDebugger()
{
}

void TelnetDebugger::handle()
{
    if (telnetServer.hasClient())
    {
        if (!telnetClient || !telnetClient.connected())
        {
            if (telnetClient)
            {
                telnetClient.stop();
                Serial.println("Telnet Client Stop");
            }
            telnetClient = telnetServer.available();
            Serial.println("New Telnet client");
            telnetClient
                .flush(); // clear input buffer, else you get strange characters
        }
    }

    while (telnetClient.available())
    { // get data from Client
        Serial.write(telnetClient.read());
    }

    if (!message)
    { // run every 2000 ms
        startTime = millis();

        if (telnetClient && telnetClient.connected())
        { // send data to Client
            telnetClient.println("Debugger mode with Telnet");
            if (WiFi.status() == WL_CONNECTED)
            {
                telnetClient.println("Connected to: ");
                telnetClient.println(WiFi.localIP());
            }
            // if (client.connected())
            // {
            //   telnetClient.println("Conectado a MQTT");
            // }
            message = true;
        }
    }
    delay(10); // to avoid strange characters left in buffer
}

void TelnetDebugger::initService()
{
    telnetServer.begin();
    telnetServer.setNoDelay(true);
    Serial.println("Please connect Telnet Client, exit with ^] and 'quit'");
}

void TelnetDebugger::println(const char *message)
{
    telnetClient.println(message);
}

TelnetDebugger Debugger;