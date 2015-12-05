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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include <bcm_host.h>
#pragma GCC diagnostic pop

#include "font.h"
#include "temperatureTrace.h"

//-------------------------------------------------------------------------

int8_t
CTemperatureTrace::
getTemperature()
{
    double temperature = 0.0;

    char buffer[128];

    memset(buffer, 0, sizeof(buffer));

    if (vc_gencmd(buffer, sizeof(buffer), "measure_temp") == 0)
    {
        sscanf(buffer, "temp=%lf'C", &temperature);
    }

    return (int8_t)(temperature + 0.5);
}

//-------------------------------------------------------------------------

CTemperatureTrace::
CTemperatureTrace(
    uint16_t width,
    uint16_t traceHeight,
    uint16_t yPosition,
    uint16_t gridHeight)
:
    CTrace(width,
           traceHeight,
           yPosition,
           gridHeight,
           1,
           "Temperature",
           std::vector<std::string>{"temperature"},
           std::vector<CRGB565>{{102,167,225}}),
    m_traceHeight(traceHeight)
{
}

//-------------------------------------------------------------------------

void
CTemperatureTrace::
show(
    const CFrameBuffer565& fb,
    time_t now)
{
    int8_t temperature = (getTemperature() * m_traceHeight) / 100;

    update(std::vector<int8_t>{temperature}, now);

    putImage(fb);
}

