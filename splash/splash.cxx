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
#include <unistd.h>

#include <iostream>
#include <print>

#include "image565.h"
#include "image565Qoi.h"
#include "interface565Factory.h"
#include "joystick.h"

//-------------------------------------------------------------------------

using namespace raspifb16;

//-------------------------------------------------------------------------

namespace
{
const std::string defaultDevice{"/dev/fb1"};
}

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
    std::println(stream, "    --qoi,-q - qoi file to display");
    std::println(stream, "");
}

//-------------------------------------------------------------------------

int
main(
    int argc,
    char *argv[])
{
    std::string device{};
    std::string program{basename(argv[0])};
    std::string qoi{};
    auto interfaceType{raspifb16::InterfaceType565::FRAME_BUFFER_565};

    //---------------------------------------------------------------------

    static const char* sopts = "d:hkq:";
    static option lopts[] =
    {
        { "device", required_argument, nullptr, 'd' },
        { "help", no_argument, nullptr, 'h' },
        { "kmsdrm", no_argument, nullptr, 'k' },
        { "qoi", required_argument, nullptr, 'q' },
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

        case 'q':

            qoi = optarg;

            break;

        default:

            printUsage(std::cerr, program);
            ::exit(EXIT_FAILURE);

            break;
        }
    }

    if (qoi.empty())
    {
        printUsage(std::cerr, program);
        ::exit(EXIT_FAILURE);
    }

    try
    {
        constexpr bool block{true};
        Joystick js{block};
        auto fb{raspifb16::createInterface565(interfaceType, device)};

        fb->clearBuffers();

        const auto image = readQoi(qoi);
        fb->putImage(center(*fb, image), image);
        fb->update();

        do {
            js.read();
        }
        while (not js.buttonPressed(Joystick::BUTTON_START));

        fb->clearBuffers();
    }
    catch (std::exception& error)
    {
        std::println(std::cerr, "Error: {}", error.what());
        exit(EXIT_FAILURE);
    }

    return 0;
}

