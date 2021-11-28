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
#ifndef _TEMPSENSORS_H
#define _TEMPSENSORS_H

#include "config.h"
#include "jsonconfig.h"
#include "tools.h"

#ifndef CAPTEMPS
#define CAPTEMPS 768
#endif

#ifndef TEMPAVG
#define TEMPAVG 10
#define TEMPLOOP 10
#define NUMSENSOR 5

#define AMBTEMP "ambient"
#define UPPERTEMP "upper"
#define CENTERTEMP "center"
#define LOWERTEMP "lower"
#define OUTPUTTEMP "lower"

#define AMBSENSE 32
#define UPPERSENSE 33
#define CENTERSENSE 25
#define LOWERSENSE 25
#define OUTPUTSENSE 14
#endif

#include <CircularBuffer.h>
#include <OneWire.h>
#include <DS18B20.h>
#include <Ticker.h>
#include <ArduinoLog.h> // DEBUG

// Device resolution
#define TEMP_9_BIT 0x1F  //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

struct Sensor
{
    char name[32];                         // Name of sensor
    int pin;                               // Sensor pin
    double value;                          // Latest temp reading
    double average;                        // Average reading (6 over 1 min)
    double calibration;                    // Calibration offset
    CircularBuffer<float, TEMPAVG> buffer; // Circ buffer for avging
};

class Sensors
{
private:

public:
    Sensors();
    void sensorInit();
    void pollTemps();
    double getTempF(uint8_t);

    Sensor sensor[NUMSENSOR];
};

extern Sensors tempSensors;

#endif // _TEMPSENSORS_H
