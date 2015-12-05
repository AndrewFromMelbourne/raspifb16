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
#include "rgb565.h"
#include "trace.h"

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
:
    public CTrace
{
public:

    CCpuTrace(
        uint16_t width,
        uint16_t traceHeight,
        uint16_t yPosition,
        uint16_t gridHeight = 20);

    virtual void show(const CFrameBuffer565& fb, time_t now) override;

private:

    uint16_t m_traceHeight;

    SCpuStats m_currentStats;
    SCpuStats m_previousStats;

    static void getCpuStats(SCpuStats& cpuStats);

    static SCpuStats
    diffCpuStats(const SCpuStats& lhs,
                 const SCpuStats& rhs);
};

//-------------------------------------------------------------------------

#endif

