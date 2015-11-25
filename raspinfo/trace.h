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

#ifndef TRACE_H
#define TRACE_H

//-------------------------------------------------------------------------

#include <cstdint>
#include <string>
#include <vector>

#include <sys/time.h>

#include "framebuffer565.h"
#include "panel.h"
#include "rgb565.h"

//-------------------------------------------------------------------------

struct STraceData
{
    std::string m_name;
    CRGB565 m_traceColour;
    CRGB565 m_gridColour;
    std::vector<int8_t> m_values;
};

//-------------------------------------------------------------------------

class CTrace
:
    public CPanel
{
public:

    CTrace(
        int16_t width,
        int16_t traceHeight,
        int16_t yPosition,
        int16_t gridHeight,
        int16_t traces,
        const std::string& title,
        const std::vector<std::string>& traceNames,
        const std::vector<CRGB565>& traceColours);

    virtual void show(const CFrameBuffer565& fb, time_t now) override = 0;

protected:

    void update(const std::vector<int8_t>& data, time_t now);

private:

    int16_t m_traceHeight;
    int16_t m_gridHeight;
    int16_t m_traces;
    int16_t m_columns;

    std::vector<STraceData> m_data;
    std::vector<int8_t> m_time;

    static const CRGB565 sc_foreground;
    static const CRGB565 sc_background;
    static const CRGB565 sc_gridColour;
};

//-------------------------------------------------------------------------

#endif

