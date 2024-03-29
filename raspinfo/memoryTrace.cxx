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

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <system_error>

#include <inttypes.h>
#include <unistd.h>

#include "memoryTrace.h"

//-------------------------------------------------------------------------

MemoryStats::
MemoryStats()
:
    m_total{0},
    m_buffers{0},
    m_cached{0},
    m_used{0}
{
    std::ifstream ifs{"/proc/meminfo", std::ifstream::in};

    if (ifs.is_open() == false)
    {
        throw std::system_error{errno,
                                std::system_category(),
                                "unable to open /proc/meminfo"};
    }

    int free{0};

    while (ifs.eof() == false)
    {
        std::string name;
        int value;
        std::string unit;

        ifs >> name >> value >> unit;

        if (name == "MemTotal:")
        {
            m_total = value;
        }
        else if (name == "MemFree:")
        {
            free = value;
        }
        else if (name == "Buffers:")
        {
            m_buffers = value;
        }
        else if (name == "Cached:")
        {
            m_cached = value;
        }
    }

    m_used = m_total - free - m_buffers - m_cached;
}

//-------------------------------------------------------------------------

MemoryTrace::
MemoryTrace(
    int width,
    int traceHeight,
    int fontHeight,
    int yPosition,
    int gridHeight)
:
    TraceStack(
        width,
        traceHeight,
        fontHeight,
        100,
        yPosition,
        gridHeight,
        3,
        "Memory",
        std::vector<std::string>{"used", "buffers", "cached"},
        std::vector<raspifb16::RGB565>{{0, 109, 44},
                                       {102, 194, 164},
                                       {237, 248, 251}})
{
}

//-------------------------------------------------------------------------

void
MemoryTrace::
update(
    time_t now,
    raspifb16::Interface565Font& font)
{
    MemoryStats memoryStats;

    int used = (memoryStats.used() * m_traceScale)
                 / memoryStats.total();

    int buffers = (memoryStats.buffers() * m_traceScale)
                    / memoryStats.total();

    int cached = (memoryStats.cached() * m_traceScale)
                   / memoryStats.total();

    Trace::addData(std::vector<int>{used, buffers, cached}, now);
}

