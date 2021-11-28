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
#ifndef _PIDOUTPUT_H
#define _PIDOUTPUT_H

#include "pwmheat.h"
#include "tempsensors.h"

#include <QuickPID.h>
#include <Ticker.h>
#include <ArduinoLog.h>
#include <Arduino.h>

void checkOutputPid(int input);
void stopOutputPID();
void loopOutputPID();
float getOutputKp();               // proportional gain
float getOutputKi();               // integral gain
float getOutputKd();               // derivative gain
void setOutputKp(float Kx);        // proportional gain
void setOutputKi(float Kx);        // integral gain
void setOutputKd(float Kx);        // derivative gain

#endif // _PIDOUTPUT_H
