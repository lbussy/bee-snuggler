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

#include "tools.h"

void _delay(unsigned long ulDelay)
{
    // Safe semi-blocking delay
    vTaskDelay(ulDelay); // Builtin to ESP32
}

void resetController()
{
    Log.notice(F("Reboot request - rebooting system." CR));
    config.snugconfig.nodrd = true;
    saveConfig();
    ESP.restart();
}

void setDoReset()
{
    doReset = true; // Semaphore required for reset in callback
}

void setDoWiFiReset()
{
    doWiFiReset = true; // Semaphore required for wifi reset in web page
}

void maintLoop()
{
    // Necessary because we cannot delay or do radio work in a callback/Ticker
    //
    if (doReset)
    { // Check for Reset Pending
        doReset = false;
        resetController();
    }
    if (doWiFiReset)
    { // Need to do this to prevent WDT
        doWiFiReset = false;
        resetWifi();
    }
    if (ESP.getFreeHeap() < MINFREEHEAP)
    {
        Log.warning(F("Maintenance: Heap memory has degraded below safe minimum, restarting." CR));
        setDoReset();
    }
}

double convertFtoC(double F)
{
    // T(°C) = (T(°F) - 32) × 5/9
    return (F - 32) / 1.8;
}

double convertCtoF(double C)
{
    // T(°F) = T(°C) × 9/5 + 32
    return (C * 1.8) + 32;
}

double convertCalFtoC(double F)
{
    // Zero-based conversion for the purposes of adding
    // or subtracting a calibration amount.
    // T(°C) = (T(°F) X (5/9)) - (32/9))
    return F * 0.555;
}

double convertCalCtoF(double C)
{
    // Zero-based conversion for the purposes of adding
    // or subtracting a calibration amount.
    // T(°F) = T(°C) × 9/5 + 32
    return C * 1.8;
}

double convertGtoL(double G)
{
    // L = G / 0.26417
    return G / 0.26417;
}

double convertLtoG(double L)
{
    // G = L * 0.26417
    return L * 0.26417;
}

void getGuid(char *str)
{
    // char newguid[sizeof(guid)];
    // getGuid(newguid);
    uint64_t chipid = ESP.getEfuseMac();
    uint32_t int32_1, int32_2;

    int32_1 = chipid & 0x00000000FFFFFFFF;
    int32_2 = (chipid & 0xFFFFFFFF00000000) >> 32;

    char first[9], secon[9];
    sprintf(first, "%08X", int32_1);
    sprintf(secon, "%08X", int32_2);

    strcpy(str, first);
    strcat(str, secon);
}
