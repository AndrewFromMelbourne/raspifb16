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

#include "panel.h"
#include "traceStack.h"

//-------------------------------------------------------------------------

class MemoryStats
{
public:

    MemoryStats();

    int total() const noexcept { return m_total; }
    int buffers() const noexcept { return m_buffers; }
    int cached() const noexcept { return m_cached; }
    int used() const noexcept { return m_used; }

private:

    int m_total;
    int m_buffers;
    int m_cached;
    int m_used;
};

//-------------------------------------------------------------------------

class MemoryTrace
:
    public TraceStack
{
public:

    MemoryTrace(
        int width,
        int traceHeight,
        int fontHeight,
        int yPosition,
        int gridHeight = 20);

    void update(time_t now, raspifb16::Interface565Font& font) override;
};

//-------------------------------------------------------------------------

