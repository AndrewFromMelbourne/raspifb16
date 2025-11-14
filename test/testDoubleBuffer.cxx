//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2025 Andrew Duncan
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

#include <chrono>
#include <iostream>
#include <print>
#include <system_error>
#include <thread>

#include "dumbbuffer565.h"

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
    std::println(stream, "    --connector,-c - dri connector to use");
    std::println(stream, "    --device,-d - dri device to use");
    std::println(stream, "    --help,-h - print usage and exit");
    std::println(stream, "");
}

//-------------------------------------------------------------------------

int
main(
    int argc,
    char *argv[])
{
    uint32_t connector{0};
    std::string device{};
    const std::string program = basename(argv[0]);

    //---------------------------------------------------------------------

    static const char* sopts = "c:d:h";
    static option lopts[] =
    {
        { "connector", required_argument, nullptr, 'c' },
        { "device", required_argument, nullptr, 'd' },
        { "help", no_argument, nullptr, 'h' },
        { nullptr, no_argument, nullptr, 0 }
    };

    int opt{};

    while ((opt = ::getopt_long(argc, argv, sopts, lopts, nullptr)) != -1)
    {
        switch (opt)
        {
        case 'c':

            connector = std::stol(optarg);

            break;

        case 'd':

            device = optarg;

            break;

        case 'h':

            printUsage(std::cout, program);
            ::exit(EXIT_SUCCESS);

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
        DumbBuffer565 fb{device, connector};

        //-----------------------------------------------------------------

        std::println("Dimensions: {}x{}", fb.getWidth(), fb.getHeight());
        std::println("hasAtomic: {}", fb.hasAtomic());
        std::println("hasUniversalPlanes: {}", fb.hasUniversalPlanes());
        auto drmVersion = fb.getDrmVersion();
        std::println("drmVersion: {} {}.{}.{}",
                     drmVersion->name,
                     drmVersion->version_major,
                     drmVersion->version_minor,
                     drmVersion->version_patchlevel);

        //-----------------------------------------------------------------

        constexpr RGB565 red{255, 0, 0};
        constexpr RGB565 green{0, 255, 0};

        fb.clear(red);
        fb.update();
        fb.clear(green);

        //-----------------------------------------------------------------

        int count = 0;
        const auto start = std::chrono::steady_clock::now();

        while (std::chrono::steady_clock::now() - start < 10s)
        {
            ++count;
            fb.update();
        }

        const auto end = std::chrono::steady_clock::now();
        const auto duration = std::chrono::duration<double>(end - start);

        std::println("FPS: {:.1f}", count / duration.count());
    }
    catch (std::exception& error)
    {
        std::println(std::cerr, "Error: {}", error.what());
        exit(EXIT_FAILURE);
    }

    return 0;
}

