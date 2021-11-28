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

#include "pwmheat.h"

HeatPWM *HeatPWM::heatpwm = NULL;

HeatPWM *HeatPWM::getInstance()
{
    if (!heatpwm)
    {
        heatpwm = new HeatPWM();
        HeatPWM *heatpwm = HeatPWM::getInstance();
        heatpwm->resolutionMask = (static_cast<int>(pow(2, heatpwm->resolutionInt) - 1));
        ledcSetup(heatpwm->channel, heatpwm->freq, heatpwm->resolutionInt);
        heatpwm->attach();
        heatpwm->setDutyCyclePct(0);
    }
    return heatpwm;
}

void HeatPWM::attach()
{
    // Attach pin to LED control channel
    ledcAttachPin(heatpwm->pin, heatpwm->channel);
}

void HeatPWM::detach()
{
    // Detach pin from LED control channel
    (heatpwm->inverted) ? (heatpwm->dutyCycle = getResMask()) : (heatpwm->dutyCycle = 0);
    ledcDetachPin(heatpwm->pin);
    if (heatpwm->inverted)
        digitalWrite(heatpwm->pin, HIGH);
    else
        digitalWrite(heatpwm->pin, LOW);
}

void HeatPWM::setDutyCycle(int dutyCycle)
{
    if (!dutyCycle)
    {
        heatpwm->detach();
    }
    else
    {
        attach();
        if (heatpwm->inverted)
        {
            heatpwm->dutyCycle = heatpwm->invert(dutyCycle);
        }
        else
        {
            heatpwm->dutyCycle = dutyCycle;
        }
        ledcWrite(heatpwm->channel, heatpwm->dutyCycle);
    }
}

void HeatPWM::setDutyCyclePct(double dutyCyclePct)
{
    // Set class properties
    heatpwm->setDutyCycle(round((dutyCyclePct / 100) * heatpwm->resolutionMask));
}

int HeatPWM::getResolutionInt()
{
    return heatpwm->resolutionInt;
}

int HeatPWM::getResMask()
{
    return heatpwm->resolutionMask;
}

int HeatPWM::getDutyCycle()
{
    if (heatpwm->inverted)
        return heatpwm->invert(heatpwm->dutyCycle);
    else
        return heatpwm->dutyCycle;
}

double HeatPWM::getDutyCyclePct()
{
    return (double)heatpwm->getDutyCycle() / (double)heatpwm->resolutionMask * (double(100));
}

int HeatPWM::invert(int number)
{
    // Inverting the bits one by one
    for (int i = 0; i < heatpwm->getResolutionInt(); i++)
        number = (number ^ (1 << i));

    return number;
}
