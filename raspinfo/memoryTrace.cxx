//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2015 Andrew Duncan
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//-------------------------------------------------------------------------

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <inttypes.h>
#include <unistd.h>

#include "font.h"
#include "memoryTrace.h"

//-------------------------------------------------------------------------

void
CMemoryTrace::
getMemoryStats(
    SMemoryStats& memoryStats)
{
    FILE *fp = fopen("/proc/meminfo", "r");

    if (fp == 0)
    {
        perror("unable to open /proc/stat");
        exit(EXIT_FAILURE);
    }

    char buffer[128];

    while (fgets(buffer, sizeof(buffer), fp))
    {
        char name[64];
        uint32_t value;

        if (sscanf(buffer, "%[a-zA-Z]: %" SCNu32 " kB", name, &value) == 2)
        {
            if (strcmp(name, "MemTotal") == 0)
            {
                memoryStats.total = value;
            }
            else if (strcmp(name, "MemFree") == 0)
            {
                memoryStats.free = value;
            }
            else if (strcmp(name, "Buffers") == 0)
            {
                memoryStats.buffers = value;
            }
            else if (strcmp(name, "Cached") == 0)
            {
                memoryStats.cached = value;
            }
        }
    }

    fclose(fp);

    memoryStats.used = memoryStats.total
                     - memoryStats.free
                     - memoryStats.buffers
                     - memoryStats.cached;
}

//-------------------------------------------------------------------------

CMemoryTrace::
CMemoryTrace(
    uint16_t width,
    uint16_t traceHeight,
    uint16_t yPosition,
    uint16_t gridHeight)
:
    CTrace(width,
           traceHeight,
           yPosition,
           gridHeight,
           3,
           "Memory",
           std::vector<std::string>{"used", "buffers", "cached"},
           std::vector<CRGB565>{{0,109,44},{102,194,164},{237,248,251}}),
    m_traceHeight(traceHeight)
{
}

//-------------------------------------------------------------------------

void
CMemoryTrace::
show(
    const CFrameBuffer565& fb,
    time_t now)
{
    SMemoryStats memoryStats;
    getMemoryStats(memoryStats);

    int8_t used = (memoryStats.used * m_traceHeight) / memoryStats.total;
    int8_t buffers = (memoryStats.buffers * m_traceHeight)
                   / memoryStats.total;
    int8_t cached = (memoryStats.cached * m_traceHeight)
                 / memoryStats.total;

    update(std::vector<int8_t>{used, buffers, cached}, now);

    putImage(fb);
}

