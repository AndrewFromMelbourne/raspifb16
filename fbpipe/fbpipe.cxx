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

#include <deque>
#include <iostream>
#include <system_error>

#include "framebuffer565.h"
#include "image565.h"
#include "image565Font8x16.h"
#include "point.h"

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
    std::ostream& os,
    const std::string& name)
{
    os << "\n";
    os << "Usage: " << name << " <options>\n";
    os << "\n";
    os << "    --device,-d - dri device to use";
    os << " (default is " << defaultDevice << ")\n";
    os << "    --help,-h - print usage and exit\n";
    os << "\n";
}

//-------------------------------------------------------------------------

int
main(
    int argc,
    char *argv[])
{
    std::string device = defaultDevice;
    std::string program = basename(argv[0]);

    //---------------------------------------------------------------------

    static const char* sopts = "d:h";
    static option lopts[] =
    {
        { "device", required_argument, nullptr, 'd' },
        { "help", no_argument, nullptr, 'h' },
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
        const RGB565 black{0, 0, 0};
        const RGB565 white{255, 255, 255};
        FrameBuffer565 fb{device};

        fb.clear(black);

        const int columns = fb.getWidth() / font.getPixelWidth();
        const int rows = fb.getHeight() / font.getPixelHeight();

        Image565 image{fb.getWidth(), fb.getHeight()};
        image.clear(black);

        //-----------------------------------------------------------------

        std::deque<std::string> lines;
        std::string line;

        while (std::getline(std::cin, line))
        {
            lines.push_back(line.substr(0, columns));

            while (lines.size() > static_cast<unsigned int>(rows))
            {
                lines.pop_front();
            }

            int y = 0;

            for (const auto& l : lines)
            {
                font.drawString(
                    Interface565Point{0, y},
                    l,
                    white,
                    image);

                y += font.getPixelHeight();
            }

            fb.putImage(Interface565Point{0, 0}, image);
        }
    }
    catch (std::exception& error)
    {
        std::cerr << "Error: " << error.what() << "\n";
        exit(EXIT_FAILURE);
    }

    //---------------------------------------------------------------------

    return 0 ;
}

