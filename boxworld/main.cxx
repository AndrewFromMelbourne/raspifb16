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

#include <csignal>
#include <iostream>
#include <thread>

#include "image565Font8x8.h"
#include "interface565Factory.h"
#include "joystick.h"

#include "boxworld.h"

//-------------------------------------------------------------------------

using namespace std::chrono_literals;
using namespace raspifb16;

//-------------------------------------------------------------------------

namespace
{
volatile static std::sig_atomic_t run{1};
const std::string defaultJoystick{"/dev/input/js0"};
}

//-------------------------------------------------------------------------

void
printUsage(
    std::ostream& os,
    const std::string& name)
{
    os << "\n";
    os << "Usage: " << name << " <options>\n";
    os << "\n";
    os << "    --device,-d - device to use\n";
    os << "    --help,-h - print usage and exit\n";
    os << "    --joystick,-j - joystick device to use";
    os << " (default is " << defaultJoystick << ")\n";
    os << "    --kmsdrm,-k - use KMS/DRM dumb buffer\n";
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
    std::string joystick{defaultJoystick};
    auto interfaceType{raspifb16::InterfaceType565::FRAME_BUFFER_565};

    //---------------------------------------------------------------------

    static const char* sopts = "d:hj:k";
    static option lopts[] =
    {
        { "device", required_argument, nullptr, 'd' },
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

        case 'h':

            printUsage(std::cout, program);
            ::exit(EXIT_SUCCESS);

            break;

        case 'j':

            joystick = optarg;

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
        Image565Font8x8 font;
        Joystick js{joystick};
        auto fb{raspifb16::createInterface565(interfaceType, device)};
        fb->clear(RGB565{0, 0, 0});

        Boxworld boxworld;
        boxworld.init();
        boxworld.draw(*fb, font);

        //-----------------------------------------------------------------

        while (run)
        {
            js.read();

            if (js.buttonPressed(Joystick::BUTTON_START))
            {
                run = 0;
            }
            else
            {
                boxworld.update(js);
                boxworld.draw(*fb, font);
            }

            std::this_thread::sleep_for(250ms);
        }

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

