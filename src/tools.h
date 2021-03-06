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
#ifndef _TOOLS_H
#define _TOOLS_H

#include "config.h"
#include "wifihandler.h"
#include "ntphandler.h"
#include "mdnshandler.h"

#include <Ticker.h>
#include <SPIFFS.h>
#include <ArduinoLog.h>
#include <Arduino.h>

void _delay(unsigned long);
void resetController();
void setDoWiFiReset();
void setDoReset();
void maintLoop();

double convertFtoC(double);
double convertCtoF(double);
double convertCalFtoC(double);
double convertCalCtoF(double);
double convertGtoL(double);
double convertLtoG(double);

void getGuid(char *str); // 17 chars including null terminator

static bool __attribute__((unused)) doReset = false;        // Semaphore for reset
static bool __attribute__((unused)) doWiFiReset = false;    // Semaphore for wifi reset

#endif
