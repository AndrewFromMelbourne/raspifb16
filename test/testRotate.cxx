//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2024 Andrew Duncan
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

#include <fmt/format.h>

#include <chrono>
#include <iostream>
#include <print>
#include <system_error>
#include <thread>

#include "image565.h"
#include "image565Font8x16.h"
#include "image565Graphics.h"
#include "image565Process.h"
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
    std::println(stream, "Usage: {} <options>", name);
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

        constexpr RGB565 darkBlue{0, 0, 63};
        constexpr RGB565 white{255, 255, 255};
        constexpr RGB565 darkGrey{63, 63, 63};

        fb->clearBuffers(darkGrey);

        constexpr int width{72};
        constexpr int height{16};

        Image565 image(width, height);
        image.clear(darkBlue);

        //-----------------------------------------------------------------

        Image565Font8x16 font;

        font.drawString(
            Interface565Point{4, 0},
            "rotating",
            white,
            image);

        //-----------------------------------------------------------------

        image = scaleUp(image, 3);

        //-----------------------------------------------------------------

        auto degreeChar = font.getCharacterCode(Interface565Font::CharacterCode::DEGREE_SYMBOL).value_or(' ');

        for (int angle = 0; angle < 3600; ++angle)
        {
            fb->clear(darkGrey);

            font.drawString(
                Interface565Point{4, 0},
                std::format("Angle: {:3d}{}{:02d}'", angle / 10, degreeChar, (angle % 10) * 6),
                white,
                *fb);

            const auto rotated = rotate(image, darkGrey, angle / 10.0);
            const Interface565Point p
            {
                (fb->getWidth() - rotated.getWidth()) / 2,
                (fb->getHeight() - rotated.getHeight()) / 2
            };

            const auto start = std::chrono::steady_clock::now();
            fb->putImage(p, rotated);
            if (not fb->update())
            {
                // No Vsync, so add a fake vsync time

                constexpr auto frametime = 10ms;
                const auto end = std::chrono::steady_clock::now();
                const auto duration = end - start;

                if (duration < frametime)
                {
                    std::this_thread::sleep_for(frametime - duration );
                }
            }
        }
    }
    catch (std::exception& error)
    {
        std::println(std::cerr, "Error: {}", error.what());
        exit(EXIT_FAILURE);
    }

    return 0;
}

