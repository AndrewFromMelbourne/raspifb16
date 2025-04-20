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

#include <fmt/format.h>

#include <chrono>
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
    FILE* file,
    const std::string& name)
{
    fmt::print(file, "\n");
    fmt::print(file, "Usage: {}\n", name);
    fmt::print(file, "\n");
    fmt::print(file, "    --device,-d - device to use\n");
    fmt::print(file, "    --help,-h - print usage and exit\n");
    fmt::print(file, "    --kmsdrm,-k - use KMS/DRM dumb buffer\n");
    fmt::print(file, "\n");
}

//-------------------------------------------------------------------------

int
main(
    int argc,
    char *argv[])
{
    std::string device{};
    std::string program{basename(argv[0])};
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

            printUsage(stdout, program);
            ::exit(EXIT_SUCCESS);

            break;

        case 'k':

            interfaceType = raspifb16::InterfaceType565::KMSDRM_DUMB_BUFFER_565;

            break;

        default:

            printUsage(stderr, program);
            ::exit(EXIT_FAILURE);

            break;
        }
    }

    //---------------------------------------------------------------------

    try
    {
        auto fb{raspifb16::createInterface565(interfaceType, device)};
        fb->clearBuffers();

        //-----------------------------------------------------------------

        const RGB565 white{255, 255, 255};

        //-----------------------------------------------------------------

        circle(*fb, Interface565Point{60, 60}, 50, white);
        circleFilled(*fb, Interface565Point{180, 60}, 50, white);

        //-----------------------------------------------------------------

        fb->update();

        //-----------------------------------------------------------------

        std::this_thread::sleep_for(10s);

        fb->clearBuffers();
    }
    catch (std::exception& error)
    {
        fmt::print(stderr, "Error: {}\n", error.what());
        exit(EXIT_FAILURE);
    }

    return 0;
}
