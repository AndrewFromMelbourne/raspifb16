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
#include <functional>
#include <iostream>
#include <system_error>
#include <thread>

#include "image565.h"
#include "image565Graphics.h"
#include "interface565Factory.h"
#include "point.h"

//-------------------------------------------------------------------------

using namespace std::chrono_literals;
using namespace raspifb16;

//-------------------------------------------------------------------------

RGB565 colour(int red, int green, int blue)
{
    return RGB565((255 * red) / 31, (255 * green) / 31, (255 * blue) / 31);
}

//-------------------------------------------------------------------------

uint8_t alpha32(int alpha)
{
    return static_cast<uint8_t>((255 * alpha) / 31);
}

//-------------------------------------------------------------------------

void fadeFromBlack(Image565& image, int x, int y, const RGB565& rgb)
{
    const RGB565 black{0, 0, 0};

    for (int i = 0 ; i < 32 ; ++i)
    {
        image.setPixelRGB(Interface565Point(x, y + i),
                          RGB565::blend(alpha32(i), rgb, black));
    }
}

//-------------------------------------------------------------------------

void fadeToWhite(Image565& image, int x, int y, const RGB565& rgb)
{
    const RGB565 white{255, 255, 255};

    for (int i = 0 ; i < 32 ; ++i)
    {
        image.setPixelRGB(Interface565Point(x, y + i),
                          RGB565::blend(alpha32(i), white, rgb));
    }
}

//-------------------------------------------------------------------------

void colourLine(Image565& image, int x, int y, const RGB565& rgb)
{
    verticalLine(image, x, y, y + 31, rgb);
}

//-------------------------------------------------------------------------

void hues(
    Image565& image,
    int y,
    std::function<void(Image565&, int, int, const RGB565&)> f)
{
    int red = 31;
    int green = 0;
    int blue = 0;
    int x = 0;

    for (green = 0 ; green <= 31 ; ++green)
    {
        f(image, x++, y, colour(red, green, blue));
    }

    green = 31;

    for (red = 31 ; red >= 0 ; --red)
    {
        f(image, x++, y, colour(red, green, blue));
    }

    red = 0;

    for (blue = 0 ; blue <= 31 ; ++blue)
    {
        f(image, x++, y, colour(red, green, blue));
    }

    blue = 31;

    for (green = 31 ; green >= 0 ; --green)
    {
        f(image, x++, y, colour(red, green, blue));
    }

    green = 0;

    for (red = 0 ; red <= 31 ; ++red)
    {
        f(image, x++, y, colour(red, green, blue));
    }

    red = 31;

    for (blue = 31 ; blue >= 0 ; --blue)
    {
        f(image, x++, y, colour(red, green, blue));
    }
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
    auto interfaceType{raspifb16::InterfaceType565::FRAME_BUFFER_565};

    //---------------------------------------------------------------------

    static const char* sopts = "d:hk";
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

        fb->clear();

        //-----------------------------------------------------------------

        Image565 image{192, 96};

        hues(image, 0, fadeFromBlack);
        hues(image, 32, colourLine);
        hues(image, 64, fadeToWhite);

        //-----------------------------------------------------------------

        Interface565Point imageLocation
        {
            (fb->getWidth() - image.getWidth()) / 2,
            (fb->getHeight() - image.getHeight()) / 2
        };

        fb->putImage(imageLocation, image);
        fb->update();

        //-----------------------------------------------------------------

        std::this_thread::sleep_for(10s);

        fb->clear();
    }
    catch (std::exception& error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        exit(EXIT_FAILURE);
    }

    return 0;
}
