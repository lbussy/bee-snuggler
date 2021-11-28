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

#include "jsonconfig.h"

Config config;
static const char *filename = FILENAME;

bool deleteConfigFile()
{
    if (!SPIFFS.begin())
    {
        return false;
    }
    return SPIFFS.remove(filename);
}

bool loadConfig()
{
    // Manage loading the configuration
    if (!loadFile())
    {
        return false;
    }
    else
    {
        saveFile();
        return true;
    }
}

bool loadFile()
{
    if (!SPIFFS.begin())
    {
        return false;
    }
    // Loads the configuration from a file on SPIFFS
    File file = SPIFFS.open(filename, "r");
    if (!SPIFFS.exists(filename) || !file)
    {
        // File does not exist or unable to read file
    }
    else
    {
        // Existing configuration present
    }

    if (!deserializeConfig(file))
    {
        file.close();
        return false;
    }
    else
    {
        file.close();
        return true;
    }
}

bool saveConfig()
{
    return saveFile();
}

bool saveFile()
{
    // Saves the configuration to a file on SPIFFS
    File file = SPIFFS.open(filename, "w");
    if (!file)
    {
        file.close();
        return false;
    }

    // Serialize JSON to file
    if (!serializeConfig(file))
    {
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool deserializeConfig(Stream &src)
{
    // Deserialize configuration
    StaticJsonDocument<CAP_DESER_CONF> doc;

    // Parse the JSON object in the file
    DeserializationError err = deserializeJson(doc, src);

    if (err)
    {
        config.load(doc.as<JsonObject>());
        return true;
    }
    else
    {
        config.load(doc.as<JsonObject>());
        return true;
    }
}

bool serializeConfig(Print &dst)
{
    // Serialize configuration
    StaticJsonDocument<CAP_SER_CONF> doc;

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    config.save(root);

    // Serialize JSON to file
    return serializeJsonPretty(doc, dst) > 0;
}

bool printFile()
{
    // Prints the content of a file to the Serial
    File file = SPIFFS.open(filename, "r");
    if (!file)
        return false;

    while (file.available())
        printChar(true, (const char *)file.read());

    printCR(true);
    file.close();
    return true;
}

bool printConfig()
{
    // Serialize configuration
    StaticJsonDocument<CAP_SER_CONF> doc;

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    config.save(root);

    bool retval = true;
    // Serialize JSON to file
    retval = serializeJson(doc, Serial) > 0;
    printCR(true);
    return retval;
}

void ApConfig::save(JsonObject obj) const
{
    obj["ssid"] = ssid;
    obj["passphrase"] = passphrase;
}

void ApConfig::load(JsonObjectConst obj)
{
    // Load Access Point configuration
    //
    if (obj["ssid"].isNull())
    {
        strlcpy(ssid, APNAME, sizeof(ssid));
    }
    else
    {
        const char *sd = obj["ssid"];
        strlcpy(ssid, sd, sizeof(ssid));
    }

    if (obj["passphrase"].isNull())
    {
        strlcpy(passphrase, AP_PASSWD, sizeof(passphrase));
    }
    else
    {
        const char *ps = obj["passphrase"];
        strlcpy(passphrase, ps, sizeof(passphrase));
    }
}

void OTA::save(JsonObject obj) const
{
    obj["dospiffs1"] = dospiffs1;
    obj["dospiffs2"] = dospiffs2;
    obj["didupdate"] = didupdate;
}

void OTA::load(JsonObjectConst obj)
{
    // Load OTA configuration
    //
    if (obj["dospiffs1"].isNull())
    {
        dospiffs1 = false;
    }
    else
    {
        dospiffs1 = obj["dospiffs1"];
    }

    if (obj["dospiffs2"].isNull())
    {
        dospiffs2 = false;
    }
    else
    {
        dospiffs2 = obj["dospiffs2"];
    }

    if (obj["didupdate"].isNull())
    {
        didupdate = false;
    }
    else
    {
        didupdate = obj["didupdate"];
    }
}

void SnugConfig::save(JsonObject obj) const
{
    obj["guid"] = guid;
    obj["hostname"] = hostname;
    obj["nodrd"] = nodrd;
    obj["yardname"] = yardname;
    obj["hivename"] = hivename;
    obj["imperial"] = imperial;
    obj["fancontrol"] = fancontrol;
    obj["propfan"] = propfan;
    obj["fanrangehigh"] = fanrangehigh;
    obj["fanrangelow"] = fanrangelow;
}

void SnugConfig::load(JsonObjectConst obj)
{
    // Load Cop configuration
    //
    getGuid(guid); // Always get this

    if (obj["hostname"].isNull())
    {
        strlcpy(hostname, APNAME, sizeof(hostname));
    }
    else
    {
        const char *hn = obj["hostname"];
        strlcpy(hostname, hn, sizeof(hostname));
    }

    if (obj["nodrd"].isNull())
    {
        nodrd = true;
    }
    else
    {
        nodrd = obj["nodrd"];
    }

    if (obj["yardname"].isNull())
    {
        strlcpy(yardname, YARDNAME, sizeof(yardname));
    }
    else
    {
        const char *yn = obj["yardname"];
        strlcpy(yardname, yn, sizeof(yardname));
    }

    if (obj["hivename"].isNull())
    {
        strlcpy(hivename, HIVENAME, sizeof(hivename));
    }
    else
    {
        const char *bn = obj["hivename"];
        strlcpy(hivename, bn, sizeof(hivename));
    }

    if (obj["imperial"].isNull())
    {
        imperial = IMPERIAL;
    }
    else
    {
        bool units = obj["imperial"];
        imperial = units;
    }

    if (obj["fancontrol"].isNull())
    {
        fancontrol = true;
    }
    else
    {
        bool fc = obj["fancontrol"];
        fancontrol = fc;
    }

    if (obj["propfan"].isNull())
    {
        propfan = true;
    }
    else
    {
        bool pf = obj["propfan"];
        propfan = pf;
    }

    if (obj["fanrangehigh"].isNull())
    {
        fanrangehigh = 100;
    }
    else
    {
        int fh = obj["fanrangehigh"];
        fanrangehigh = fh;
    }

    if (obj["fanrangelow"].isNull())
    {
        fanrangelow = 25;
    }
    else
    {
        int fl = obj["fanrangelow"];
        fanrangelow = fl;
    }
}

void Temperatures::save(JsonObject obj) const
{
    obj["setpoint"] = setpoint;
    obj["controlpoint"] = controlpoint;
    obj["controlenabled"] = controlenabled;
    // obj["dotreatment"] = dotreatment; // We do not save this
    obj["outputmax"] = outputmax;
    obj["enablemaxout"] = enablemaxout;
    obj["treatpoint"] = treatpoint;
    obj["treatperiod"] = treatperiod;
    obj["coolperiod"] = coolperiod;
    obj["ambientenabled"] = enabled[0];
    obj["ambientcal"] = calibration[0];
    obj["upperenabled"] = enabled[1];
    obj["uppercal"] = calibration[1];
    obj["centerenabled"] = enabled[2];
    obj["centercal"] = calibration[2];
    obj["lowerenabled"] = enabled[3];
    obj["lowercal"] = calibration[3];
    obj["outputenabled"] = enabled[4];
    obj["outputcal"] = calibration[4];
}

void Temperatures::load(JsonObjectConst obj)
{
    // Load Temperature configuration
    // 
    if (obj["setpoint"].isNull())
    {
        setpoint = DEFSET;
    }
    else
    {
        float sp = obj["setpoint"];
        setpoint = sp;
    }

    if (obj["controlpoint"].isNull())
    {
        controlpoint = DEFCON;
    }
    else
    {
        int cp = obj["controlpoint"];
        controlpoint = cp;
    }

    if (obj["controlenabled"].isNull())
    {
        controlenabled = CTRLENABLE;
    }
    else
    {
        bool ce = obj["controlenabled"];
        controlenabled = ce;
    }

    dotreatment = false; // Always false on startup

    if (obj["treatpoint"].isNull())
    {
        treatpoint = DEFTREATSET;
    }
    else
    {
        float tp = obj["treatpoint"];
        treatpoint = tp;
    }

    if (obj["treatperiod"].isNull())
    {
        treatperiod = DEFTREATMINS;
    }
    else
    {
        float tp = obj["treatperiod"];
        treatperiod = tp;
    }

    if (obj["coolperiod"].isNull())
    {
        coolperiod = DEFCOOLMINS;
    }
    else
    {
        float cp = obj["coolperiod"];
        coolperiod = cp;
    }

    // Ouput temperature control
    if (obj["outputmax"].isNull())
    {
        outputmax = DEFMAX;
    }
    else
    {
        float sp = obj["outputmax"];
        outputmax = sp;
    }

    if (obj["enablemaxout"].isNull())
    {
        enablemaxout = MAXENABLE;
    }
    else
    {
        bool ce = obj["enablemaxout"];
        enablemaxout = ce;
    }

    // Sensor Control

    if (obj["ambientenabled"].isNull())
    {
        enabled[0] = false;
    }
    else
    {
        bool en = obj["ambientenabled"];
        enabled[0] = en;
    }

    if (obj["ambientcal"].isNull())
    {
        calibration[0] = 0.0;
    }
    else
    {
        float cal = obj["ambientcal"];
        calibration[0] = cal;
    }

    if (obj["upperenabled"].isNull())
    {
        enabled[1] = false;
    }
    else
    {
        bool en = obj["upperenabled"];
        enabled[1] = en;
    }

    if (obj["uppercal"].isNull())
    {
        calibration[1] = 0.0;
    }
    else
    {
        float cal = obj["uppercal"];
        calibration[1] = cal;
    }

    if (obj["centerenabled"].isNull())
    {
        enabled[2] = false;
    }
    else
    {
        bool en = obj["centerenabled"];
        enabled[2] = en;
    }

    if (obj["centercal"].isNull())
    {
        calibration[2] = 0.0;
    }
    else
    {
        float cal = obj["centercal"];
        calibration[2] = cal;
    }

    if (obj["lowerenabled"].isNull())
    {
        enabled[3] = false;
    }
    else
    {
        bool en = obj["lowerenabled"];
        enabled[3] = en;
    }

    if (obj["lowercal"].isNull())
    {
        calibration[3] = 0.0;
    }
    else
    {
        float cal = obj["lowercal"];
        calibration[3] = cal;
    }

    if (obj["outputenabled"].isNull())
    {
        enabled[4] = false;
    }
    else
    {
        bool en = obj["outputenabled"];
        enabled[4] = en;
    }

    if (obj["outputcal"].isNull())
    {
        calibration[4] = 0.0;
    }
    else
    {
        float cal = obj["outputcal"];
        calibration[4] = cal;
    }
}

void Config::save(JsonObject obj) const
{
    // Add Access Point object
    apconfig.save(obj.createNestedObject("apconfig"));
    // Add Bee Snuggler object
    snugconfig.save(obj.createNestedObject("snugconfig"));
    // Add OTA object
    ota.save(obj.createNestedObject("ota"));
    // Add Temperatures object
    temps.save(obj.createNestedObject("temps"));
}

void Config::load(JsonObjectConst obj)
{
    // Load all config objects
    //
    apconfig.load(obj["apconfig"]);
    snugconfig.load(obj["snugconfig"]);
    ota.load(obj["ota"]);
    temps.load(obj["temps"]);
}
