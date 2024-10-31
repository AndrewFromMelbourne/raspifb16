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

#include <chrono>
#include <cstdlib>
#include <iostream>
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
    std::ostream& os,
    const std::string& name)
{
    os << "\n";
    os << "Usage: " << name << " <options>\n";
    os << "\n";
    os << "    --character,-c - character to print\n";
    os << "    --device,-d - device to use\n";
    os << "    --font,-f - font file to use\n";
    os << "    --help,-h - print usage and exit\n";
    os << "\n";
}

//-------------------------------------------------------------------------

int
main(
    int argc,
    char *argv[])
{
    std::string device{};
    std::string program{basename(argv[0])};
    std::string font{};
    uint32_t c{'A'};
    auto interfaceType{raspifb16::InterfaceType565::FRAME_BUFFER_565};

    //---------------------------------------------------------------------

    static const char* sopts = "c:d:f:hk";
    static option lopts[] =
    {
        { "character", required_argument, nullptr, 'c' },
        { "device", required_argument, nullptr, 'd' },
        { "font", required_argument, nullptr, 'f' },
        { "help", no_argument, nullptr, 'h' },
        { nullptr, no_argument, nullptr, 0 }
    };

    int opt{};

    while ((opt = ::getopt_long(argc, argv, sopts, lopts, nullptr)) != -1)
    {
        switch (opt)
        {
        case 'c':

            c = ::strtol(optarg, nullptr, 0);

            break;
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

        fb->clear(black);

        Image565 image(fb->getWidth(), fb->getHeight());
        image.clear(black);

        //-----------------------------------------------------------------

        Image565FreeType ft{font, 32};
        const Interface565Point p = ft.drawWideChar(p, c, white, image);

        //-----------------------------------------------------------------

        fb->putImage(Interface565Point{0, 0}, image);
        fb->update();
        std::this_thread::sleep_for(10s);
        fb->clear();

    }
    catch (std::exception& error)
    {
        std::cerr << "Error: " << error.what() << "\n";
        exit(EXIT_FAILURE);
    }

    //---------------------------------------------------------------------

    return 0 ;
}

