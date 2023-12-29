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

#include <iostream>
#include <system_error>

#include <unistd.h>

#include "framebuffer565.h"
#include "image565.h"
#include "image565Graphics.h"
#include "point.h"

//-------------------------------------------------------------------------

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
        image.setPixelRGB(Image565Point(x, y + i),
                          RGB565::blend(alpha32(i), rgb, black));
    }
}

//-------------------------------------------------------------------------

void fadeToWhite(Image565& image, int x, int y, const RGB565& rgb)
{
    const RGB565 white{255, 255, 255};

    for (int i = 0 ; i < 32 ; ++i)
    {
        image.setPixelRGB(Image565Point(x, y + i),
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

int
main()
{
    try
    {
        FrameBuffer565 fb{"/dev/fb1"};
        fb.clear();

        //-----------------------------------------------------------------

        Image565 image{192, 96};

        hues(image, 0, fadeFromBlack);
        hues(image, 32, colourLine);
        hues(image, 64, fadeToWhite);

        //-----------------------------------------------------------------

        Interface565Point imageLocation
        {
            (fb.getWidth() - image.getWidth()) / 2,
            (fb.getHeight() - image.getHeight()) / 2
        };

        fb.putImage(imageLocation, image);

        //-----------------------------------------------------------------

        sleep(10);

        fb.clear();
    }
    catch (std::exception& error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        exit(EXIT_FAILURE);
    }

    return 0;
}
