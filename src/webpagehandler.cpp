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

#include "webpagehandler.h"

#if SAFEMODE == 1
#undef SPIFFSEDIT
#endif

AsyncWebServer server(HTTPPORT);

const char *wh_urlstart = URLSTART;
const char *wh_tld = TLD;
const char *wh_delim = DELIM;

// This BS is needed because a weird combo of libs and core is causing the
// methods to conflict (WiFiManager shit)
enum KC_METHODS
{
    BS_HTTP_GET = 0b00000001,
    BS_HTTP_POST = 0b00000010,
    BS_HTTP_DELETE = 0b00000100,
    BS_HTTP_PUT = 0b00001000,
    BS_HTTP_PATCH = 0b00010000,
    BS_HTTP_HEAD = 0b00100000,
    BS_HTTP_OPTIONS = 0b01000000,
    BS_HTTP_ANY = 0b01111111,
};

static HANDLER_STATE (*cf[])(AsyncWebServerRequest *) = { // Configuration functions
    handleControllerPost,
    handleControlPost,
    handleOutputPost,
    handleFanPost,
    handleSensorPost,
    handlePWMHeatPost,
    handlePWMFanPost};
static const char *cf_str[] = {
    "handleControllerPost",
    "handleControlPost",
    "handleOutputPost",
    "handleFanPost",
    "handleSensorPost",
    "handlePWMHeatPost",
    "handlePWMFanPost"};
static const int controlHandlers = sizeof(cf) / sizeof(*cf);

void initWebServer()
{
    setRegPageHandlers();
    setAPIPageHandlers();
    setActionPageHandlers();
    setInfoPageHandlers();
    setConfigurationPageHandlers();
    setEditor();

    // File not found handler
    server.onNotFound([](AsyncWebServerRequest *request)
                      {
                          if (request->method() == BS_HTTP_OPTIONS)
                          {
                              request->send(200);
                          }
                          else
                          {
                              Log.verbose(F("Serving 404 for request to %s." CR), request->url().c_str());
                              request->redirect("/404/");
                          }
                      });

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    server.begin();

    Log.notice(F("Async HTTP server started on port %l." CR), HTTPPORT);
    if (online)
        Log.verbose(F("Open: http://%s.local to view application." CR), WiFi.getHostname());
    else
        Log.verbose(F("Open: http://%s.local to view application." CR), WiFi.localIP().toString().c_str());
}

void setRegPageHandlers()
{
    // Regular page aliases

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm").setCacheControl("max-age=300");
    server.serveStatic("/index/", SPIFFS, "/").setDefaultFile("index.htm").setCacheControl("max-age=300");
    server.serveStatic("/about/", SPIFFS, "/").setDefaultFile("about.htm").setCacheControl("max-age=300");
    server.serveStatic("/temps/", SPIFFS, "/").setDefaultFile("temps.htm").setCacheControl("max-age=300");
    server.serveStatic("/help/", SPIFFS, "/").setDefaultFile("help.htm").setCacheControl("max-age=300");
    server.serveStatic("/ota1/", SPIFFS, "/").setDefaultFile("ota1.htm").setCacheControl("max-age=300");
    server.serveStatic("/ota2/", SPIFFS, "/").setDefaultFile("ota2.htm").setCacheControl("max-age=300");
    server.serveStatic("/mitetreat/", SPIFFS, "/").setDefaultFile("mitetreat.htm").setCacheControl("max-age=300");
    server.serveStatic("/settings/", SPIFFS, "/").setDefaultFile("settings.htm").setCacheControl("max-age=300");
    server.serveStatic("/controllerreset/", SPIFFS, "/").setDefaultFile("controllerreset.htm").setCacheControl("max-age=300");
    server.serveStatic("/wifireset/", SPIFFS, "/").setDefaultFile("wifireset.htm").setCacheControl("max-age=300");
    server.serveStatic("/offline/", SPIFFS, "/").setDefaultFile("offline.htm").setCacheControl("max-age=300");
    server.serveStatic("/disabled/", SPIFFS, "/").setDefaultFile("disabled.htm").setCacheControl("max-age=300");
    server.serveStatic("/notavail/", SPIFFS, "/").setDefaultFile("notavail.htm").setCacheControl("max-age=300");
    server.serveStatic("/404/", SPIFFS, "/").setDefaultFile("404.htm").setCacheControl("max-age=300");
}

