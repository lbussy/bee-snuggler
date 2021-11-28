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

#include "tempsensors.h"

Sensors tempSensors;

Ticker sensorCheck;
const char *sensorName[NUMSENSOR] = {AMBTEMP, UPPERTEMP, CENTERTEMP, LOWERTEMP, OUTPUTTEMP};
int sensorPin[NUMSENSOR] = {AMBSENSE, UPPERSENSE, CENTERSENSE, LOWERSENSE, OUTPUTSENSE};

const double deviceDisconnectedF = DEVICE_DISCONNECTED_F;

Sensors::Sensors()
{
    for (int i = 0; i < NUMSENSOR; i++)
    {
        strlcpy(sensor[i].name, sensorName[i], sizeof(sensor[i].name));
        sensor[i].pin = sensorPin[i];
        sensor[i].value = deviceDisconnectedF;
        sensor[i].average = deviceDisconnectedF;
        sensor[i].calibration = config.temps.calibration[i];
        sensor[i].buffer.clear();
    }
    pollTemps();
}

void Sensors::pollTemps()
{
    for (int i = 0; i < NUMSENSOR; i++)
    {
        double _val = getTempF(sensor[i].pin);
        bool goodVal = !((int)_val == (int)deviceDisconnectedF);
        Serial.print(""); // Don't remove this!!! Weird quantum state BS going on.

        if (!goodVal)
        {
            // Delete oldest value if device shows disconnected
            if (!sensor[i].buffer.isEmpty())
                sensor[i].buffer.shift();
        }
        else
        {
            // Calibration: Values will be stored corrected
            sensor[i].value = _val + sensor[i].calibration;
            // Push to buffer
            sensor[i].buffer.push(sensor[i].value);
        }

        if (sensor[i].buffer.isEmpty())
        {
            sensor[i].average = deviceDisconnectedF;
        }
        else
        {
            // Create average
            float avg = 0.0;
            for (int x = 0; x < sensor[i].buffer.size(); x++)
            {
                avg += sensor[i].buffer[x] / sensor[i].buffer.size();
            }
            sensor[i].average = avg;
        }
    }
    sensorCheck.once(TEMPLOOP, []() { tempSensors.pollTemps(); });
}

double Sensors::getTempF(uint8_t pin)
{
    OneWire oneWire(pin);
    DS18B20 sensor(&oneWire);
    float retVal = deviceDisconnectedF;
    sensor.begin(); // No idea why I need to do this, timing?
    delay(10);
    if (sensor.begin())
    {
        sensor.setResolution(TEMP_12_BIT);
        sensor.requestTemperatures();
        while (!sensor.isConversionComplete())
            delay(10);
        retVal = sensor.getTempF();
    }
    return retVal;
}
