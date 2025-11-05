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

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "traceStack.h"
#include "rgb565.h"

//-------------------------------------------------------------------------

TraceStack::TraceStack(
    int width,
    int traceHeight,
    int fontHeight,
    int traceScale,
    int yPosition,
    int gridHeight,
    const std::string& title,
    std::initializer_list<TraceConfiguration> traces)

:
    Trace(
        width,
        traceHeight,
        fontHeight,
        traceScale,
        yPosition,
        gridHeight,
        title,
        traces)
{
}

//-------------------------------------------------------------------------

void
TraceStack::draw()
{
    for (int i = 0 ; i < m_columns ; ++i)
    {
        int j = m_traceHeight - 1;

        for (auto& trace : m_traceData)
        {
            int value = (trace.value(i) * m_traceHeight)
                          / m_traceScale;

            for (int v = 0 ; v < value ; ++v)
            {
                if (((j % m_gridHeight) == 0) or ((m_time[i] % 60) == 0))
                {
                    getImage().setPixelRGB(
                        raspifb16::Interface565Point{i, j--},
                        trace.gridColour());
                }
                else
                {
                    getImage().setPixelRGB(
                        raspifb16::Interface565Point{i, j--},
                        trace.traceColour());
                }
            }
        }

        for ( ; j >= 0 ; --j)
        {
            if (((j % m_gridHeight) == 0) or ((m_time[i] % 60) == 0))
            {
                getImage().setPixelRGB(
                    raspifb16::Interface565Point{i, j},
                    sc_gridColour);
            }
            else
            {
                getImage().setPixelRGB(
                    raspifb16::Interface565Point{i, j},
                    sc_background);
            }
        }
    }
}