void setAPIPageHandlers()
{
    server.on("/api/v1/", BS_HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  Log.verbose(F("Processing %s." CR), request->url().c_str());

                  // Serialize API
                  StaticJsonDocument<CAP_API> doc;
                  JsonObject root = doc.to<JsonObject>();
                  api.save(root);

                  // Serialize JSON to String
                  String api;
                  serializeJson(doc, api);

                  send_json(request, api);
              });

    server.on("/api/v1/action/", BS_HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  Log.verbose(F("Processing %s." CR), request->url().c_str());

                  // Serialize configuration
                  StaticJsonDocument<CAP_ACTION_API> doc;
                  JsonObject root = doc.to<JsonObject>();
                  api.actionAPI.save(root);

                  // Serialize JSON to String
                  String api;
                  serializeJson(doc, api);

                  send_json(request, api);
              });

    server.on("/api/v1/info/", BS_HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  Log.verbose(F("Processing %s." CR), request->url().c_str());

                  // Serialize configuration
                  StaticJsonDocument<CAP_INFO_API> doc;
                  JsonObject root = doc.to<JsonObject>();
                  api.infoAPI.save(root);

                  // Serialize JSON to String
                  String api;
                  serializeJson(doc, api);

                  send_json(request, api);
              });

    server.on("/api/v1/info/sensors/", BS_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
                  Log.verbose(F("Sending %s." CR), request->url().c_str());
                  HeatPWM *heatpwm = HeatPWM::getInstance();
                  FanPWM *fanpwm = FanPWM::getInstance();
                  StaticJsonDocument<CAPTEMPS> doc;

                  char title[132];
                  strlcpy(title, config.snugconfig.hivename, sizeof(title));
                  strlcat(title, " in ", sizeof(title));
                  strlcat(title, config.snugconfig.yardname, sizeof(title));
                  doc["title"] = title;
                  doc["imperial"] = config.snugconfig.imperial;
                  doc["controlenabled"] = config.temps.controlenabled;
                  doc["controlpoint"] = config.temps.controlpoint;
                  doc["setpoint"] = config.temps.setpoint;
                  doc["dotreatment"] = config.temps.dotreatment;
                  doc["treatpoint"] = config.temps.treatpoint;
                  doc["pwmheat"] = heatpwm->getDutyCyclePct();
                  doc["pwmfan"] = fanpwm->getDutyCyclePct();

                  int numEnabled = 0;
                  char *sensorName[NUMSENSOR];
                  double sensorAverage[NUMSENSOR];

                  for (int i = 0; i < NUMSENSOR; i++)
                  {
                      sensorName[i] = tempSensors.sensor[i].name;
                      doc["sensors"][i]["enable"] = config.temps.enabled[i];

                      if (config.temps.enabled[i])
                      {
                          numEnabled++;
                      }
                      sensorAverage[i] = tempSensors.sensor[i].average;
                  }

                  for (int i = 0; i < NUMSENSOR; i++)
                  {
                      doc["sensors"][i]["name"] = (const char *)sensorName[i];
                      doc["sensors"][i]["value"] = (const float)sensorAverage[i];
                  }

                  String json;
                  serializeJson(doc, json);
                  send_json(request, json);
              });
}

