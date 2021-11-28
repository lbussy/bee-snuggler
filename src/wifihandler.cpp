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

#include "wifihandler.h"

bool shouldSaveConfig = false;
bool online = false;

Ticker blinker;
Ticker onlineTicker;

void doWiFi()
{
    doWiFi(false);
}

void doWiFi(const char * ssid, const char * password)
{
    blinker.attach_ms(STABLINK, wifiBlinker);
    Log.notice(F("Connecting on demand to %s.."), ssid);
    
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(config.snugconfig.hostname);
    long startTime = millis();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        _delay(200);
        Serial.print(".");
        if (millis() >= startTime + 10000)
        {
            Serial.println();
            Log.error(F("Unable to connect to %s." CR), ssid);
            blinker.attach_ms(CONFIGBLINK, wifiBlinker);
            while (true) ;
        }
    }
    Serial.println();

    Log.notice(F("Connected. IP address: %s." CR), WiFi.localIP().toString().c_str());
    config.snugconfig.nodrd = true;
    WiFi.onEvent(wifiEvent);
    blinker.detach();        // Turn off blinker
    digitalWrite(LED, HIGH); // Turn off LED
} 

void doWiFi(bool dontUseStoredCreds)
{
    DNSServer dnsServer;
    WiFiManager wm;

    // Set config portal channel, default = 1. Use 0 => random channel from 1-13
    wm.setWiFiAPChannel(13);

    // We can't use WiFi.SSID() in ESP32as it's only valid after connected.
    // SSID and Password stored in ESP32 wifi_ap_record_t and wifi_config_t are also cleared in reboot
    // Have to create a new function to store in EEPROM/SPIFFS for this purpose
    // Router_SSID = wm.WiFi_SSID();
    // Router_Pass = wm.WiFi_Pass();

    // Remove this line if you do not want to see WiFi password printed
    // Log.verbose(F("Stored: SSID = %s, Pass = %s" CR), Router_SSID.c_str(), Router_Pass.c_str());

    // AsyncWiFiManager Callbacks
    wm.setAPCallback(apCallback); // Called after AP has started
    // wm.setConfigResetCallback(configResetCallback); // Called after settings are reset
    // wm.setPreSaveConfigCallback(preSaveConfigCallback); // Called before saving wifi creds
    wm.setSaveConfigCallback(saveConfigCallback); //  Called only if wifi is saved/changed, or setBreakAfterConfig(true)
    // wm.setSaveParamsCallback(saveParamsCallback); // Called after parameters are saved via params menu or wifi config
    // wm.setWebServerCallback(webServerCallback); // Called after webserver is setup

#ifndef DISABLE_LOGGING
    if (Log.getLevel())
        wm.setDebugOutput(true); // Verbose debug is enabled by default
    else
        wm.setDebugOutput(false);
#else
    wm.setDebugOutput(false);
#endif

    // Allow non-default host name
    WiFiManagerParameter custom_hostname("name", "Host Name", config.snugconfig.hostname, 32);
    wm.addParameter(&custom_hostname);

    if (dontUseStoredCreds)
    {
        // Voluntary portal
        blinker.attach_ms(APBLINK, wifiBlinker);
        wm.setConfigPortalTimeout(120);

        if (wm.startConfigPortal(config.apconfig.ssid, config.apconfig.passphrase))
        {
            // We finished with portal, do we need this?
        }
        else
        {
            // Hit timeout on voluntary portal
            blinker.detach(); // Turn off blinker
            digitalWrite(LED, LOW);
            _delay(3000);
            digitalWrite(LED, HIGH);
            Log.notice(F("Hit timeout for on-demand portal, exiting." CR));
            resetController();
            _delay(1000);
        }
    }
    else
    {
        // Normal WiFi connection attempt
        blinker.attach_ms(STABLINK, wifiBlinker);
        wm.setConnectTimeout(30);
        wm.setConfigPortalTimeout(120);
        if (!wm.autoConnect(config.apconfig.ssid, config.apconfig.passphrase))
        {
            Log.warning(F("Failed to connect and/or hit timeout." CR));
            blinker.detach(); // Turn off blinker
            digitalWrite(LED, LOW);
            _delay(3000);
            digitalWrite(LED, HIGH);
            Log.warning(F("Restarting." CR));
            resetController();
            _delay(1000);
        }
        else
        {
            // We finished with portal (We were configured)
            blinker.detach();        // Turn off blinker
            digitalWrite(LED, HIGH); // Turn off LED
            WiFi.setHostname(config.snugconfig.hostname);
        }
    }

    if (shouldSaveConfig)
    { // Save configuration
        if (custom_hostname.getValue() != config.snugconfig.hostname)
        {
            Log.notice(F("Saving custom hostname configuration: %s." CR), custom_hostname.getValue());
            strlcpy(config.snugconfig.hostname, custom_hostname.getValue(), sizeof(config.snugconfig.hostname));
            WiFi.setHostname(config.snugconfig.hostname);
            config.snugconfig.nodrd = true;
            saveConfig();
            Log.notice(F("Restarting to pick up custom hostname." CR));
            ESP.restart();
        }
    }
    wm.disconnect();

    Log.notice(F("Connected. IP address: %s." CR), WiFi.localIP().toString().c_str());
    blinker.detach();        // Turn off blinker
    digitalWrite(LED, HIGH); // Turn off LED

    WiFi.onEvent(wifiEvent);
}

