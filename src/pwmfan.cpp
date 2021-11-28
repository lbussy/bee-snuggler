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

#include "pwmfan.h"

FanPWM *FanPWM::fanpwm = NULL;

FanPWM *FanPWM::getInstance()
{
    if (!fanpwm)
    {
        fanpwm = new FanPWM();
        FanPWM *fanpwm = FanPWM::getInstance();
        fanpwm->resolutionMask = (static_cast<int>(pow(2, fanpwm->resolutionInt) - 1));
        ledcSetup(fanpwm->channel, fanpwm->freq, fanpwm->resolutionInt);
        fanpwm->attach();
        fanpwm->setDutyCyclePct(0);
    }
    return fanpwm;
}

void FanPWM::attach()
{
    // Attach pin to LED control channel
    ledcAttachPin(fanpwm->pin, fanpwm->channel);
}

void FanPWM::detach()
{
    // Detach pin from LED control channel
    (fanpwm->inverted) ? (fanpwm->dutyCycle = getResMask()) : (fanpwm->dutyCycle = 0);
    ledcDetachPin(fanpwm->pin);
    if (fanpwm->inverted)
        digitalWrite(fanpwm->pin, HIGH);
    else
        digitalWrite(fanpwm->pin, LOW);
}

void FanPWM::setDutyCycle(int dutyCycle)
{
    if (!dutyCycle)
    {
        fanpwm->detach();
    }
    else
    {
        attach();
        if (fanpwm->inverted)
        {
            fanpwm->dutyCycle = fanpwm->invert(dutyCycle);
        }
        else
        {
            fanpwm->dutyCycle = dutyCycle;
        }
        ledcWrite(fanpwm->channel, fanpwm->dutyCycle);
    }
}

void FanPWM::setDutyCyclePct(double dutyCyclePct)
{
    // Set class properties
    fanpwm->setDutyCycle(round((dutyCyclePct / 100) * fanpwm->resolutionMask));
}

int FanPWM::getResolutionInt()
{
    return fanpwm->resolutionInt;
}

int FanPWM::getResMask()
{
    return fanpwm->resolutionMask;
}

int FanPWM::getDutyCycle()
{
    if (fanpwm->inverted)
        return fanpwm->invert(fanpwm->dutyCycle);
    else
        return fanpwm->dutyCycle;
}

double FanPWM::getDutyCyclePct()
{
    return (double)fanpwm->getDutyCycle() / (double)fanpwm->resolutionMask * (double(100));
}

int FanPWM::invert(int number)
{
    // Inverting the bits one by one
    for (int i = 0; i < fanpwm->getResolutionInt(); i++)
        number = (number ^ (1 << i));

    return number;
}
