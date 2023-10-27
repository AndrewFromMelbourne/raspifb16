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

#pragma once

//-------------------------------------------------------------------------

#include <cstdint>
#include <string>
#include <vector>

#include <sys/time.h>

#include "panel.h"
#include "rgb565.h"

//-------------------------------------------------------------------------

namespace raspifb16
{
class FrameBuffer565;
}

//-------------------------------------------------------------------------

struct TraceData
{
    std::string m_name;
    raspifb16::RGB565 m_traceColour;
    raspifb16::RGB565 m_gridColour;
    std::vector<int16_t> m_values;
};

//-------------------------------------------------------------------------

class Trace
:
    public Panel
{
public:

    Trace(
        int16_t width,
        int16_t traceHeight,
        int16_t traceScale,
        int16_t yPosition,
        int16_t gridHeight,
        int16_t traces,
        const std::string& title,
        const std::vector<std::string>& traceNames,
        const std::vector<raspifb16::RGB565>& traceColours);

    static int16_t getLegendHeight();

    void update(time_t now) override = 0;

protected:

    void addData(const std::vector<int16_t>& data, time_t now);
    virtual void draw() = 0;

    int16_t m_traceHeight;
    int16_t m_traceScale;
    int16_t m_gridHeight;
    int16_t m_columns;

    bool m_autoScale;

    std::vector<TraceData> m_traceData;
    std::vector<int8_t> m_time;

    static const raspifb16::RGB565 sc_foreground;
    static const raspifb16::RGB565 sc_background;
    static const raspifb16::RGB565 sc_gridColour;
};

//-------------------------------------------------------------------------