void setActionPageHandlers()
{
    // Action Page Handlers

    server.on("/api/v1/action/ping/", BS_HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  Log.verbose(F("Processing %s." CR), request->url().c_str());
                  request->send(200, F("text/plain"), F("Ok."));
              });

    server.on("/api/v1/action/wifireset/", BS_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
                  Log.verbose(F("Processing %s." CR), request->url().c_str());
                  _delay(2000);
                  if (!SAFEMODE)
                  {
                      setDoWiFiReset(); // Wipe settings, reset controller
                      request->send(200, F("text/plain"), F("Ok."));
                  }
                  else
                  {
                      request->send(SPIFFS, "/disabled.htm");
                  }
              });

    server.on("/api/v1/action/wifireset/", BS_HTTP_ANY, [](AsyncWebServerRequest *request)
              { send_not_allowed(request); });

    server.on("/api/v1/action/reset/", BS_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
                  Log.verbose(F("Processing %s." CR), request->url().c_str());
                  _delay(2000);
                  setDoReset();
                  request->send(200, F("text/plain"), F("Ok."));
              });

    server.on("/api/v1/action/reset/", BS_HTTP_ANY, [](AsyncWebServerRequest *request)
              { send_not_allowed(request); });

    server.on("/api/v1/action/updatestart/", BS_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
                  Log.verbose(F("Processing %s." CR), request->url().c_str());
                  if (!SAFEMODE)
                  {
                      setDoOTA(); // Trigger the OTA update
                      request->send(200, F("text/plain"), F("Ok."));
                  }
                  else
                  {
                      request->send(SPIFFS, "/disabled.htm");
                  }
              });

    server.on("/api/v1/action/updatestart/", BS_HTTP_ANY, [](AsyncWebServerRequest *request)
              { send_not_allowed(request); });

    server.on("/api/v1/action/clearupdate/", BS_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
                  Log.verbose(F("Processing %s." CR), request->url().c_str());
                  if (!SAFEMODE)
                  {
                      config.ota.dospiffs1 = false;
                      config.ota.dospiffs2 = false;
                      config.ota.didupdate = false;
                      config.snugconfig.nodrd = false;
                  }
                  request->send(200, F("text/plain"), F("Ok."));
              });

    server.on("/api/v1/action/clearupdate/", BS_HTTP_ANY, [](AsyncWebServerRequest *request)
              { send_not_allowed(request); });

    server.on("/api/v1/action/dotreatment/", BS_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
                  Log.verbose(F("Processing %s." CR), request->url().c_str());
                  // Loop through all parameters
                  int params = request->params();
                  for (int i = 0; i < params; i++)
                  {
                      AsyncWebParameter *p = request->getParam(i);
                      if (p->isPost())
                      {
                          // Process any p->name().c_str() / p->value().c_str() pairs
                          const char *name = p->name().c_str();
                          const char *value = p->value().c_str();

                          // Treatment settings
                          if (strcmp(name, "dotreatment") == 0)
                          {
                              if (strcmp(value, "true") == 0)
                              {
                                  Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                                  config.temps.dotreatment = true;
                              }
                              else if (strcmp(value, "false") == 0)
                              {
                                  Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                                  config.temps.dotreatment = false;
                              }
                              else
                              {
                                  Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                              }
                          }
                      }
                  }
                  request->send(200, F("text/plain"), F("Ok."));
              });

    server.on("/api/v1/action/dotreatment/", BS_HTTP_ANY, [](AsyncWebServerRequest *request)
              { send_not_allowed(request); });
}

