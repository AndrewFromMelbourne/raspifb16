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

#include <cmath>
#include <cstdint>
#include <iomanip>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

#include <ifaddrs.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include <bcm_host.h>
#pragma GCC diagnostic pop

#include "font.h"
#include "dynamicInfo.h"
#include "system.h"

//-------------------------------------------------------------------------

std::string
CDynamicInfo::
getIpAddress(
    char& interface)
{
    struct ifaddrs *ifaddr = nullptr;
    struct ifaddrs *ifa = nullptr;
    interface = 'X';

    std::string address = "   .   .   .   ";

    getifaddrs(&ifaddr);

    for (ifa = ifaddr ; ifa != nullptr ; ifa = ifa->ifa_next)
    {
        if (ifa ->ifa_addr->sa_family == AF_INET)
        {
            void *addr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;

            if (strcmp(ifa->ifa_name, "lo") != 0)
            {
                char buffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, addr, buffer, sizeof(buffer));
                address = buffer;
                interface = ifa->ifa_name[0];
                break;
            }
        }
    }

    if (ifaddr != nullptr)
    {
        freeifaddrs(ifaddr);
    }

    return address;
}

//-------------------------------------------------------------------------

std::string
CDynamicInfo::
getMemorySplit()
{
    std::string result = " / ";

    int arm_mem = 0;
    int gpu_mem = 0;

    char buffer[128];

    memset(buffer, 0, sizeof(buffer));

    if (vc_gencmd(buffer, sizeof(buffer), "get_mem arm") == 0)
    {
        try
        {
            std::regex pattern{R"(arm=(\d+)M)"};
            std::smatch match;

            if (std::regex_search(std::string(buffer), match, pattern) &&
                (match.size() == 2))
            {
                arm_mem = std::stoi(match[1].str());
            }
        }
        catch (std::exception&)
        {
            // ignore
        }
    }

    if (vc_gencmd(buffer, sizeof(buffer), "get_mem gpu") == 0)
    {
        try
        {
            std::regex pattern{R"(gpu=(\d+)M)"};
            std::smatch match;

            if (std::regex_search(std::string(buffer), match, pattern) &&
                (match.size() == 2))
            {
                gpu_mem = std::stoi(match[1].str());
            }
        }
        catch (std::exception&)
        {
            // ignore
        }
    }

    if ((arm_mem != 0) && (gpu_mem != 0))
    {
        result = std::to_string(gpu_mem) + "/" + std::to_string(arm_mem);
    }

    return result;
}

//-------------------------------------------------------------------------

std::string
CDynamicInfo::
getTemperature()
{
    return std::to_string(raspinfo::getTemperature());
}

//-------------------------------------------------------------------------

std::string
CDynamicInfo::
getTime(
    time_t now)
{
    char buffer[128];

    struct tm result;
    struct tm *lt = localtime_r(&now, &result);
    std::strftime(buffer, sizeof(buffer), "%T", lt);

    return buffer;
}

//-------------------------------------------------------------------------

CDynamicInfo::
CDynamicInfo(
    int16_t width,
    int16_t yPosition)
:
    CPanel{width, 2 * (raspifb16::sc_fontHeight + 4), yPosition},
    m_heading(255, 255, 0),
    m_foreground(255, 255, 255),
    m_background(0, 0, 0),
    m_memorySplit(getMemorySplit())
{
}

//-------------------------------------------------------------------------

void
CDynamicInfo::
show(
    const raspifb16::CFrameBuffer565& fb,
    time_t now)
{
    getImage().clear(m_background);

    //---------------------------------------------------------------------

    raspifb16::SFontPosition position = { 0, 0 };

    position = drawString(position.x,
                          position.y,
                          "ip(",
                          m_heading,
                          getImage());

    char interface = ' ';
    std::string ipaddress = getIpAddress(interface);

    position = drawChar(position.x,
                        position.y,
                        interface,
                        m_foreground,
                        getImage());

    position = drawString(position.x,
                          position.y,
                          ") ",
                          m_heading,
                          getImage());

    position = drawString(position.x,
                          position.y,
                          ipaddress,
                          m_foreground,
                          getImage());

    position = drawString(position.x,
                          position.y,
                          " memory ",
                          m_heading,
                          getImage());

    position = drawString(position.x,
                          position.y,
                          m_memorySplit,
                          m_foreground,
                          getImage());

    position = drawString(position.x,
                          position.y,
                          " MB",
                          m_foreground,
                          getImage());

    //---------------------------------------------------------------------

    position.x = 0;
    position.y += raspifb16::sc_fontHeight + 4;

    position = drawString(position.x,
                          position.y,
                          "time ",
                          m_heading,
                          getImage());

    std::string timeString = getTime(now);

    position = drawString(position.x,
                          position.y,
                          timeString,
                          m_foreground,
                          getImage());

    position = drawString(position.x,
                          position.y,
                          " temperature ",
                          m_heading,
                          getImage());

    std::string temperatureString = getTemperature();

    position = drawString(position.x,
                          position.y,
                          temperatureString,
                          m_foreground,
                          getImage());

    uint8_t degreeSymbol = 0xF8;

    position = drawChar(position.x,
                        position.y,
                        degreeSymbol,
                        m_foreground,
                        getImage());


    position = drawString(position.x,
                          position.y,
                          "C",
                          m_foreground,
                          getImage());

    //---------------------------------------------------------------------
    
    putImage(fb);
}

