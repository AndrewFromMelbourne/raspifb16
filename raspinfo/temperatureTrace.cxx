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

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <inttypes.h>
#include <unistd.h>

#include "bcm_host.h"

#include "font.h"
#include "temperatureTrace.h"

//-------------------------------------------------------------------------

int8_t
CTemperatureTrace::
getTemperature()
{
    double temperature = 0.0;

    char buffer[128];

    memset(buffer, 0, sizeof(buffer));

    if (vc_gencmd(buffer, sizeof(buffer), "measure_temp") == 0)
    {
        sscanf(buffer, "temp=%lf'C", &temperature);
    }

    return (int8_t)(temperature + 0.5);
}

//-------------------------------------------------------------------------

CTemperatureTrace::
CTemperatureTrace(
    int16_t width,
    int16_t traceHeight,
    int16_t yPosition,
    int16_t gridHeight)
:
    m_traceHeight(traceHeight),
    m_yPosition(yPosition),
    m_gridHeight(gridHeight),
    m_values(0),
    m_temperature(width),
    m_time(width),
    m_image(width, traceHeight + sc_fontHeight + 4),
    m_graphColour(102, 167, 225),
    m_graphGridColour(102, 167, 225),
    m_foreground(255, 255, 255),
    m_background(0, 0, 0),
    m_gridColour(48, 48, 48)
{
    m_graphGridColour = CRGB565::blend(63, m_gridColour, m_graphColour);

    //---------------------------------------------------------------------

    m_image.clear(m_background);

    uint8_t smallSquare = 0xFE;

    SFontPosition position = 
        drawString(0,
                   m_image.getHeight() - 2 - sc_fontHeight,
                   "Temperature",
                   m_foreground,
                   m_image);

    position = drawString(position.x,
                          position.y,
                          " (temperature:",
                          m_foreground,
                          m_image);

    position = drawChar(position.x,
                        position.y,
                        smallSquare,
                        m_graphColour,
                        m_image);

    position = drawString(position.x,
                          position.y,
                          ")",
                          m_foreground,
                          m_image);

    for (int32_t j = 0 ; j < traceHeight + 1 ; j+= m_gridHeight)
    {
        for (int32_t i = 0 ; i < m_image.getWidth() ;  ++i)
        {
            m_image.setPixel(i, j, m_gridColour);
        }
    }
}

//-------------------------------------------------------------------------

void
CTemperatureTrace::
show(
    const CFrameBuffer565& fb,
    time_t now)
{
    int8_t temperature = (getTemperature() * m_traceHeight) / 100;
    int16_t index;

    if (m_values < m_image.getWidth())
    {
        index = m_values++;
    }
    else
    {
        index = m_image.getWidth() - 1;

        rotate(m_temperature.begin(),
               m_temperature.begin() + 1,
               m_temperature.end());

        rotate(m_time.begin(), m_time.begin() + 1, m_time.end());
    }

    m_temperature[index] = temperature;
    m_time[index] = now % 60;

    //-----------------------------------------------------------------

    for (int16_t i = 0 ; i < m_values ; ++i)
    {
        int16_t j = m_traceHeight - 1;

        for (int16_t t = 0 ; t < m_temperature[i] ; ++t)
        {
            if (((j % m_gridHeight) == 0) || (m_time[i] == 0))
            {
                m_image.setPixel(i, j--, m_graphGridColour);
            }
            else
            {
                m_image.setPixel(i, j--, m_graphColour);
            }
        }

        for ( ; j >= 0 ; --j)
        {
            if (((j % m_gridHeight) == 0) || (m_time[i] == 0))
            {
                m_image.setPixel(i, j, m_gridColour);
            }
            else
            {
                m_image.setPixel(i, j, m_background);
            }
        }
    }

    fb.putImage(0, m_yPosition, m_image);
}

