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
    fscanf(fp, "%" SCNu32, &(cpuStats.user));
    fscanf(fp, "%" SCNu32, &(cpuStats.nice));
    fscanf(fp, "%" SCNu32, &(cpuStats.system));
    fscanf(fp, "%" SCNu32, &(cpuStats.idle));
    fscanf(fp, "%" SCNu32, &(cpuStats.iowait));
    fscanf(fp, "%" SCNu32, &(cpuStats.irq));
    fscanf(fp, "%" SCNu32, &(cpuStats.softirq));
    fscanf(fp, "%" SCNu32, &(cpuStats.steal));
    fscanf(fp, "%" SCNu32, &(cpuStats.guest));
    fscanf(fp, "%" SCNu32, &(cpuStats.guest_nice));

    fclose(fp);
}

//-------------------------------------------------------------------------

SCpuStats
CCpuTrace::
diffCpuStats(
    const SCpuStats& lhs,
    const SCpuStats& rhs)
{
    SCpuStats result;

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

    return result;
}

//-------------------------------------------------------------------------


CCpuTrace::
CCpuTrace(
    int16_t width,
    int16_t traceHeight,
    int16_t yPosition,
    int16_t gridHeight)
:
    CTrace(width,
           traceHeight,
           yPosition,
           gridHeight,
           3,
           "CPU",
           std::vector<std::string>{"user", "nice", "system"},
           std::vector<CRGB565>{{4,90,141},{116,169,207},{241,238,246}}),
    m_traceHeight{traceHeight}
{
    getCpuStats(m_currentStats);
}

//-------------------------------------------------------------------------

void
CCpuTrace::
show(
    const CFrameBuffer565& fb,
    time_t now)
{
    m_previousStats = m_currentStats;

    getCpuStats(m_currentStats);

    SCpuStats diff = diffCpuStats(m_currentStats, m_previousStats);

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

    update(std::vector<int8_t>{user, nice, system}, now);

    putImage(fb);
}

