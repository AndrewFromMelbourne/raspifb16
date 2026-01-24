//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2026 Andrew Duncan
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

#include <getopt.h>
#include <libgen.h>
#include <unistd.h>

#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <numbers>
#include <print>
#include <system_error>
#include <thread>

#include "image565.h"
#include "image565Graphics.h"
#include "interface565Factory.h"
#include "point.h"

//-------------------------------------------------------------------------

using namespace raspifb16;
using namespace std::chrono_literals;

//-------------------------------------------------------------------------

void
printUsage(
    std::ostream& stream,
    const std::string& name)
{
    std::println(stream, "");
    std::println(stream, "Usage: {}", name);
    std::println(stream, "");
    std::println(stream, "    --device,-d - device to use");
    std::println(stream, "    --help,-h - print usage and exit");
    std::println(stream, "    --kmsdrm,-k - use KMS/DRM dumb buffer");
    std::println(stream, "");
}

//-------------------------------------------------------------------------

int
main(
    int argc,
    char *argv[])
{
    std::string device{};
    const std::string program{basename(argv[0])};
    auto interfaceType{raspifb16::InterfaceType565::FRAME_BUFFER_565};

    //---------------------------------------------------------------------

    static const char* sopts = "d:hk";
    static option lopts[] =
    {
        { "device", required_argument, nullptr, 'd' },
        { "help", no_argument, nullptr, 'h' },
        { "kmsdrm", no_argument, nullptr, 'k' },
        { nullptr, no_argument, nullptr, 0 }
    };

    int opt{};

    while ((opt = ::getopt_long(argc, argv, sopts, lopts, nullptr)) != -1)
    {
        switch (opt)
        {
        case 'd':

            device = optarg;

            break;

        case 'h':

            printUsage(std::cout, program);
            ::exit(EXIT_SUCCESS);

            break;

        case 'k':

            interfaceType = raspifb16::InterfaceType565::KMSDRM_DUMB_BUFFER_565;

            break;

        default:

            printUsage(std::cerr, program);
            ::exit(EXIT_FAILURE);

            break;
        }
    }

    //---------------------------------------------------------------------

    try
    {
        auto fb{raspifb16::createInterface565(interfaceType, device)};

        const int fheight = fb->getHeight();
        const int fwidth = fb->getWidth();

        constexpr RGB565 white{255, 255, 255};
        constexpr RGB565 grey(192, 192, 192);
        constexpr RGB565 black{0, 0, 0};

        fb->clearBuffers(grey);

        //-----------------------------------------------------------------

        const int diameter = std::min(fwidth, fheight) / 4;
        const int radius = (diameter / 2) - 5;

        //-----------------------------------------------------------------

        auto starVertex = [](int i, int r, int x, int y) -> Interface565Point
        {
            constexpr auto pi = std::numbers::pi;
            constexpr auto phi = std::numbers::phi;
            const auto sinValue = std::sin((i * pi) / 5.0);
            const auto cosValue = std::cos((i * pi) / 5.0);
            const auto radius = (i % 2 == 0) ? r : (r * (2 - phi));
            return Interface565Point{
                x + static_cast<int>(std::round(radius * sinValue)),
                y - static_cast<int>(std::round(radius * cosValue))
            };
        };

        //-----------------------------------------------------------------

        const int jIncrement = fheight / 4;
        const int iIncrement = fwidth / 4;

        for (int j = 0; j <= fheight; j += jIncrement)
        {
            const int index = j / jIncrement;
            const int startI = (index % 2 == 0) ? 0 : (iIncrement / 2);
            for (int i = startI; i <= fwidth; i += iIncrement)
            {
                const std::array<Interface565Point, 10> starVertices{
                    starVertex(0, radius, i, j),
                    starVertex(1, radius, i, j),
                    starVertex(2, radius, i, j),
                    starVertex(3, radius, i, j),
                    starVertex(4, radius, i, j),
                    starVertex(5, radius, i, j),
                    starVertex(6, radius, i, j),
                    starVertex(7, radius, i, j),
                    starVertex(8, radius, i, j),
                    starVertex(9, radius, i, j)
                };

                polygonFilled(*fb, starVertices, white);
                polygon(*fb, starVertices, black);
            }
        }

        //-----------------------------------------------------------------

        fb->update();

        //-----------------------------------------------------------------

        std::this_thread::sleep_for(10s);
    }
    catch (std::exception& error)
    {
        std::println(std::cerr, "Error: {}", error.what());
        exit(EXIT_FAILURE);
    }

    return 0;
}
