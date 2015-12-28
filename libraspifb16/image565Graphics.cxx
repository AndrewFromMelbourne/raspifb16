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

#include <cmath>
#include <cstdint>

#include "image565.h"
#include "image565Graphics.h"
#include "point.h"

//-------------------------------------------------------------------------

void
raspifb16::
box(
    CImage565& image,
    const CImage565Point& p1,
    const CImage565Point& p2,
    uint16_t rgb)
{
    verticalLine(image, p1.x(), p1.y(), p2.y(), rgb);
    horizontalLine(image, p1.x(), p2.x(), p1.y(), rgb);
    verticalLine(image, p2.x(), p1.y(), p2.y(), rgb);
    horizontalLine(image, p1.x(), p2.x(), p2.y(), rgb);
}

//-------------------------------------------------------------------------

void
raspifb16::
boxFilled(
    CImage565& image,
    const CImage565Point& p1,
    const CImage565Point& p2,
    uint16_t rgb)
{
    int16_t sign_y = (p1.y() <= p2.y()) ? 1 : -1;
    int16_t y = p1.y();

    horizontalLine(image, p1.x(), p2.x(), y, rgb);

    while (y != p2.y())
    {
        y += sign_y;
        horizontalLine(image, p1.x(), p2.x(), y, rgb);
    }
}

//-------------------------------------------------------------------------

void
raspifb16::
line(
    CImage565& image,
    const CImage565Point& p1,
    const CImage565Point& p2,
    uint16_t rgb)
{
    if (p1.y() == p2.y())
    {
        horizontalLine(image, p1.x(), p2.x(), p1.y(), rgb);
    }
    else if (p1.x() == p2.x())
    {
        verticalLine(image, p1.x(), p1.y(), p2.y(), rgb);
    }
    else
    {
        int16_t dx = std::abs(p2.x() - p1.x());
        int16_t dy = std::abs(p2.y() - p1.y());

        int16_t sign_x = (p1.x() <= p2.x()) ? 1 : -1;
        int16_t sign_y = (p1.y() <= p2.y()) ? 1 : -1;

        int16_t x = p1.x();
        int16_t y = p1.y();

        image.setPixel(p1, rgb);

        if (dx > dy)
        {
            int16_t d = 2 * dy - dx;
            int16_t incrE = 2 * dy;
            int16_t incrNE = 2 * (dy - dx);

            while (x != p2.x())
            {
                x += sign_x;

                if (d <= 0)
                {
                    d += incrE;
                }
                else
                {
                    d += incrNE;
                    y += sign_y;
                }

                image.setPixel(CImage565Point(x, y), rgb);
            }
        }
        else
        {
            int16_t d = 2 * dx - dy;
            int16_t incrN = 2 * dx;
            int16_t incrNE = 2 * (dx - dy);

            while (y != p2.y())
            {
                y += sign_y;

                if (d <= 0)
                {
                    d += incrN;
                }
                else
                {
                    d += incrNE;
                    x += sign_x;
                }

                image.setPixel(CImage565Point(x, y), rgb);
            }
        }
    }
}

//-------------------------------------------------------------------------

void
raspifb16::
horizontalLine(
    CImage565& image,
    int16_t x1,
    int16_t x2,
    int16_t y,
    uint16_t rgb)
{
    int16_t sign_x = (x1 <= x2) ? 1 : -1;
    int16_t x = x1;

    image.setPixel(CImage565Point(x, y), rgb);

    while (x != x2)
    {
        x += sign_x;
        image.setPixel(CImage565Point(x, y), rgb);
    }
}

//-------------------------------------------------------------------------

void
raspifb16::
verticalLine(
    CImage565& image,
    int16_t x,
    int16_t y1,
    int16_t y2,
    uint16_t rgb)
{
    int16_t sign_y = (y1 <= y2) ? 1 : -1;
    int16_t y = y1;

    image.setPixel(CImage565Point(x, y), rgb);

    while (y != y2)
    {
        y += sign_y;
        image.setPixel(CImage565Point(x, y), rgb);
    }
}

