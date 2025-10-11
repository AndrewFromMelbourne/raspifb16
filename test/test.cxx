//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2015 Andrew Duncan
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
#include <iostream>
#include <print>
#include <system_error>
#include <thread>

#include "image565.h"
#include "image565Font8x16.h"
#include "image565Graphics.h"
#include "interface565Factory.h"
#include "point.h"

//-------------------------------------------------------------------------

using namespace raspifb16;
using namespace std::chrono_literals;

//-------------------------------------------------------------------------

void test(bool expression, std::string_view message)
{
    if (!expression)
    {
        std::println(
            std::cerr,
            "{}({}) : {} : test failed",
            __FILE__,
            __LINE__,
            message);
        exit(EXIT_FAILURE);
    }
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
        Image565Font8x16 font;
        auto fb{raspifb16::createInterface565(interfaceType, device)};
        fb->clearBuffers();

        //-----------------------------------------------------------------

        const RGB565 red{255, 0, 0};
        const RGB565 green{0, 255, 0};

        //-----------------------------------------------------------------

        Image565 image{48, 48};
        image.clear(red);

        auto rgb = image.getPixelRGB(Interface565Point(0,0));

        test(rgb.has_value(), "Image565::getPixelRGB()");
        test((*rgb == red), "Image565::getPixelRGB()");

        line(image,
             Interface565Point(0,0),
             Interface565Point(47,47),
             green);

        auto imageLocation = center(*fb, image);

        fb->putImage(imageLocation, image);

        rgb = fb->getPixelRGB(imageLocation);

        test(rgb.has_value(), "FrameBuffer565::getPixelRGB()");
        test((*rgb == green), "FrameBuffer565::getPixelRGB()");

        //-----------------------------------------------------------------

        const RGB565 darkBlue{0, 0, 63};
        const RGB565 white{255, 255, 255};

        Image565 textImage(168, 16);
        textImage.clear(darkBlue);

        font.drawString(
            Interface565Point{0, 0},
            "This is a test string",
            white,
            textImage);

        const Interface565Point textLocation
        {
            (fb->getWidth() - textImage.getWidth()) / 2,
            (fb->getHeight() - textImage.getHeight()) / 3
        };

        fb->putImage(textLocation, textImage);
        fb->update();

        //-----------------------------------------------------------------

        std::this_thread::sleep_for(10s);

        fb->clearBuffers();
    }
    catch (std::exception& error)
    {
        std::println(std::cerr, "Error: {}", error.what());
        exit(EXIT_FAILURE);
    }

    return 0;
}

