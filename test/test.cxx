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

#include <iostream>
#include <system_error>

#include <unistd.h>

#include "framebuffer565.h"
#include "image565.h"
#include "image565Font.h"
#include "image565Graphics.h"
#include "point.h"

//-------------------------------------------------------------------------

using namespace raspifb16;

//-------------------------------------------------------------------------

int
main(void)
{
    try
    {
        FrameBuffer565 fb{"/dev/fb1"};
        fb.clear();

        //-----------------------------------------------------------------

        Image565 image{48, 48};
        image.clear(RGB565{255, 0, 0});
        line(image,
             Image565Point(0,0),
             Image565Point(47,47),
             RGB565{0, 255, 0});

        FB565Point imageLocation
        {
            (fb.getWidth() - image.getWidth()) / 2, 
            (fb.getHeight() - image.getHeight()) / 2
        };

        fb.putImage(imageLocation, image);

        //-----------------------------------------------------------------

        Image565 textImage(168, 16);
        textImage.clear(RGB565(0, 0, 63));

        FB565Point textLocation
        {
            (fb.getWidth() - textImage.getWidth()) / 2, 
            (fb.getHeight() - textImage.getHeight()) / 3
        };

        drawString(
            FontPoint{0, 0},
            "This is a test string",
            RGB565{255, 255, 255},
            textImage);

        fb.putImage(textLocation, textImage);

        //-----------------------------------------------------------------

        sleep(10);

        fb.clear();
    }
    catch (std::exception& error)
    {
        std::cerr << "Error: " << error.what() << "\n";
        exit(EXIT_FAILURE);
    }

    return 0;
}
