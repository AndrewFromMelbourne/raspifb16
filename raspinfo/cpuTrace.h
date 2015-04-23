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

#ifndef CPU_TRACE_H
#define CPU_TRACE_H

//-------------------------------------------------------------------------

#include <cstdint>
#include <vector>

#include <sys/time.h>

#include "framebuffer565.h"
#include "image565.h"
#include "rgb565.h"

//-------------------------------------------------------------------------

struct SCpuStats
{
    uint32_t user;
    uint32_t nice;
    uint32_t system;
    uint32_t idle;
    uint32_t iowait;
    uint32_t irq;
    uint32_t softirq;
    uint32_t steal;
    uint32_t guest;
    uint32_t guest_nice;
};

//-------------------------------------------------------------------------

class CCpuTrace
{
public:

    CCpuTrace(int16_t width,
              int16_t traceHeight,
              int16_t yPosition,
              int16_t gridHeight = 20);

    int16_t getBottom() const { return m_yPosition + m_image.getHeight(); }

    void show(const CFrameBuffer565& fb, time_t now);

private:

    int16_t m_traceHeight;
    int16_t m_yPosition;
    int16_t m_gridHeight;
    int16_t m_values;
    std::vector<int8_t> m_user;
    std::vector<int8_t> m_nice;
    std::vector<int8_t> m_system;
    std::vector<int8_t> m_time;
    SCpuStats m_currentStats;
    SCpuStats m_previousStats;
    CImage565 m_image;
    CRGB565 m_userColour;
    CRGB565 m_userGridColour;
    CRGB565 m_niceColour;
    CRGB565 m_niceGridColour;
    CRGB565 m_systemColour;
    CRGB565 m_systemGridColour;
    CRGB565 m_foreground;
    CRGB565 m_background;
    CRGB565 m_gridColour;

    static void getCpuStats(SCpuStats& cpuStats);
    static void diffCpuStats(const SCpuStats& lhs,
                             const SCpuStats& rhs,
                             SCpuStats& result);
};

//-------------------------------------------------------------------------

#endif

