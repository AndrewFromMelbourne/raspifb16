//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2023 Andrew Duncan
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

#include <chrono>
#include <deque>
#include <iostream>
#include <print>
#include <system_error>
#include <thread>

#include "image565FreeType.h"
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
    std::println(stream, "    --font,-f - font file to use");
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
    std::string font{};
    auto interfaceType{raspifb16::InterfaceType565::FRAME_BUFFER_565};

    //---------------------------------------------------------------------

    static const char* sopts = "d:f:hk";
    static option lopts[] =
    {
        { "device", required_argument, nullptr, 'd' },
        { "font", required_argument, nullptr, 'f' },
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

        case 'f':

            font = optarg;

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
        const RGB565 black{0, 0, 0};
        const RGB565 white{255, 255, 255};
        auto fb{raspifb16::createInterface565(interfaceType, device)};

        Image565 image(fb->getWidth(), fb->getHeight());
        image.clear(black);

        //-----------------------------------------------------------------

        Image565FreeType ft{font, 32};
        Interface565Point p{0, 0};

        p = ft.drawString(p, "abcdefghijklmnopqrstuvwxyz ", white, image);
        p = ft.drawString(p, "0123456789", white, image);

        p.set(0, p.y() + ft.getPixelHeight());

        p = ft.drawString(p, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", white, image);

        p.set(0, p.y() + ft.getPixelHeight());

        p = ft.drawChar(p, '@', white, image);

        p.set(0, p.y() + ft.getPixelHeight());

        for (int j = 0 ; j < 16 ; ++j)
        {
            for (auto i = 0 ; i < 16 ; ++i)
            {
                auto c = static_cast<uint8_t>(i + (j * 16));
                p.setX(i * ft.getPixelWidth());
                ft.drawChar(p, c, white, image);
            }

            p.setY(p.y() + ft.getPixelHeight());
        }

        //-----------------------------------------------------------------

        fb->putImage(Interface565Point{0, 0}, image);
        fb->update();
        std::this_thread::sleep_for(10s);
    }
    catch (std::exception& error)
    {
        std::println(std::cerr, "Error: {}", error.what());
        exit(EXIT_FAILURE);
    }

    //---------------------------------------------------------------------

    return 0 ;
}

