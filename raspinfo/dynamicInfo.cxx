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
#include <cstring>
#include <iomanip>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

#include <fcntl.h>
#include <ifaddrs.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "dynamicInfo.h"
#include "fileDescriptor.h"
#include "image565Font8x16.h"
#include "system.h"

//=========================================================================

namespace
{

//-------------------------------------------------------------------------

std::string
getIpAddress(
    char& interface)
{

    interface = 'X';
    std::string address{"   .   .   .   "};
    ifaddrs *ifaddr{};

    ::getifaddrs(&ifaddr);

    if (not ifaddr)
    {
        return address;
    }

    for (ifaddrs *ifa = ifaddr ; ifa != nullptr ; ifa = ifa->ifa_next)
    {
        if (ifa ->ifa_addr->sa_family == AF_INET)
        {
            void *addr = &((sockaddr_in *)ifa->ifa_addr)->sin_addr;

            if (std::string(ifa->ifa_name) != std::string("lo"))
            {
                char buffer[INET_ADDRSTRLEN];
                ::inet_ntop(AF_INET, addr, buffer, sizeof(buffer));
                address = buffer;
                interface = ifa->ifa_name[0];
                break;
            }
        }
    }

    ::freeifaddrs(ifaddr);

    return address;
}

//-------------------------------------------------------------------------

std::string
vcGenCmd(const std::string& command)
{
     raspifb16::FileDescriptor fd{::open("/dev/vcio", 0)};

    if (fd.fd() == -1)
    {
        return {};
    }

    static constexpr auto RESPONSE_LENGTH{1024};
    static constexpr auto PROPERTY_SIZE{(RESPONSE_LENGTH / 4) + 7};

    uint32_t property[PROPERTY_SIZE] =
    {
        0x00000000,
        0x00000000,
        0x00030080,
        RESPONSE_LENGTH,
        0x00000000,
        0x00000000,
    };

    memcpy(property + 6, command.data(), command.length() + 1);

    property[0] = PROPERTY_SIZE * sizeof(property[0]);

    if (::ioctl(fd.fd(), _IOWR(100, 0, char *), property) == -1)
    {
        return {};
    }

    return reinterpret_cast<char *>(property + 6);
}

//-------------------------------------------------------------------------

std::string
getMemorySplit()
{
    std::string result{};

    int arm_mem{};
    int gpu_mem{};

    const auto& arm{vcGenCmd("get_mem arm")};

    if (not arm.empty())
    {
        try
        {
            std::regex pattern{R"(arm=(\d+)M)"};
            std::smatch match;
            std::string vcGenResult(arm);

            if (std::regex_search(vcGenResult, match, pattern) and
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

    const auto& gpu{vcGenCmd("get_mem gpu")};

    if (not gpu.empty())
    {
        try
        {
            std::regex pattern{R"(gpu=(\d+)M)"};
            std::smatch match;
            std::string vcGenResult(gpu);

            if (std::regex_search(vcGenResult, match, pattern) and
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

    if (arm_mem and gpu_mem)
    {
        result = std::to_string(gpu_mem) + "/" + std::to_string(arm_mem);
    }

    return result;
}

//-------------------------------------------------------------------------

std::string
getTemperature()
{
    return std::to_string(raspinfo::getTemperature());
}

//-------------------------------------------------------------------------

std::string
getTime(
    time_t now)
{
    char buffer[128];

    tm result;
    tm *lt = ::localtime_r(&now, &result);
    std::strftime(buffer, sizeof(buffer), "%T", lt);

    return buffer;
}

//-------------------------------------------------------------------------

} // namespace

//=========================================================================

DynamicInfo::DynamicInfo(
    int width,
    int fontHeight,
    int yPosition)
:
    Panel{width, 2 * fontHeight + 4, yPosition},
    m_heading(255, 255, 0),
    m_foreground(255, 255, 255),
    m_background(0, 0, 0),
    m_memorySplit(getMemorySplit())
{
}

//-------------------------------------------------------------------------

void
DynamicInfo::init(
    raspifb16::Interface565Font& font)
{
}

//-------------------------------------------------------------------------

void
DynamicInfo::update(
    time_t now,
    raspifb16::Interface565Font& font)
{
    getImage().clear(m_background);

    //---------------------------------------------------------------------

    raspifb16::Interface565Point position = { 0, 0 };

    position = font.drawString(position,
                          "ip(",
                          m_heading,
                          getImage());

    char interface = ' ';
    std::string ipaddress = getIpAddress(interface);

    position = font.drawChar(position,
                             interface,
                             m_foreground,
                             getImage());

    position = font.drawString(position,
                               ") ",
                               m_heading,
                               getImage());

    position = font.drawString(position,
                               ipaddress,
                               m_foreground,
                               getImage());

    if (not m_memorySplit.empty())
    {
        position = font.drawString(position,
                                " memory ",
                                m_heading,
                                getImage());

        position = font.drawString(position,
                                m_memorySplit,
                                m_foreground,
                                getImage());

        position = font.drawString(position,
                                " MB",
                                m_foreground,
                                getImage());
    }

    //---------------------------------------------------------------------

    position.set(0, position.y() + font.getPixelHeight() + 4);

    position = font.drawString(position,
                               "time ",
                               m_heading,
                               getImage());

    std::string timeString = getTime(now);

    position = font.drawString(position,
                               timeString,
                               m_foreground,
                               getImage());

    position = font.drawString(position,
                               " temperature ",
                               m_heading,
                               getImage());

    std::string temperatureString = getTemperature();

    position = font.drawString(position,
                               temperatureString,
                               m_foreground,
                               getImage());

    uint8_t degreeSymbol = 0xF8;

    position = font.drawChar(position,
                             degreeSymbol,
                             m_foreground,
                             getImage());


    position = font.drawString(position,
                               "C",
                               m_foreground,
                               getImage());
}