void resetWifi()
{
    // Wipe wifi settings and reset controller
    WiFi.disconnect(true, true);
    blinker.detach();       // Turn off blinker
    digitalWrite(LED, LOW); // Turn on LED
    Log.notice(F("Restarting after clearing wifi settings." CR));
    config.snugconfig.nodrd = true;
    saveConfig();
    ESP.restart();
}

void wifiBlinker()
{
    // Invert Current State of LED
    digitalWrite(LED, !(digitalRead(LED)));
}

// WiFiManager Callbacks

void apCallback(WiFiManager *wm)
{
    // Entered Access Point mode
    Log.verbose(F("[CALLBACK]: setAPCallback fired." CR));
#ifdef ESP32
    esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT20); // Set the bandwidth of ESP32 interface
#endif
    blinker.detach(); // Turn off blinker
    blinker.attach_ms(APBLINK, wifiBlinker);
    Log.notice(F("Entered portal mode; name: %s, IP: %s." CR),
               wm->getConfigPortalSSID().c_str(),
               WiFi.softAPIP().toString().c_str());
}

void configResetCallback() {
    Log.verbose(F("[CALLBACK]: setConfigResetCallback fired." CR));
}

void preSaveConfigCallback() {
    Log.verbose(F("[CALLBACK]: preSaveConfigCallback fired." CR));
}

void saveConfigCallback()
{
    Log.verbose(F("[CALLBACK]: setSaveConfigCallback fired." CR));
    shouldSaveConfig = true;
}

void saveParamsCallback()
{
    Log.verbose(F("[CALLBACK]: setSaveParamsCallback fired." CR));
}

void webServerCallback() {
    Log.verbose(F("[CALLBACK]: setWebServerCallback fired." CR));
}

void wifiEvent(WiFiEvent_t event)
{
    Serial.printf("[WiFi-event] event: %d\n", event);
    if (!WiFi.isConnected())
    {
        Log.warning(F("WiFi lost connection." CR));
        WiFi.begin();

        int WLcount = 0;
        while (!WiFi.isConnected() && WLcount < 190)
        {
            delay(100);
            printDot(true);
            ++WLcount;
        }

        if (!WiFi.isConnected())
        {
            // We failed to reconnect.
            Log.error(F("Unable to reconnect WiFI, restarting." CR));
            delay(1000);
            ESP.restart();
        }
    }
}

void checkOnline()
{
    IPAddress ip;
    if (ip.fromString("1.1.1.1"))
    {
        int pingResult = Ping.ping(ip);

        if (pingResult >= 0)
        {
            online = true;
            Log.verbose(F("[Online] Ok (code: %d)." CR), pingResult);
        }
        else
        {
            online = false;
            Log.verbose(F("[Online]] Failed (code: %d)." CR), pingResult);
        }
    }
    else
        online = false;
    
    onlineTicker.once(60, checkOnline);
}
