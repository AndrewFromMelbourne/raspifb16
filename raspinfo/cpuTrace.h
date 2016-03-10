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

#include "traceStack.h"

//-------------------------------------------------------------------------

namespace raspifb16
{
class FrameBuffer565;
}

//-------------------------------------------------------------------------

class CpuStats
{
public:

    CpuStats();

    uint32_t total() const;
    uint32_t user() const { return m_user; }
    uint32_t nice() const { return m_nice; }
    uint32_t system() const { return m_system; }

    CpuStats& operator-=(const CpuStats& rhs);

private:

    uint32_t m_user;
    uint32_t m_nice;
    uint32_t m_system;
    uint32_t m_idle;
    uint32_t m_iowait;
    uint32_t m_irq;
    uint32_t m_softirq;
    uint32_t m_steal;
    uint32_t m_guest;
    uint32_t m_guest_nice;
};

CpuStats operator-(const CpuStats& lhs, const CpuStats& rhs);

//-------------------------------------------------------------------------

class CpuTrace
:
    public TraceStack
{
public:

    CpuTrace(
        int16_t width,
        int16_t traceHeight,
        int16_t yPosition,
        int16_t gridHeight = 20);

    void update(time_t now) override;

private:

    CpuStats m_previousStats;
};

//-------------------------------------------------------------------------

#endif

