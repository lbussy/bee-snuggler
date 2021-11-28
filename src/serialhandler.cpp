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

#include "serialhandler.h"

#if SAFEMODE == 1
#undef DOTELNET
#define DOTELNET false
#endif

#undef SERIAL
#if DOTELNET == true
ESPTelnet SerialAndTelnet;
#define SERIAL SerialAndTelnet // Use Telnet
#else
#define SERIAL Serial // Use hardware serial
#endif

void serial()
{
#if DOTELNET == true
    char buffer[32];
    strcpy(buffer, (const char *)"Connected to ");
    strcat(buffer, (const char *)API_KEY);
    strcat(buffer, (const char *)"\n");
    SERIAL.setWelcomeMsg(buffer);
#endif
    SERIAL.begin(BAUD);
    SERIAL.println();
    SERIAL.flush();
#ifndef DISABLE_LOGGING
    SERIAL.setDebugOutput(true);
    Log.begin(LOG_LEVEL, &SERIAL, true);
    Log.setPrefix(printTimestamp);
    Log.notice(F("Serial logging started at %l." CR), BAUD);
#endif
}

void serialLoop()
{
#if DOTELNET == true
    SERIAL.handle();
#endif
}

void printTimestamp(Print *_logOutput)
{
    time_t now;
    time_t rawtime = time(&now);
    struct tm ts;
    ts = *localtime(&rawtime);
    char locTime[prefLen] = {'\0'};
    strftime(locTime, sizeof(locTime), "%FT%TZ ", &ts);
    _logOutput->print(locTime);
}

size_t printDot()
{
    return printDot(false);
}

size_t printDot(bool safe)
{
#ifndef DISABLE_LOGGING
    return SERIAL.print(F("."));
#else
    return 0;
#endif
}

size_t printChar(const char *chr)
{
    return printChar(false, chr);
}

size_t printChar(bool safe, const char *chr)
{
#ifndef DISABLE_LOGGING
    return SERIAL.println(chr);
#else
    return 0;
#endif
}

size_t printCR()
{
    return printCR(false);
}

size_t printCR(bool safe)
{
#ifndef DISABLE_LOGGING
    return SERIAL.println();
#else
    return 0;
#endif
}

void flush()
{
    flush(false);
}

void flush(bool safe)
{
    SERIAL.flush();
}

size_t myPrint(const __FlashStringHelper *ifsh)
{
    return SERIAL.print(ifsh);
}

size_t myPrint(const String &s)
{
    return SERIAL.print(s);
}

size_t myPrint(const char str[])
{
    return SERIAL.print(str);
}

size_t myPrint(char c)
{
    return SERIAL.print(c);
}

size_t myPrint(unsigned char b, int base)
{
    return SERIAL.print(b, base);
}

size_t myPrint(int n, int base)
{
    return SERIAL.print(n, base);
}

size_t myPrint(unsigned int n, int base)
{
    return SERIAL.print(n, base);
}

size_t myPrint(long n, int base)
{
    return SERIAL.print(n, base);
}

size_t myPrint(unsigned long n, int base)
{
    return SERIAL.print(n, base);
}

size_t myPrint(double n, int digits)
{
    return SERIAL.print(n, digits);
}

size_t myPrint(const Printable &x)
{
    return SERIAL.print(x);
}

size_t myPrint(struct tm *timeinfo, const char *format)
{
#ifdef ESP32
    return SERIAL.print(timeinfo, format);
#else
    return 0;
#endif
}

size_t myPrintln(const __FlashStringHelper *ifsh)
{
    return SERIAL.println(ifsh);
}

size_t myPrintln(void)
{
    return SERIAL.println();
}

size_t myPrintln(const String &s)
{
    return SERIAL.println(s);
}

size_t myPrintln(const char c[])
{
    return SERIAL.println(c);
}

size_t myPrintln(char c)
{
    return SERIAL.println(c);
}

size_t myPrintln(unsigned char b, int base)
{
    return SERIAL.println(b, base);
}

size_t myPrintln(int num, int base)
{
    return SERIAL.println(num, base);
}

size_t myPrintln(unsigned int num, int base)
{
    return SERIAL.println(num, base);
}

size_t myPrintln(long num, int base)
{
    return SERIAL.println(num, base);
}

size_t myPrintln(unsigned long num, int base)
{
    return SERIAL.println(num, base);
}

size_t myPrintln(double num, int digits)
{
    return SERIAL.println(num, digits);
}

size_t myPrintln(const Printable &x)
{
    return SERIAL.println(x);
}

size_t myPrintln(struct tm *timeinfo, const char *format)
{
#ifdef ESP32
    return SERIAL.println(timeinfo, format);
#else
    return 0;
#endif
}
