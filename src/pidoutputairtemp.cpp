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

#include "pidoutputairtemp.h"

Ticker outputPIDTicker;
static bool outputPidEnabled = false;

// Heat Output PID
static float _setpointO, _inputO, _outputO;
float _KpO = 2, _KiO = 5, _KdO = 1;
QuickPID heaterPID(&_inputO, &_outputO, &_setpointO, _KpO, _KiO, _KdO, QuickPID::DIRECT);

void checkOutputPid(int input)
{
    HeatPWM *heatpwm = HeatPWM::getInstance();

    // Make sure we have an output sensor
    if (tempSensors.sensor[4].value != DEVICE_DISCONNECTED_F)
    {
        // Set input as output sensor
        _inputO = tempSensors.sensor[4].value; // NOTE: This is a static/hard coded point

        // Calculate load and setpoints
        double minTemp;
        // Set mintemp
        if (tempSensors.sensor[0].average != DEVICE_DISCONNECTED_F) // NOTE: This is hard coded to ambient
            minTemp = 40.0;
        else
            minTemp = tempSensors.sensor[0].value;
        
        // Get load demand % based on input
        double _pctLoad = (heatpwm->getResMask() - input) / heatpwm->getResMask();
        // Calculate total span in degrees
        double _span = (config.temps.outputmax - minTemp);
        // Calculate output ait setpoint adding the % over min to the min
        double _tempSet = minTemp + (_span * _pctLoad);
        // Constrain setpoint between minTemp and config.temps.outputmax just in case
        _setpointO = constrain(_tempSet, minTemp, config.temps.outputmax);

        if (! outputPidEnabled)
        {
            outputPidEnabled = true;

            // Set up QuickPID
            heaterPID.SetMode(QuickPID::AUTOMATIC);
            heaterPID.SetSampleTimeUs(HEATERPIDSAMPLETIME); // TODO: 30 secs, should be 5-10x less than system time constant
            heaterPID.SetOutputLimits(0, heatpwm->getResMask());

            loopOutputPID();
        }
    }
    else
    {
        Log.notice(F("Disabling max output temp control." CR));
        stopOutputPID();
    }
}

void stopOutputPID()
{
    config.temps.enablemaxout = false;
    outputPidEnabled = false;
}

void loopOutputPID()
{
    if (config.temps.controlenabled && config.temps.enablemaxout)
    {
        heaterPID.Compute();
        HeatPWM *heatpwm = HeatPWM::getInstance();
        heatpwm->setDutyCycle(_outputO);
    }
    else
        stopOutputPID();

    if (outputPidEnabled)
        outputPIDTicker.once(OUTPUTPIDLOOPRATE, loopOutputPID);
}

float getOutputKp() {return _KpO;}

float getOutputKi() {return _KiO;}

float getOutputKd() {return _KdO;}

void setOutputKp(float Kx)
{
    _KpO = Kx;
    heaterPID.SetTunings(_KpO, _KiO, _KdO);
}

void setOutputKi(float Kx)
{
    _KiO = Kx;
    heaterPID.SetTunings(_KpO, _KiO, _KdO);
}

void setOutputKd(float Kx)
{
    _KdO = Kx;
    heaterPID.SetTunings(_KpO, _KiO, _KdO);
}
