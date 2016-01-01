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
#include <stdexcept>
#include <string>
#include <system_error>

#include <ifaddrs.h>
#include <netdb.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <linux/if_link.h>
#include <sys/socket.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include <bcm_host.h>
#pragma GCC diagnostic pop

#include "system.h"
#include "networkTrace.h"

//-------------------------------------------------------------------------

CNetworkStats::
CNetworkStats()
:
    m_tx{0},
    m_rx{0}
{
    struct ifaddrs *ifaddr;

    if (getifaddrs(&ifaddr) == -1)
    {
        throw std::system_error(errno,
                                std::system_category(),
                                "getifaddrs");
    }

    struct ifaddrs *ifa = ifaddr;
    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr != nullptr)
        {
            int family = ifa->ifa_addr->sa_family;

            if ((family == AF_PACKET) &&
                (std::string(ifa->ifa_name) != "lo"))
            {
                rtnl_link_stats* stats =
                    static_cast<rtnl_link_stats*>(ifa->ifa_data);

                m_tx += stats->tx_bytes;
                m_rx += stats->rx_bytes;
            }
        }
    }

    freeifaddrs(ifaddr);
}

//-------------------------------------------------------------------------

CNetworkStats&
CNetworkStats::
operator-=(
    const CNetworkStats& rhs)
{
    m_tx -= rhs.m_tx;
    m_rx -= rhs.m_rx;

    return *this;
}

//-------------------------------------------------------------------------

CNetworkStats
operator-(
    const CNetworkStats& lhs,
    const CNetworkStats& rhs)
{
    return CNetworkStats(lhs) -= rhs;
}

//-------------------------------------------------------------------------

CNetworkTrace::
CNetworkTrace(
    int16_t width,
    int16_t traceHeight,
    int16_t yPosition,
    int16_t gridHeight)
:
    CTraceGraph(
        width,
        traceHeight,
        0,
        yPosition,
        gridHeight,
        2,
        "Network",
        std::vector<std::string>{"tx", "rx"},
        std::vector<raspifb16::CRGB565>{{102,167,225}, {102,225,167}}),
    m_previousStats{}
{
}

//-------------------------------------------------------------------------

void
CNetworkTrace::
update(
    time_t now)
{
    CNetworkStats currentStats;

    CNetworkStats diff{currentStats - m_previousStats};

    int16_t tx = diff.tx();
    int16_t rx = diff.rx();

    CTrace::addData(std::vector<int16_t>{tx, rx}, now);

    m_previousStats = currentStats;
}

