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
#include <iostream>

#include "image565Font8x16.h"
#include "image565Graphics.h"
#include "panel.h"
#include "rgb565.h"
#include "trace.h"

//-------------------------------------------------------------------------

const raspifb16::RGB565 Trace::sc_foreground{255, 255, 255};
const raspifb16::RGB565 Trace::sc_background{0, 0, 0};
const raspifb16::RGB565 Trace::sc_gridColour{48, 48, 48};

//-------------------------------------------------------------------------

Trace::Trace(
    int width,
    int traceHeight,
    int fontHeight,
    int traceScale,
    int yPosition,
    int gridHeight,
    int traces,
    const std::string& title,
    const std::vector<std::string>& traceNames,
    const std::vector<raspifb16::RGB565>& traceColours)
:
    Panel(width, traceHeight + fontHeight + 4, yPosition),
    m_traceHeight{traceHeight},
    m_fontHeight{fontHeight},
    m_traceScale{traceScale},
    m_gridHeight{gridHeight},
    m_columns{0},
    m_title{title},
    m_autoScale{traceScale == 0},
    m_traceData(),
    m_time(width)
{
    for (int trace = 0 ; trace < traces ; ++trace)
    {

        TraceData traceData =
        {
            traceNames[trace],
            traceColours[trace],
            raspifb16::RGB565::blend(63,
                                     sc_gridColour,
                                     traceColours[trace]),
        };
        traceData.m_values.resize(width, 0);

        m_traceData.push_back(traceData);
    }
}

//-------------------------------------------------------------------------

void
Trace::init(
    raspifb16::Interface565Font& font)
{
    getImage().clear(sc_background);

    raspifb16::Interface565Point position(0, m_traceHeight + 2);

    position = font.drawString(
                   raspifb16::Interface565Point(0, m_traceHeight + 2),
                   m_title + " (",
                   sc_foreground,
                   getImage());

    bool first = true;
    for (auto& trace : m_traceData)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            position.setX(position.x() + font.getPixelWidth());
        }

        position = font.drawString(position,
                                   trace.m_name + ":",
                                   sc_foreground,
                                   getImage());

        // draw small box

        const auto width = font.getPixelWidth();
        const auto height = font.getPixelHeight();

        raspifb16::Interface565Point p1{position.x(), position.y() + height / 4};
        raspifb16::Interface565Point p2{position.x() + width / 2,
                                        position.y() + (3 * height) / 4};

        boxFilled(getImage(), p1, p2, trace.m_traceColour);
        position.setX(position.x() + font.getPixelWidth());
    }

    position = font.drawString(position,
                               ")",
                               sc_foreground,
                               getImage());

    for (auto j = 0 ; j < m_traceHeight + 1 ; j+= m_gridHeight)
    {
        horizontalLine(getImage(),
                       0,
                       getImage().getWidth() - 1,
                       j,
                       sc_gridColour);
    }
}

//-------------------------------------------------------------------------

void
Trace::addData(
    const std::vector<int>& data,
    time_t now)
{
    int index{0};

    if (m_columns < getImage().getWidth())
    {
        index = m_columns++;
    }
    else
    {
        index = m_columns - 1;

        for (auto& trace : m_traceData)
        {
            std::rotate(trace.m_values.begin(),
                        trace.m_values.begin() + 1,
                        trace.m_values.end());
        }

        std::rotate(m_time.begin(), m_time.begin() + 1, m_time.end());
    }

    //-----------------------------------------------------------------

    auto value = data.begin();
    for (auto& trace : m_traceData)
    {
        trace.m_values[index] = *(value++);
    }

    m_time[index] = now % 60;

    //-----------------------------------------------------------------

    if (m_autoScale)
    {
        m_traceScale = 0;

        for (auto& trace : m_traceData)
        {
            int max = *max_element(trace.m_values.begin(),
                                       trace.m_values.end());

            m_traceScale = std::max(m_traceScale, max);
        }

        if (m_traceScale == 0)
        {
            m_traceScale = 1;
        }
    }

    //-----------------------------------------------------------------

    draw();
}

