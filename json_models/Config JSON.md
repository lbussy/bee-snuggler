# Configuration JSON:

```
{
  "apconfig": {
    "ssid": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "passphrase": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
  },
  "snugconfig": {
    "guid": "xxxxxxxxxxxxxxxx",
    "hostname": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "nodrd": false,
    "yardname": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "hivename": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "imperial": false,
    "fancontrol": false,
    "propfan": false,
    "fanrangehigh": 100,
    "fanrangelow": 100
  },
  "ota": {
    "dospiffs1": false,
    "dospiffs2": false,
    "didupdate": false
  },
  "temps": {
    "setpoint": 100.999,
    "controlpoint": 2,
    "controlenabled": false,
    "treatpoint": 100.999,
    "treatperiod": 1000,
    "coolperiod": 1000,
    "dotreatment": false,
    "outputmax": 100.999,
    "enablemaxout": false,
    "ambientenabled": false,
    "ambientcal": -99,
    "upperenabled": true,
    "uppercal": 99,
    "centerenabled": false,
    "centercal": -99,
    "lowerenabled": false,
    "lowercal": 99
  }
}
```

## Size:

```
Serialize:      1024
Deserialize:    1536
```

## Deserializing:

```
// Stream& input;

StaticJsonDocument<1536> doc;

DeserializationError error = deserializeJson(doc, input);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

const char* apconfig_ssid = doc["apconfig"]["ssid"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* apconfig_passphrase = doc["apconfig"]["passphrase"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

JsonObject snugconfig = doc["snugconfig"];
const char* snugconfig_guid = snugconfig["guid"]; // "xxxxxxxxxxxxxxxx"
const char* snugconfig_hostname = snugconfig["hostname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool snugconfig_nodrd = snugconfig["nodrd"]; // false
const char* snugconfig_yardname = snugconfig["yardname"];
const char* snugconfig_hivename = snugconfig["hivename"];
bool snugconfig_imperial = snugconfig["imperial"]; // false
bool snugconfig_fancontrol = snugconfig["fancontrol"]; // false
bool snugconfig_propfan = snugconfig["propfan"]; // false
int snugconfig_fanrangehigh = snugconfig["fanrangehigh"]; // 100
int snugconfig_fanrangelow = snugconfig["fanrangelow"]; // 100

JsonObject ota = doc["ota"];
bool ota_dospiffs1 = ota["dospiffs1"]; // false
bool ota_dospiffs2 = ota["dospiffs2"]; // false
bool ota_didupdate = ota["didupdate"]; // false

JsonObject temps = doc["temps"];
float temps_setpoint = temps["setpoint"]; // 100.999
int temps_controlpoint = temps["controlpoint"]; // 2
bool temps_controlenabled = temps["controlenabled"]; // false
float temps_treatpoint = temps["treatpoint"]; // 100.999
int temps_treatperiod = temps["treatperiod"]; // 1000
int temps_coolperiod = temps["coolperiod"]; // 1000
bool temps_dotreatment = temps["dotreatment"]; // false
float temps_outputmax = temps["outputmax"]; // 100.999
bool temps_enablemaxout = temps["enablemaxout"]; // false
bool temps_ambientenabled = temps["ambientenabled"]; // false
int temps_ambientcal = temps["ambientcal"]; // -99
bool temps_upperenabled = temps["upperenabled"]; // true
int temps_uppercal = temps["uppercal"]; // 99
bool temps_centerenabled = temps["centerenabled"]; // false
int temps_centercal = temps["centercal"]; // -99
bool temps_lowerenabled = temps["lowerenabled"]; // false
int temps_lowercal = temps["lowercal"]; // 99
```

## Serializing:

```
StaticJsonDocument<1024> doc;

JsonObject apconfig = doc.createNestedObject("apconfig");
apconfig["ssid"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
apconfig["passphrase"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

JsonObject snugconfig = doc.createNestedObject("snugconfig");
snugconfig["guid"] = "xxxxxxxxxxxxxxxx";
snugconfig["hostname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
snugconfig["nodrd"] = false;
snugconfig["yardname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
snugconfig["hivename"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
snugconfig["imperial"] = false;
snugconfig["fancontrol"] = false;
snugconfig["propfan"] = false;
snugconfig["fanrangehigh"] = 100;
snugconfig["fanrangelow"] = 100;

JsonObject ota = doc.createNestedObject("ota");
ota["dospiffs1"] = false;
ota["dospiffs2"] = false;
ota["didupdate"] = false;

JsonObject temps = doc.createNestedObject("temps");
temps["setpoint"] = 100.999;
temps["controlpoint"] = 2;
temps["controlenabled"] = false;
temps["treatpoint"] = 100.999;
temps["treatperiod"] = 1000;
temps["coolperiod"] = 1000;
temps["dotreatment"] = false;
temps["outputmax"] = 100.999;
temps["enablemaxout"] = false;
temps["ambientenabled"] = false;
temps["ambientcal"] = -99;
temps["upperenabled"] = true;
temps["uppercal"] = 99;
temps["centerenabled"] = false;
temps["centercal"] = -99;
temps["lowerenabled"] = false;
temps["lowercal"] = 99;

serializeJson(doc, output);
```