void setInfoPageHandlers()
{
    // Info Page Handlers

    server.on("/api/v1/info/resetreason/", BS_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
                  // Used to provide the reset reason json
                  Log.verbose(F("Sending %s." CR), request->url().c_str());

                  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2);
                  StaticJsonDocument<capacity> doc;
                  JsonObject r = doc.createNestedObject("r");

                  r["reason"] = rstReason();
                  r["description"] = rstDescription();

                  String resetreason;
                  serializeJson(doc, resetreason);
                  send_json(request, resetreason);
              });

    server.on("/api/v1/info/heap/", BS_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
                  // Used to provide the heap json
                  Log.verbose(F("Sending %s." CR), request->url().c_str());

                  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3);
                  StaticJsonDocument<capacity> doc;
                  JsonObject h = doc.createNestedObject("h");

                  uint32_t free;
                  uint16_t max;
                  uint8_t frag;
                  multi_heap_info_t info;
                  heap_caps_get_info(&info, MALLOC_CAP_INTERNAL);
                  free = info.total_free_bytes;
                  max = info.largest_free_block;
                  frag = 100 - (max * 100) / free;

                  h["free"] = (const uint32_t)free;
                  h["max"] = (const uint16_t)max;
                  h["frag"] = (const uint8_t)frag;

                  String heap;
                  serializeJson(doc, heap);
                  send_json(request, heap);
              });

    server.on("/api/v1/info/uptime/", BS_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
                  // Used to provide the uptime json
                  Log.verbose(F("Sending %s." CR), request->url().c_str());

                  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(5);
                  StaticJsonDocument<capacity> doc;
                  JsonObject u = doc.createNestedObject("u");

                  const int days = uptimeDays();
                  const int hours = uptimeHours();
                  const int minutes = uptimeMinutes();
                  const int seconds = uptimeSeconds();
                  const int millis = uptimeMillis();

                  u["days"] = days;
                  u["hours"] = hours;
                  u["minutes"] = minutes;
                  u["seconds"] = seconds;
                  u["millis"] = millis;

                  String ut = "";
                  serializeJson(doc, ut);
                  send_json(request, ut);
              });

    server.on("/api/v1/info/thisVersion/", BS_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
                  Log.verbose(F("Sending %s." CR), request->url().c_str());
                  const size_t capacity = JSON_OBJECT_SIZE(4);
                  DynamicJsonDocument doc(capacity);

                  doc["fw_version"] = fw_version();
                  doc["fs_version"] = fs_version();

                  doc["branch"] = branch();
                  doc["build"] = build();

                  String json;
                  serializeJson(doc, json);
                  send_json(request, json);
              });

    server.on("/api/v1/info/thatVersion/", BS_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
                  Log.verbose(F("Sending %s." CR), request->url().c_str());
                  const size_t capacity = JSON_OBJECT_SIZE(2);
                  DynamicJsonDocument doc(capacity);

                  const char *fw_version = thatVersion.fw_version;
                  const char *fs_version = thatVersion.fs_version;
                  doc["fw_version"] = fw_version;
                  doc["fs_version"] = fs_version;

                  String json;
                  serializeJson(doc, json);
                  send_json(request, json);
              });

    server.on("/api/v1/info/treatment/", BS_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
                  Log.verbose(F("Sending %s." CR), request->url().c_str());
                  HeatPWM *heatpwm = HeatPWM::getInstance();
                  FanPWM *fanpwm = FanPWM::getInstance();
                  StaticJsonDocument<256> doc;

                  doc["dotreatment"] = config.temps.dotreatment;
                  doc["imperial"] = config.snugconfig.imperial;
                  doc["setpoint"] = config.temps.treatpoint;
                  doc["treatperiod"] = config.temps.treatperiod;
                  doc["coolperiod"] = config.temps.coolperiod;
                  doc["treatremaining"] = remainingTreatTime;
                  doc["coolremaining"] = remainingFanTime;
                  doc["controlpoint"] = tempSensors.sensor[config.temps.controlpoint].name;
                  doc["controlpointtemp"] = tempSensors.sensor[config.temps.controlpoint].average;
                  doc["outputtemp"] = tempSensors.sensor[4].average; // NOTE: This is a hard-coded array index
                  doc["pwmheat"] = heatpwm->getDutyCyclePct();
                  doc["pwmfan"] = fanpwm->getDutyCyclePct();
                  doc["treatstate"] = treatState();

                  String json;
                  serializeJson(doc, json);
                  send_json(request, json);
              });
}

