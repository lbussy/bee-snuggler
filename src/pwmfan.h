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
#ifndef _PWMFAN_H
#define _PWMFAN_H

#include "config.h"
#include <Arduino.h>
#include <ArduinoLog.h>
#include <Ticker.h>

class FanPWM
{
private:
    // Singleton Declarations
    FanPWM() {}
    static FanPWM *fanpwm;

    // Private properties
    int freq = 2;
    // int pin = FAN;
    int pin = LED_BUILTIN; // DEBUG
    bool inverted = true;
    int channel = 2;
    int resolutionInt = 8;
    int resolutionMask;
    int dutyCycle;

    // Private Methods
    void attach();
    void detach();
    int invert(int number);

public:
    // Singleton Declarations
    static FanPWM *getInstance();
    ~FanPWM() { fanpwm = NULL; }

    // Setters
    void setDutyCycle(int dutyCycle);
    void setDutyCyclePct(double dutyCyclePct);

    // Getters
    int getResolutionInt();
    int getResMask();
    int getDutyCycle();
    double getDutyCyclePct();
};

#endif // _PWMFAN_H
