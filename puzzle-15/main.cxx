//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2022 Andrew Duncan
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

#include <atomic>
#include <csignal>
#include <cstring>
#include <iostream>
#include <print>

#include "interface565Factory.h"
#include "joystick.h"
#include "puzzle.h"

//-------------------------------------------------------------------------

using namespace fb16;

//-------------------------------------------------------------------------

namespace
{
std::atomic<bool> run{true};
const std::string defaultJoystick{"/dev/input/js0"};
}

//-------------------------------------------------------------------------

static void
signalHandler(
    int signalNumber)
{
    switch (signalNumber)
    {
    case SIGINT:
    case SIGTERM:

        run = false;
        break;
    };
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
    std::println(stream, "    --fitToScreen,-f - fit puzzle to screen");
    std::println(stream, "    --help,-h - print usage and exit");
    std::println(stream, "    --joystick,-j - joystick device to use, default {}", defaultJoystick);
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
    bool fitToScreen{false};
    const std::string program{::basename(argv[0])};
    std::string joystick{defaultJoystick};
    auto interfaceType{fb16::InterfaceType565::FRAME_BUFFER_565};

    //---------------------------------------------------------------------

    static const char* sopts = "d:fhj:k";
    static option lopts[] =
    {
        { "device", required_argument, nullptr, 'd' },
        { "fitToScreen", no_argument, nullptr, 'f' },
        { "help", no_argument, nullptr, 'h' },
        { "joystick", required_argument, nullptr, 'j' },
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

            fitToScreen = true;
            break;

        case 'h':

            printUsage(std::cout, program);
            ::exit(EXIT_SUCCESS);
            break;

        case 'j':

            joystick = optarg;
            break;

        case 'k':

            interfaceType = fb16::InterfaceType565::KMSDRM_DUMB_BUFFER_565;
            break;

        default:

            printUsage(std::cerr, program);
            ::exit(EXIT_FAILURE);
            break;
        }
    }

    //---------------------------------------------------------------------

    for (auto signal : { SIGINT, SIGTERM })
    {
        struct sigaction sa{};

        sa.sa_handler = signalHandler;
        sa.sa_flags = 0;

        if (sigaction(signal, &sa, nullptr) == -1)
        {
            std::println(
                std::cerr,
                "Error: installing {} signal handler : {}",
                strsignal(signal),
                strerror(errno));

            ::exit(EXIT_FAILURE);
        }
    }

    //---------------------------------------------------------------------

    try
    {
        constexpr bool block{true};
        Joystick js(joystick, block);
        auto fb{fb16::createInterface565(interfaceType, device)};

        Puzzle puzzle{fitToScreen};
        puzzle.init();
        puzzle.draw(*fb);
        fb->update();

        //-----------------------------------------------------------------

        while (run)
        {
            js.read();

            if (js.buttonPressed(Joystick::BUTTON_START))
            {
                run = 0;
            }
            else if (puzzle.update(js))
            {
                puzzle.draw(*fb);
                fb->update();
            }
        }
    }
    catch (std::exception& error)
    {
        std::println(std::cerr, "Error: {}", error.what());
        exit(EXIT_FAILURE);
    }
}

