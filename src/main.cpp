/* Copyright (C) 2021 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Bee Snuggler (bee-snuggler).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#include "main.h"

DoubleResetDetector *drd;

Ticker getThatVersionTicker;
Ticker rebootTimer;
Ticker doControlTicker;

void setup()
{
    drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);

    // Set pins
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    pinMode(HEAT, OUTPUT);
    digitalWrite(HEAT, HIGH);

    if (!loadConfig())
    { // If configuration does not load, sit and blink slowly like an idiot
        Ticker blinker;
        blinker.attach_ms(CONFIGBLINK, []{digitalWrite(LED, !(digitalRead(LED)));});
        while (true) ;
    }

    serial();

    // Check if portal is requested
    if (!config.snugconfig.nodrd && drd->detectDoubleReset())
    {
        Log.notice(F("DRD: Portal requested." CR));
        doWiFi(true);
    }
    else
    {
        Log.verbose(F("Starting WiFi." CR));
        config.snugconfig.nodrd = false;
        if (STATICMODE) // STATICMODE for WiFI is turned on in PlatformIO.ini
        {
            // DEBUG:
            Log.verbose(F("Using static IP information." CR));
            const bool staticIP = false; // Set to use Static IP information
            const char * ssid = "Bohica";
            const char * password = "festivediamond378";
            IPAddress local_IP(192, 168, 168, 125);
            IPAddress gateway(192, 168, 168, 1);
            IPAddress subnet(255, 255, 255, 0);
            IPAddress primaryDNS(1, 1, 1, 1);   //optional
            IPAddress secondaryDNS(1, 0, 0, 1); //optional
            // DEBUG:
            WiFi.setHostname(config.snugconfig.hostname);
            if (staticIP)
            {
                if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
                    while (true) ; // Sit here and act stupid if that fails
            }
            doWiFi(ssid, password);
        }
        else
        {
            doWiFi();
        }
    }

    checkOnline();   // Start online tests
    if (online)
    {
        execspiffs();    // Check for pending SPIFFS update
        setClock();      // Set NTP Time
        mdnssetup();     // Set up mDNS responder
        doVersionPoll(); // Get server version at startup
    }

    initWebServer(); // Turn on web server

    // Setup tickers
    getThatVersionTicker.attach(POLLSERVERVERSION, doVersionPoll); // Poll for server version
    rebootTimer.attach(ESPREBOOT, setDoReset);                     // Reboot every 24 hours

    if (Log.getLevel())
        Log.notice(F("Started %s version %s/%s (%s) [%s]." CR), API_KEY, fw_version(), fs_version(), branch(), build());

    initChamberPID();   // Start temp processing
}

void loop()
{
    doOTALoop();
    maintLoop();
    serialLoop();
    drd->loop();
}
