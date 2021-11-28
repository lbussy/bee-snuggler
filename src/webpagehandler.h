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

#pragma once
#ifndef _WEBPAGEHANDLER_H
#define _WEBPAGEHANDLER_H

#include "uptime.h"
#include "jsonconfig.h"
#include "version.h"
#include "config.h"
#include "tools.h"
#include "thatVersion.h"
#include "execota.h"
#include "mdnshandler.h"
#include "resetreasons.h"
#include "api.h"
#include "tempsensors.h"
#include "pwmheat.h"
#include "pwmfan.h"
#include "pidchambertemp.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoLog.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <SPIFFSEditor.h>
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

enum HANDLER_STATE
{
    NOT_PROCESSED = -1,
    FAIL_PROCESS,
    PROCESSED
};

void initWebServer();
void setRegPageHandlers();
void setAPIPageHandlers();
void setActionPageHandlers();
void setInfoPageHandlers();
void setConfigurationPageHandlers();
void setEditor();
void stopWebServer();

HANDLER_STATE handleControllerPost(AsyncWebServerRequest *);
HANDLER_STATE handleControlPost(AsyncWebServerRequest *);
HANDLER_STATE handleOutputPost(AsyncWebServerRequest *);
HANDLER_STATE handleFanPost(AsyncWebServerRequest *);
HANDLER_STATE handleSensorPost(AsyncWebServerRequest *);
HANDLER_STATE handlePWMHeatPost(AsyncWebServerRequest *);
HANDLER_STATE handlePWMFanPost(AsyncWebServerRequest *);

void send_not_allowed(AsyncWebServerRequest *request);
void send_json(AsyncWebServerRequest *request, String &json);

extern struct ThatVersion thatVersion;
extern struct Config config;
extern struct API api;
extern unsigned long remainingTreatTime;
extern unsigned long remainingFanTime;

#endif // _WEBPAGEHANDLER_H
