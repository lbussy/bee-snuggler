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

#include "pidchambertemp.h"

Ticker chamberPIDTicker;
Ticker chamberFanTicker;

// Chamber PID
static float _setpointC, _inputC, _outputC;
float _KpC = 2, _KiC = 5, _KdC = 1;
QuickPID chamberPID(&_inputC, &_outputC, &_setpointC, _KpC, _KiC, _KdC, QuickPID::DIRECT);

// Treatment
unsigned long startTreatTime = 0;
unsigned long finishTreatTime = 0;
unsigned long remainingTreatTime = 0;
unsigned long remainingFanTime = 0;
unsigned long startFanTime = 0;
unsigned long finishFanTime = 0;
bool fanOn;
bool fanCoolDown;

void initChamberPID()
{
    // Set up QuickPID
    _setpointC = config.temps.setpoint;
    _inputC = tempSensors.sensor[config.temps.controlpoint].value;
    chamberPID.SetMode(QuickPID::AUTOMATIC);
    chamberPID.SetSampleTimeUs(CHAMBERPIDSAMPLETIME); // 30 secs, should be 5-10x less than system time constant
    HeatPWM *heatpwm = HeatPWM::getInstance();
    chamberPID.SetOutputLimits(0, heatpwm->getResMask());
    // Start the loop
    loopChamberPID();
}

TREAT_STATE treatState()
{
    if (config.temps.dotreatment)
    {
        return TREAT_ON;
    }
    else if (fanCoolDown)
    {
        return TREAT_COOLDOWN;
    }
    else
    {
        return TREAT_IDLE;
    }
}

void setControlOff()
{
    // Use this to crash everything off
    HeatPWM *heatpwm = HeatPWM::getInstance();
    FanPWM *fanpwm = FanPWM::getInstance();

    // Finish Mite Treatment
    config.temps.dotreatment = false;
    heatpwm->setDutyCyclePct(0);

    // Finish Mite Treatment Cooldown
    chamberFanTicker.detach();
    startFanTime = 0;
    finishFanTime = 0;
    remainingFanTime = config.temps.coolperiod;
    fanOn = false;
    fanCoolDown = false;
    fanpwm->setDutyCyclePct(0);

    // Turn off all control
    config.temps.controlenabled = false;
    config.temps.dotreatment = false;
}

void loopChamberPID()
{
    HeatPWM *heatpwm = HeatPWM::getInstance();
    // Handle Mite Treatment
    if (config.temps.dotreatment)
    {
        if (startTreatTime == 0)
        {
            // Start Mite Treatment
            startTreatTime = millis();
            finishTreatTime = millis() + (config.temps.treatperiod * 60 * 1000);
            _setpointC = config.temps.treatpoint;
            chamberFanTicker.once(CHAMBERPIDLOOPRATE, loopFanControl);
        }
        else if (finishTreatTime < millis())
        {
            // Finish Mite Treatment
            startTreatTime = 0;
            finishTreatTime = 0;
            remainingTreatTime = config.temps.treatperiod;
            config.temps.dotreatment = false;
            heatpwm->setDutyCyclePct(0);
            _setpointC = config.temps.setpoint;
        }
        else
        {
            // Continue Mite Treatment
            remainingTreatTime = (finishTreatTime - millis()) / 60 / 1000;
            _setpointC = config.temps.treatpoint;
        }
    }
    else
    {
        // Make sure we are zeroed in case we turned it off early
        startTreatTime = 0;
        finishTreatTime = 0;
        remainingTreatTime = config.temps.treatperiod;
        _setpointC = config.temps.setpoint;
    }

    // Handle temp control
    if (config.temps.dotreatment || config.temps.controlenabled)
    {
        if (tempSensors.sensor[config.temps.controlpoint].value != DEVICE_DISCONNECTED_F)
        {
            _inputC = tempSensors.sensor[config.temps.controlpoint].value;
            chamberPID.Compute();
            if (config.temps.enablemaxout)
                checkOutputPid(_outputC);
            else
                heatpwm->setDutyCycle(_outputC);
        }
        else
        {
            // Shut off all temp control
            config.temps.dotreatment = false;
            config.temps.controlenabled = false;
        }
    }
    else
    {
        heatpwm->setDutyCycle(0);
    }
    chamberPIDTicker.once(CHAMBERPIDLOOPRATE, loopChamberPID);
}

void loopFanControl()
{
    HeatPWM *heatpwm = HeatPWM::getInstance();
    FanPWM *fanpwm = FanPWM::getInstance();

    // Handle Mite Treatment Fan Disposition
    if (config.temps.dotreatment || fanOn)
    {
        if (startFanTime == 0)
        {
            // Start Mite Treatment Fan
            startFanTime = startTreatTime;
            fanOn = true;
            fanCoolDown = false;
        }
        else if (!config.temps.dotreatment)
        {
            // Continue Mite Treatment Fan (cool-down)
            if (!fanCoolDown)
            {
                int cooldown = config.temps.coolperiod * 60 * 1000;
                finishFanTime += millis() + cooldown;
                fanCoolDown = true;
            }
            remainingFanTime = (finishFanTime - millis()) / 60 / 1000;
        }
        chamberFanTicker.once(CHAMBERPIDLOOPRATE, loopFanControl);
    }
    else
    {
        // Make sure we are zeroed in case we turned it off early
        startFanTime = 0;
        finishFanTime = 0;
        remainingFanTime = 0;
        fanOn = false;
        fanCoolDown = false;
    }

    // Handle Fan control
    if (fanOn)
    {
        // Set fan speed
        if (!config.temps.dotreatment)
        {
            if (finishFanTime < millis())
            {
                // Finish cooldown
                fanCoolDown = false;
                fanOn = false;
            }
            // In cooldown mode, set fan to 100%
            fanpwm->setDutyCyclePct(100);
        }
        else
        {
            // Set fan output speed
            float heatOutput = heatpwm->getDutyCyclePct();
            int fanRange = config.snugconfig.fanrangehigh - config.snugconfig.fanrangelow;
            float fanSet = (fanRange * heatOutput/100) + config.snugconfig.fanrangelow;
            // Just in case, constrain the value between low and high
            fanSet = fanSet < config.snugconfig.fanrangelow ? config.snugconfig.fanrangelow : fanSet;
            fanSet = fanSet > config.snugconfig.fanrangehigh ? config.snugconfig.fanrangehigh : fanSet;
            fanpwm->setDutyCyclePct(fanSet);
        }
    }
    else
    {
        fanpwm->setDutyCyclePct(0);
    }
}

float getChamberKp() { return _KpC; }

float getChamberKi() { return _KiC; }

float getChamberKd() { return _KdC; }

void setChamberKp(float Kx)
{
    _KpC = Kx;
    chamberPID.SetTunings(_KpC, _KiC, _KdC);
}

void setChamberKi(float Kx)
{
    _KiC = Kx;
    chamberPID.SetTunings(_KpC, _KiC, _KdC);
}

void setChamberKd(float Kx)
{
    _KdC = Kx;
    chamberPID.SetTunings(_KpC, _KiC, _KdC);
}
