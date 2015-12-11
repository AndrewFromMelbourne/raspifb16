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

//-------------------------------------------------------------------------

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "font.h"
#include "panel.h"
#include "rgb565.h"
#include "trace.h"

//-------------------------------------------------------------------------

const CRGB565 CTrace::sc_foreground{255, 255, 255};
const CRGB565 CTrace::sc_background{0, 0, 0};
const CRGB565 CTrace::sc_gridColour{48, 48, 48};

//-------------------------------------------------------------------------

CTrace::
CTrace(
    int16_t width,
    int16_t traceHeight,
    int16_t yPosition,
    int16_t gridHeight,
    int16_t traces,
    const std::string& title,
    const std::vector<std::string>& traceNames,
    const std::vector<CRGB565>& traceColours)
:
    CPanel(width, traceHeight + sc_fontHeight + 4, yPosition),
    m_traceHeight{traceHeight},
    m_gridHeight{gridHeight},
    m_columns{0},
    m_traceData(),
    m_time(width)
{
    for (int16_t trace = 0 ; trace < traces ; ++trace)
    {

        STraceData traceData =
        {
            traceNames[trace],
            traceColours[trace],
            CRGB565::blend(63, sc_gridColour, traceColours[trace]),
        };
        traceData.m_values.resize(width, 0);

        m_traceData.push_back(traceData);
    }

    //---------------------------------------------------------------------

    getImage().clear(sc_background);

    uint8_t smallSquare = 0xFE;

    SFontPosition position = 
        drawString(0,
                   getImage().getHeight() - 2 - sc_fontHeight,
                   title + " (",
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
            position.x += sc_fontWidth;
        }

        position = drawString(position.x,
                              position.y,
                              trace.m_name + ":",
                              sc_foreground,
                              getImage());

        position = drawChar(position.x,
                            position.y,
                            smallSquare,
                            trace.m_traceColour,
                            getImage());
    }

    position = drawString(position.x,
                          position.y,
                          ")",
                          sc_foreground,
                          getImage());

    for (int32_t j = 0 ; j < traceHeight + 1 ; j+= m_gridHeight)
    {
        for (int32_t i = 0 ; i < getImage().getWidth() ;  ++i)
        {
            getImage().setPixel(i, j, sc_gridColour);
        }
    }
}

//-------------------------------------------------------------------------

 void
 CTrace::
 update(
     const std::vector<int8_t>& data,
    time_t now)
{
    int16_t index{0};

    if (m_columns < getImage().getWidth())
    {
        index = m_columns++;
    }
    else
    {
        index = getImage().getWidth() - 1;

        for (auto& trace : m_traceData)
        {
            std::rotate(trace.m_values.begin(),
                        trace.m_values.begin() + 1,
                        trace.m_values.end());
        }
    }

    //-----------------------------------------------------------------

    auto value = data.begin();
    for (auto& trace : m_traceData)
    {
        trace.m_values[index] = *value;
    }

    m_time[index] = now % 60;

    //-----------------------------------------------------------------

    for (int16_t i = 0 ; i < m_columns ; ++i)
    {
        int16_t j = m_traceHeight - 1;

        for (auto& trace : m_traceData)
        {
            for (int16_t v = 0 ; v < trace.m_values[i] ; ++v)
            {
                if (((j % m_gridHeight) == 0) || (m_time[i] == 0))
                {
                    getImage().setPixel(i, j--, trace.m_gridColour);
                }
                else
                {
                    getImage().setPixel(i, j--, trace.m_traceColour);
                }
            }
        }

        for ( ; j >= 0 ; --j)
        {
            if (((j % m_gridHeight) == 0) || (m_time[i] == 0))
            {
                getImage().setPixel(i, j, sc_gridColour);
            }
            else
            {
                getImage().setPixel(i, j, sc_background);
            }
        }
    }
}
 
