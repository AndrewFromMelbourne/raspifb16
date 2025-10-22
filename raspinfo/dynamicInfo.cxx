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

#include <fcntl.h>
#include <ifaddrs.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

#include "dynamicInfo.h"
#include "fileDescriptor.h"
#include "image565Font8x16.h"
#include "system.h"

//=========================================================================

namespace
{

//-------------------------------------------------------------------------

struct IpAddress
{
    std::string address;
    char interface;
};

//-------------------------------------------------------------------------

IpAddress
getIpAddress()
{
    IpAddress result{ .address = "   .   .   .   ", .interface = 'X'};
    ifaddrs *ifaddr{};

    ::getifaddrs(&ifaddr);

    if (not ifaddr)
    {
        return result;
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
                result.address = buffer;
                result.interface = ifa->ifa_name[0];
                break;
            }
        }
    }

    ::freeifaddrs(ifaddr);

    return result;
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

    static constexpr auto c_responseLength{1024};
    static constexpr auto c_propertySize{(c_responseLength / 4) + 7};

    uint32_t property[c_propertySize] =
    {
        0x00000000,
        0x00000000,
        0x00030080,
        c_responseLength,
        0x00000000,
        0x00000000,
    };

    memcpy(property + 6, command.data(), command.length() + 1);

    property[0] = c_propertySize * sizeof(property[0]);

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
    auto getValue = [](const std::string& token) -> int
    {
        const auto& vcGenResult{vcGenCmd("get_mem " + token)};
        int value{};

        if (not vcGenResult.empty())
        {
            try
            {
                std::regex pattern{token + "=(\\d+)M"};
                std::smatch match;

                if (std::regex_match(vcGenResult, match, pattern) and
                    (match.size() == 2))
                {
                    value = std::stoi(match[1].str());
                }
            }
            catch (std::exception&)
            {
                // ignore
            }
        }

        return value;
    };

    const int arm_mem{getValue("arm")};
    const int gpu_mem{getValue("gpu")};

    if (not arm_mem or not gpu_mem)
    {
        return "";
    }

    return std::to_string(gpu_mem) + "/" + std::to_string(arm_mem);
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
    const tm *lt = ::localtime_r(&now, &result);
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
    raspifb16::Interface565Font&)
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

    const auto ipaddress = getIpAddress();

    position = font.drawChar(position,
                             ipaddress.interface,
                             m_foreground,
                             getImage());

    position = font.drawString(position,
                               ") ",
                               m_heading,
                               getImage());

    position = font.drawString(position,
                               ipaddress.address,
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

    const std::string temperatureString = getTemperature();

    position = font.drawString(position,
                               temperatureString,
                               m_foreground,
                               getImage());

    const auto degreeCode{raspifb16::Interface565Font::CharacterCode::DEGREE_SYMBOL};
    const auto degreeSymbol = font.getCharacterCode(degreeCode);

    position = font.drawChar(position,
                             *degreeSymbol,
                             m_foreground,
                             getImage());


    position = font.drawString(position,
                               "C",
                               m_foreground,
                               getImage());
}