void setConfigurationPageHandlers()
{
    // Settings Handlers:

    // server.on("/api/v1/config/settings/", BS_HTTP_PUT, [](AsyncWebServerRequest *request) {
    server.on("/api/v1/config/", BS_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
                  // Process settings update
                  Log.verbose(F("Processing put to %s." CR), request->url().c_str());

                  HANDLER_STATE state = NOT_PROCESSED;
                  for (int i = 0; i < controlHandlers; i++)
                  {
                      if (state == FAIL_PROCESS)
                          break;
                      Log.verbose(F("Checking %s." CR), cf_str[i]);
                      HANDLER_STATE thisState = cf[i](request);
                      if (thisState == PROCESSED)
                      {
                          request->send(200, F("text/plain"), F("Ok"));
                          state = PROCESSED;
                      }
                      else if (thisState == FAIL_PROCESS)
                      {
                          request->send(500, F("text/plain"), F("Unable to process data"));
                          state = FAIL_PROCESS;
                      }
                  }
              });

    //server.on("/api/v1/config/settings/", BS_HTTP_GET, [](AsyncWebServerRequest *request) {
    server.on("/api/v1/config/", BS_HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  // Used to provide the Config json
                  // Serialize configuration
                  StaticJsonDocument<CAP_SER_CONF> doc;   // Create doc
                  JsonObject root = doc.to<JsonObject>(); // Create JSON object
                  config.save(root);                      // Fill the object with current config

                  String json;
                  serializeJson(doc, json); // Serialize JSON to String
                  send_json(request, json);
              });

    //server.on("/api/v1/config/settings/", BS_HTTP_ANY, [](AsyncWebServerRequest *request) {
    server.on("/api/v1/config/", BS_HTTP_ANY, [](AsyncWebServerRequest *request)
              { send_not_allowed(request); });

    // Settings Handlers^
}

void setEditor()
{
#ifdef SPIFFSEDIT
    // Setup SPIFFS editor
    server.addHandler(new SPIFFSEditor(SPIFFS, SPIFFSEDITUSER, SPIFFSEDITPW));
    server.on("/edit/", BS_HTTP_GET, [](AsyncWebServerRequest *request)
              { request->redirect("/edit"); });
#endif
}

void stopWebServer()
{
    server.reset();
    server.end();
    Log.notice(F("Web server stopped." CR));
}

// Settings Handlers:

HANDLER_STATE handleControllerPost(AsyncWebServerRequest *request) // Handle controller settings
{
    bool didChange = false;
    bool hostnamechanged = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            // Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Controller settings
            //
            if (strcmp(name, "hostname") == 0) // Set hostname
            {
                if ((strlen(value) < 3) || (strlen(value) > 32))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    if (!strcmp(config.snugconfig.hostname, value) == 0)
                    {
                        if (!SAFEMODE)
                        {
                            Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                            hostnamechanged = true;
                            strlcpy(config.snugconfig.hostname, value, sizeof(config.snugconfig.hostname));
                            didChange = true;
                        }
                        else
                        {
                            Log.notice(F("Settings update, [%s]:(%s) ignored (in SAFE mode)." CR), name, value);
                        }
                    }
                }
            }
            if (strcmp(name, "yardname") == 0) // Set brewery name
            {
                if ((strlen(value) < 1) || (strlen(value) > 64))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.snugconfig.yardname, value, sizeof(config.snugconfig.yardname));
                }
            }
            if (strcmp(name, "hivename") == 0) // Set brewery name
            {
                if ((strlen(value) < 1) || (strlen(value) > 64))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.snugconfig.hivename, value, sizeof(config.snugconfig.hivename));
                }
            }
            if (strcmp(name, "imperial") == 0) // Set units
            {
                if (strcmp(value, "true") == 0)
                {
                    config.snugconfig.imperial = true;
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else if (strcmp(value, "false") == 0)
                {
                    config.snugconfig.imperial = false;
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
        if (hostnamechanged)
        { // We reset hostname, process
            hostnamechanged = false;
            tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, config.snugconfig.hostname);
            mdnsreset();
            Log.verbose(F("POSTed new mDNSid, reset mDNS stack." CR));
        }
    }
    if (saveConfig())
    {
        if (didChange)
        {
            return PROCESSED;
        }
        else
        {
            return NOT_PROCESSED;
        }
    }
    else
    {
        Log.error(F("Error: Unable to save controller configuration data." CR));
        return FAIL_PROCESS;
    }
}

