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

#ifndef MEMORY_TRACE_H
#define MEMORY_TRACE_H

//-------------------------------------------------------------------------

#include <cstdint>
#include <vector>

#include "panel.h"
#include "traceStack.h"

//-------------------------------------------------------------------------

namespace raspifb16
{
class FrameBuffer565;
}

//-------------------------------------------------------------------------

class MemoryStats
{
public:

    MemoryStats();

    uint32_t total() const { return m_total; }
    uint32_t buffers() const { return m_buffers; }
    uint32_t cached() const { return m_cached; }
    uint32_t used() const { return m_used; }

private:

    uint32_t m_total;
    uint32_t m_buffers;
    uint32_t m_cached;
    uint32_t m_used;
};

//-------------------------------------------------------------------------

class MemoryTrace
:
    public TraceStack
{
public:

    MemoryTrace(
        int16_t width,
        int16_t traceHeight,
        int16_t yPosition,
        int16_t gridHeight = 20);

    virtual void update(time_t now) override;
};

//-------------------------------------------------------------------------

#endif

