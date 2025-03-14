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
#include <vector>

#include <sys/time.h>

#include "traceStack.h"

//-------------------------------------------------------------------------

class CpuStats
{
public:

    CpuStats();

    [[nodiscard]] int total() const noexcept;
    [[nodiscard]] int user() const noexcept { return m_user; }
    [[nodiscard]] int nice() const noexcept { return m_nice; }
    [[nodiscard]] int system() const noexcept { return m_system; }

    CpuStats& operator-=(const CpuStats& rhs) noexcept;

private:

    int m_user;
    int m_nice;
    int m_system;
    int m_idle;
    int m_iowait;
    int m_irq;
    int m_softirq;
    int m_steal;
    int m_guest;
    int m_guest_nice;
};

CpuStats operator-(const CpuStats& lhs, const CpuStats& rhs) noexcept;

//-------------------------------------------------------------------------

class CpuTrace
:
    public TraceStack
{
public:

    CpuTrace(
        int width,
        int traceHeight,
        int fontHeight,
        int yPosition,
        int gridHeight = 20);

    void update(time_t now, raspifb16::Interface565Font& font) override;

private:

    CpuStats m_previousStats;
};

//-------------------------------------------------------------------------

