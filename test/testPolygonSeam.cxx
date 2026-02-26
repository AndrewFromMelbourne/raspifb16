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

        //-----------------------------------------------------------------

        const auto fwidth = fb->getWidth();
        const auto fheight = fb->getHeight();
        constexpr auto offset{10};
        const auto width = std::min(fwidth, fheight) - (2 * offset);
        const auto xOffset = (fwidth - width) / 2;
        const auto yOffset = (fheight - width) / 2;

        constexpr RGB565 white{255, 255, 255};
        constexpr RGB565 black{0, 0, 0};

        //-----------------------------------------------------------------

        fb->clearBuffers(black);

        polygonFilled(
            *fb,
            {{
                Point565{xOffset, yOffset},
                Point565{width + xOffset, yOffset},
                Point565{width + xOffset, width + yOffset}
            }},
            white);

        polygonFilled(
            *fb,
            {{
                Point565{xOffset, yOffset},
                Point565{xOffset, width + yOffset},
                Point565{width + xOffset, width + yOffset}
            }},
            white);

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
