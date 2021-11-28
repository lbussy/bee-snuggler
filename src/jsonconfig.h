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
#ifndef _JSONCONFIG_H
#define _JSONCONFIG_H

#include "serialhandler.h"
#include "config.h"
#include "tempsensors.h"

#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <FS.h>
#include <Arduino.h>

#define CAP_SER_CONF 1024
#define CAP_DESER_CONF 1536

struct ApConfig
{
    // Stores Access Point configuration
    char ssid[32];
    char passphrase[64];

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct SnugConfig
{
    // Stores Bee Snuggler configuration
    char guid[17];
    char hostname[32];
    bool nodrd;
    char yardname[64];
    char hivename[64];
    bool imperial;
    bool fancontrol;
    bool propfan;
    int fanrangehigh;
    int fanrangelow;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct OTA
{
    // Stores OTA data
    bool dospiffs1;
    bool dospiffs2;
    bool didupdate;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct Temperatures
{
    // Stores Temp Probe configuration
    float setpoint;
    int controlpoint;
    bool controlenabled;
    float outputmax;
    bool enablemaxout;
    float treatpoint;
    unsigned int treatperiod;
    unsigned int coolperiod;
    bool dotreatment;
    bool enabled[NUMSENSOR];
    float calibration[NUMSENSOR];

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct Config
{
    // Stores the complete configuration
    ApConfig apconfig;
    SnugConfig snugconfig;
    OTA ota;
    Temperatures temps;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

// Config handling
bool deleteConfigFile();
bool loadConfig();
bool saveConfig();
bool loadFile();
bool saveFile();
bool printConfig();
bool printFile();
bool serializeConfig(Print &);
bool deserializeConfig(Stream &);

#endif // _JSONCONFIG_H