HANDLER_STATE handleControlPost(AsyncWebServerRequest *request) // Handle sensor control settings
{
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();

            // Control settings
            //
            if (strcmp(name, "setpoint") == 0) // Set the setpoint
            {
                didChange = true;
                const double val = atof(value);
                if ((val < 0) || (val > 110))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.setpoint = val;
                }
            }
            if (strcmp(name, "enablecontrol") == 0) // Enable temp control
            {
                didChange = true;
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.controlenabled = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.controlenabled = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "controlpoint") == 0) // Choose control point
            {
                didChange = true;
                const int val = atof(value);
                if ((val < 0) || (val > NUMSENSOR - 1))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.controlpoint = val;
                }
            }
            if (strcmp(name, "treatpoint") == 0) // Set the mite treatment setpoint
            {
                didChange = true;
                const double val = atof(value);
                if ((val < 94) || (val > 112))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.treatpoint = val;
                }
            }
            if (strcmp(name, "treatperiod") == 0) // Set the mite treatment duration
            {
                didChange = true;
                const unsigned int val = atof(value);
                if ((val < 15) || (val > 300))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.treatperiod = val;
                }
            }
            if (strcmp(name, "coolperiod") == 0) // Set the cooldown duration
            {
                didChange = true;
                const unsigned int val = atof(value);
                if ((val < 1) || (val > 15))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.coolperiod = val;
                }
            }
        }
    }
    if (saveConfig())
    {
        if (didChange)
        {
            return PROCESSED;
        }
        else
        {
            return NOT_PROCESSED;
        }
    }
    else
    {
        Log.error(F("Error: Unable to save temp control configuration data." CR));
        return FAIL_PROCESS;
    }
}

HANDLER_STATE handleOutputPost(AsyncWebServerRequest *request) // Handle output air control settings
{
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();

            // Output Air settings
            //
            if (strcmp(name, "outputmax") == 0) // Set the output maximum temp
            {
                didChange = true;
                const double val = atof(value);
                if ((val < 0) || (val > 130))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.outputmax = val;
                }
            }
            if (strcmp(name, "enablemaxout") == 0) // Enable max temp control
            {
                didChange = true;
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enablemaxout = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enablemaxout = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
    }
    if (saveConfig())
    {
        if (didChange)
        {
            return PROCESSED;
        }
        else
        {
            return NOT_PROCESSED;
        }
    }
    else
    {
        Log.error(F("Error: Unable to save temp control configuration data." CR));
        return FAIL_PROCESS;
    }
}

HANDLER_STATE handleFanPost(AsyncWebServerRequest *request) // Handle fan settings
{
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();

            // Fan settings
            //
            if (strcmp(name, "enablefancontrol") == 0) // Enable fan control
            {
                didChange = true;
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.snugconfig.fancontrol = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.snugconfig.fancontrol = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "proportionalfan") == 0) // Enable max temp control
            {
                didChange = true;
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.snugconfig.propfan = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.snugconfig.propfan = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "fanrangehigh") == 0) // Fan speed upper boundary
            {
                didChange = true;
                const int val = atof(value);
                if ((val < 0) || (val > 100))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.snugconfig.fanrangehigh = val;
                }
            }
            if (strcmp(name, "fanrangelow") == 0) // Fan speed lower boundary
            {
                didChange = true;
                const int val = atof(value);
                if ((val < 0) || (val > 100))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.snugconfig.fanrangelow = val;
                }
            }
        }
    }
    if (saveConfig())
    {
        if (didChange)
        {
            return PROCESSED;
        }
        else
        {
            return NOT_PROCESSED;
        }
    }
    else
    {
        Log.error(F("Error: Unable to save temp control configuration data." CR));
        return FAIL_PROCESS;
    }
}

