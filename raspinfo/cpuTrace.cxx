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

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <inttypes.h>
#include <unistd.h>

#include "font.h"
#include "cpuTrace.h"

//-------------------------------------------------------------------------

void
CCpuTrace::
getCpuStats(
    SCpuStats& cpuStats)
{
    FILE *fp = fopen("/proc/stat", "r");

    if (fp == 0)
    {
        perror("unable to open /proc/stat");
        exit(EXIT_FAILURE);
    }

    fscanf(fp, "%*s");
    fscanf(fp, "%"SCNu32, &(cpuStats.user));
    fscanf(fp, "%"SCNu32, &(cpuStats.nice));
    fscanf(fp, "%"SCNu32, &(cpuStats.system));
    fscanf(fp, "%"SCNu32, &(cpuStats.idle));
    fscanf(fp, "%"SCNu32, &(cpuStats.iowait));
    fscanf(fp, "%"SCNu32, &(cpuStats.irq));
    fscanf(fp, "%"SCNu32, &(cpuStats.softirq));
    fscanf(fp, "%"SCNu32, &(cpuStats.steal));
    fscanf(fp, "%"SCNu32, &(cpuStats.guest));
    fscanf(fp, "%"SCNu32, &(cpuStats.guest_nice));

    fclose(fp);
}

//-------------------------------------------------------------------------

void
CCpuTrace::
diffCpuStats(
    const SCpuStats& lhs,
    const SCpuStats& rhs,
    SCpuStats& result)
{
    result.user = lhs.user - rhs.user;
    result.nice = lhs.nice - rhs.nice;
    result.system = lhs.system - rhs.system;
    result.idle = lhs.idle - rhs.idle;
    result.iowait = lhs.iowait - rhs.iowait;
    result.irq = lhs.irq - rhs.irq;
    result.softirq = lhs.softirq - rhs.softirq;
    result.steal = lhs.steal - rhs.steal;
    result.guest = lhs.guest - rhs.guest;
    result.guest_nice = lhs.guest_nice - rhs.guest_nice;
}

//-------------------------------------------------------------------------


CCpuTrace::
CCpuTrace(
    int16_t width,
    int16_t traceHeight,
    int16_t yPosition,
    int16_t gridHeight)
:
    m_traceHeight(traceHeight),
    m_yPosition(yPosition),
    m_gridHeight(gridHeight),
    m_values(0),
    m_user(0),
    m_nice(0),
    m_system(0),
    m_time(0),
    m_image(width, traceHeight + sc_fontHeight + 4),
    m_userColour(4, 90, 141),
    m_userGridColour(4, 90, 141),
    m_niceColour(116, 169, 207),
    m_niceGridColour(116, 169, 207),
    m_systemColour(241, 238, 246),
    m_systemGridColour(241, 238, 246),
    m_foreground(255, 255, 255),
    m_background(0, 0, 0),
    m_gridColour(48, 48, 48)

{
    m_user = new int8_t[width];
    m_nice = new int8_t[width];
    m_system = new int8_t[width];
    m_time = new int8_t[width];

    m_userGridColour = CRGB565::blend(63, m_gridColour, m_userColour);
    m_niceGridColour = CRGB565::blend(63, m_gridColour, m_niceColour);
    m_systemGridColour = CRGB565::blend(63, m_gridColour, m_systemColour);

    //---------------------------------------------------------------------

    m_image.clear(m_background);

    uint8_t smallSquare = 0xFE;

    SFontPosition position = 
        drawString(0,
                   m_image.getHeight() - 2 - sc_fontHeight,
                   "CPU",
                   m_foreground,
                   m_image);

    position = drawString(position.x,
                          position.y,
                          " (user:",
                          m_foreground,
                          m_image);

    position = drawChar(position.x,
                        position.y,
                        smallSquare,
                        m_userColour,
                        m_image);

    position = drawString(position.x,
                          position.y,
                          " nice:",
                          m_foreground,
                          m_image);

    position = drawChar(position.x,
                        position.y,
                        smallSquare,
                        m_niceColour,
                        m_image);

    position = drawString(position.x,
                          position.y,
                          " system:",
                          m_foreground,
                          m_image);

    position = drawChar(position.x,
                        position.y,
                        smallSquare,
                        m_systemColour,
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

    //---------------------------------------------------------------------

    getCpuStats(m_currentStats);
}

//-------------------------------------------------------------------------

CCpuTrace::
~CCpuTrace()
{
    delete [] m_user;
    delete [] m_nice;
    delete [] m_system;
    delete [] m_time;
}

//-------------------------------------------------------------------------

void
CCpuTrace::
show(
    const CFrameBuffer565& fb,
    time_t now)
{
    SCpuStats diff;

    memcpy(&m_previousStats, &m_currentStats, sizeof(m_previousStats));

    getCpuStats(m_currentStats);

    diffCpuStats(m_currentStats, m_previousStats, diff);

    uint32_t totalCpu = diff.user
                      + diff.nice
                      + diff.system
                      + diff.idle
                      + diff.iowait
                      + diff.irq
                      + diff.softirq
                      + diff.steal
                      + diff.guest
                      + diff.guest_nice;

    int8_t user = (diff.user * m_traceHeight) / totalCpu;
    int8_t nice = (diff.nice * m_traceHeight) / totalCpu;
    int8_t system = (diff.system * m_traceHeight) / totalCpu;

    int16_t index;

    if (m_values < m_image.getWidth())
    {
        index = m_values++;
    }
    else
    {
        index = m_image.getWidth() - 1;

        memmove(&(m_user[0]), &(m_user[1]), index);
        memmove(&(m_nice[0]), &(m_nice[1]), index);
        memmove(&(m_system[0]), &(m_system[1]), index);
        memmove(&(m_time[0]), &(m_time[1]), index);
    }

    m_user[index] = user;
    m_nice[index] = nice;
    m_system[index] = system;
    m_time[index] = now % 60;

    //-----------------------------------------------------------------

    for (int16_t i = 0 ; i < m_values ; ++i)
    {
        int16_t j = m_traceHeight;

        for (int16_t u = 0 ; u < m_user[i] ; ++u)
        {
            if (((j % m_gridHeight) == 0) || (m_time[i] == 0))
            {
                m_image.setPixel(i, j--, m_userGridColour);
            }
            else
            {
                m_image.setPixel(i, j--, m_userColour);
            }
        }

        for (int16_t n = 0 ; n < m_nice[i] ; ++n)
        {
            if (((j % m_gridHeight) == 0) || (m_time[i] == 0))
            {
                m_image.setPixel(i, j--, m_niceGridColour);
            }
            else
            {
                m_image.setPixel(i, j--, m_niceColour);
            }
        }

        for (int16_t s = 0 ; s < m_system[i] ; ++s)
        {
            if (((j % m_gridHeight) == 0) || (m_time[i] == 0))
            {
                m_image.setPixel(i, j--, m_systemGridColour);
            }
            else
            {
                m_image.setPixel(i, j--, m_systemColour);
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

