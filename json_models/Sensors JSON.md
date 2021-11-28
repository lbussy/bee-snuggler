Sensors Report JSON Model:
===========================

JSON Definition:
----------------

```
{
  "title": xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx in xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
  "imperial": false,
  "controlpoint": 99,
  "setting": 999.999,
  "pwm": 100.999,
  "controlenabled": false,
  "sensors": [
    {
      "enable": false,
      "name": "xxxxxxxxxxxxxxxxxxxxxxxx",
      "value": -196.666
    },
    {
      "enable": false,
      "name": "xxxxxxxxxxxxxxxxxxxxxxxx",
      "value": -196.666
    },
    {
      "enable": false,
      "name": "xxxxxxxxxxxxxxxxxxxxxxxx",
      "value": -196.666
    },
    {
      "enable": false,
      "name": "xxxxxxxxxxxxxxxxxxxxxxxx",
      "value": -196.666
    }
  ]
}
```

Size:
-----

```
Deserialize: 768
Serialize: 768
```

Deserializing / Parsing / Loading:
----------------------------------

```
// Stream& input;

StaticJsonDocument<768> doc;

DeserializationError error = deserializeJson(doc, input);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

const char* title = doc["title"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx in ...
bool imperial = doc["imperial"]; // false
int controlpoint = doc["controlpoint"]; // 99
float setting = doc["setting"]; // 999.999
float pwm = doc["pwm"]; // 100.999
bool controlenabled = doc["controlenabled"]; // false

for (JsonObject elem : doc["sensors"].as<JsonArray>()) {

  bool enable = elem["enable"]; // false, false, false, false
  const char* name = elem["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxx", "xxxxxxxxxxxxxxxxxxxxxxxx", ...
  float value = elem["value"]; // -196.666, -196.666, -196.666, -196.666

}
```

Serializing / Saving:
---------------------

```
StaticJsonDocument<768> doc;

doc["title"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx in xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["imperial"] = false;
doc["controlpoint"] = 99;
doc["setting"] = 999.999;
doc["pwm"] = 100.999;
doc["controlenabled"] = false;

JsonArray sensors = doc.createNestedArray("sensors");

JsonObject sensors_0 = sensors.createNestedObject();
sensors_0["enable"] = false;
sensors_0["name"] = "xxxxxxxxxxxxxxxxxxxxxxxx";
sensors_0["value"] = -196.666;

JsonObject sensors_1 = sensors.createNestedObject();
sensors_1["enable"] = false;
sensors_1["name"] = "xxxxxxxxxxxxxxxxxxxxxxxx";
sensors_1["value"] = -196.666;

JsonObject sensors_2 = sensors.createNestedObject();
sensors_2["enable"] = false;
sensors_2["name"] = "xxxxxxxxxxxxxxxxxxxxxxxx";
sensors_2["value"] = -196.666;

JsonObject sensors_3 = sensors.createNestedObject();
sensors_3["enable"] = false;
sensors_3["name"] = "xxxxxxxxxxxxxxxxxxxxxxxx";
sensors_3["value"] = -196.666;

serializeJson(doc, output);
```