HANDLER_STATE handleSensorPost(AsyncWebServerRequest *request) // Handle sensor control settings
{
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            //Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Sensor settings
            //
            if (strcmp(name, "calambient") == 0) // Set the sensor calibration
            {
                didChange = true;
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.calibration[0] = val;
                }
            }
            if (strcmp(name, "enableambient") == 0) // Enable sensor
            {
                didChange = true;
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[0] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[0] = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "calupper") == 0) // Set the sensor calibration
            {
                didChange = true;
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.calibration[1] = val;
                }
            }
            if (strcmp(name, "enableupper") == 0) // Enable sensor
            {
                didChange = true;
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[1] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[1] = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "calcenter") == 0) // Set the sensor calibration
            {
                didChange = true;
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.calibration[2] = val;
                }
            }
            if (strcmp(name, "enablecenter") == 0) // Enable sensor
            {
                didChange = true;
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[2] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[2] = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "callower") == 0) // Set the sensor calibration
            {
                didChange = true;
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.calibration[3] = val;
                }
            }
            if (strcmp(name, "enablelower") == 0) // Enable sensor
            {
                didChange = true;
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[3] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[3] = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "caloutput") == 0) // Set the sensor calibration
            {
                didChange = true;
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.calibration[4] = val;
                }
            }
            if (strcmp(name, "enableoutput") == 0) // Enable sensor
            {
                didChange = true;
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[4] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[4] = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
    }
    if (saveConfig())
    {
        if (didChange)
        {
            return PROCESSED;
        }
        else
        {
            return NOT_PROCESSED;
        }
    }
    else
    {
        Log.error(F("Error: Unable to save sensor configuration data." CR));
        return FAIL_PROCESS;
    }
}

HANDLER_STATE handlePWMHeatPost(AsyncWebServerRequest *request) // Handle PWM Heat Setting
{
    bool didChange = false;
    HeatPWM *heatpwm = HeatPWM::getInstance();
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();

            // HeatPWM settings
            //
            if (strcmp(name, "pwmheat") == 0) // Set the HeatPWM value
            {
                didChange = true;
                const double val = atof(value);
                if ((val < -0) || (val > 100))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    setControlOff();
                    heatpwm->setDutyCyclePct(val);
                }
            }
        }
    }
    if (saveConfig())
    {
        if (didChange)
        {
            return PROCESSED;
        }
        else
        {
            return NOT_PROCESSED;
        }
    }
    else
    {
        Log.error(F("Error: Unable to save heat PWM data." CR));
        return FAIL_PROCESS;
    }
}

HANDLER_STATE handlePWMFanPost(AsyncWebServerRequest *request) // Handle PWM Fan setting
{
    bool didChange = false;
    FanPWM *fanpwm = FanPWM::getInstance();
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();

            // FanPWM settings
            //
            if (strcmp(name, "pwmfan") == 0) // Set the FanPWM value
            {
                didChange = true;
                const double val = atof(value);
                if ((val < -0) || (val > 100))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    setControlOff();
                    fanpwm->setDutyCyclePct(val);
                }
            }
        }
    }
    if (saveConfig())
    {
        if (didChange)
        {
            return PROCESSED;
        }
        else
        {
            return NOT_PROCESSED;
        }
    }
    else
    {
        Log.error(F("Error: Unable to save heat PWM data." CR));
        return FAIL_PROCESS;
    }
}

// Settings Handlers^

void send_not_allowed(AsyncWebServerRequest *request)
{
    Log.verbose(F("Not processing %s." CR), request->url().c_str());
    request->header("Cache-Control: no-store");
    request->send(405, F("text/plain"), F("Method Not Allowed"));
}

void send_json(AsyncWebServerRequest *request, String &json)
{
    Log.verbose(F("Adding JSON for %s." CR), request->url().c_str());
    request->header("Cache-Control: no-store");
    request->send(200, F("application/json"), json);
}
