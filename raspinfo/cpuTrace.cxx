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

#include <unistd.h>

#include "font.h"
#include "cpuTrace.h"

//-------------------------------------------------------------------------

void
CCpuStats::
read()
{
    std::ifstream ifs{"/proc/stat", std::ifstream::in};

    if (ifs.is_open() == false)
    {
        throw std::system_error{errno,
                                std::system_category(),
                                "unable to open /proc/stat"};
    }

    std::string id;
    ifs >> id;

    if (id != "cpu")
    {
        throw std::logic_error{
            "reading /proc/stat expected \"cpu\", but found " + id};
    }

    ifs
        >> m_user
        >> m_nice
        >> m_system
        >> m_idle
        >> m_iowait
        >> m_irq
        >> m_softirq
        >> m_steal
        >> m_guest
        >> m_guest_nice;
}

//-------------------------------------------------------------------------

uint32_t
CCpuStats::
total() const
{
    return m_user +
           m_nice +
           m_system +
           m_idle +
           m_iowait +
           m_irq +
           m_softirq +
           m_steal +
           m_guest +
           m_guest_nice;
}

//-------------------------------------------------------------------------

CCpuStats&
CCpuStats::
operator-=(
    const CCpuStats& rhs)
{
    m_user -= rhs.m_user;
    m_nice -= rhs.m_nice;
    m_system -= rhs.m_system;
    m_idle -= rhs.m_idle;
    m_iowait -= rhs.m_iowait;
    m_irq -= rhs.m_irq;
    m_softirq -= rhs.m_softirq;
    m_steal -= rhs.m_steal;
    m_guest -= rhs.m_guest;
    m_guest_nice -= rhs.m_guest_nice;

    return *this;
}

//-------------------------------------------------------------------------

CCpuStats
operator-(
    const CCpuStats& lhs,
    const CCpuStats& rhs)
{
    return CCpuStats(lhs) -= rhs;
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
           std::vector<raspifb16::CRGB565>{{4,90,141},
                                           {116,169,207},
                                           {241,238,246}}),
    m_traceHeight{traceHeight}
{
    m_currentStats.read();
}

//-------------------------------------------------------------------------

void
CCpuTrace::
show(
    const raspifb16::CFrameBuffer565& fb,
    time_t now)
{
    m_previousStats = m_currentStats;

    m_currentStats.read();

    CCpuStats diff = m_currentStats -  m_previousStats;

    uint32_t totalCpu = diff.total();

    int8_t user = (diff.user() * m_traceHeight) / totalCpu;
    int8_t nice = (diff.nice() * m_traceHeight) / totalCpu;
    int8_t system = (diff.system() * m_traceHeight) / totalCpu;

    update(std::vector<int8_t>{user, nice, system}, now);

    putImage(fb);
}

