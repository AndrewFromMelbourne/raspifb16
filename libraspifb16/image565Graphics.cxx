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
    Interface565& image,
    const Interface565Point& p1,
    const Interface565Point& p2,
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
    Interface565& image,
    const Interface565Point& p1,
    const Interface565Point& p2,
    uint16_t rgb)
{
    const auto sign_y = (p1.y() <= p2.y()) ? 1 : -1;
    auto y = p1.y();

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
boxFilled(
    Interface565& image,
    const Interface565Point& p1,
    const Interface565Point& p2,
    const RGB565& rgb,
    uint8_t alpha)
{
    const auto sign_x = (p1.x() <= p2.x()) ? 1 : -1;
    const auto sign_y = (p1.y() <= p2.y()) ? 1 : -1;

    for (auto j = p1.y() ; j <= p2.y() ; j += sign_y)
    {
        for (auto i = p1.x() ; i <= p2.x() ; i += sign_x)
        {
            const Interface565Point p{i, j};
            auto background = image.getPixelRGB(p);

            if (background.has_value())
            {
                image.setPixelRGB(p, rgb.blend(alpha, *background));
            }
        }
    }
}

//-------------------------------------------------------------------------

void
raspifb16::
line(
    Interface565& image,
    const Interface565Point& p1,
    const Interface565Point& p2,
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
        const auto dx = std::abs(p2.x() - p1.x());
        const auto dy = std::abs(p2.y() - p1.y());

        const auto sign_x = (p1.x() <= p2.x()) ? 1 : -1;
        const auto sign_y = (p1.y() <= p2.y()) ? 1 : -1;

        auto x = p1.x();
        auto y = p1.y();

        image.setPixel(p1, rgb);

        if (dx > dy)
        {
            auto d = 2 * dy - dx;
            const auto incrE = 2 * dy;
            const auto incrNE = 2 * (dy - dx);

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

                image.setPixel(Interface565Point(x, y), rgb);
            }
        }
        else
        {
            auto d = 2 * dx - dy;
            const auto incrN = 2 * dx;
            const auto incrNE = 2 * (dx - dy);

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

                image.setPixel(Interface565Point(x, y), rgb);
            }
        }
    }
}

//-------------------------------------------------------------------------

void
raspifb16::
horizontalLine(
    Interface565& image,
    int x1,
    int x2,
    int y,
    uint16_t rgb)
{
    const auto sign_x = (x1 <= x2) ? 1 : -1;
    auto x = x1;

    image.setPixel(Interface565Point(x, y), rgb);

    while (x != x2)
    {
        x += sign_x;
        image.setPixel(Interface565Point(x, y), rgb);
    }
}

//-------------------------------------------------------------------------

void
raspifb16::
verticalLine(
    Interface565& image,
    int x,
    int y1,
    int y2,
    uint16_t rgb)
{
    const auto sign_y = (y1 <= y2) ? 1 : -1;
    auto y = y1;

    image.setPixel(Interface565Point(x, y), rgb);

    while (y != y2)
    {
        y += sign_y;
        image.setPixel(Interface565Point(x, y), rgb);
    }
}

//=========================================================================

namespace raspifb16
{

//-------------------------------------------------------------------------

void
circleLines(
    Interface565& image,
    int x,
    int y,
    int i,
    int j,
    uint16_t rgb)
{
    horizontalLine(image, x + i, x - i, y + j, rgb);
    horizontalLine(image, x + i, x - i, y - j, rgb);
}

//-------------------------------------------------------------------------

void
circlePoints(
    Interface565& image,
    int x,
    int y,
    int i,
    int j,
    uint16_t rgb)
{
    image.setPixel(Interface565Point(x + i, y + j), rgb);
    image.setPixel(Interface565Point(x - i, y + j), rgb);
    image.setPixel(Interface565Point(x + i, y - j), rgb);
    image.setPixel(Interface565Point(x - i, y - j), rgb);

    if (i != j)
    {
        image.setPixel(Interface565Point(x + j, y + i), rgb);
        image.setPixel(Interface565Point(x + j, y - i), rgb);
        image.setPixel(Interface565Point(x - j, y + i), rgb);
        image.setPixel(Interface565Point(x - j, y - i), rgb);
    }
}

//-------------------------------------------------------------------------

} // namespace raspifb16

//=========================================================================

void
raspifb16::
circle(
    Interface565& image,
    const Interface565Point& p,
    int r,
    uint16_t rgb)
{
    int j = r;
    int d = 1 - r;
    int deltaE = 3;
    int deltaSE = -2 * r + 5;

    for (int i = 0 ;  i <= j ; ++i)
    {
        circlePoints(image, p.x(), p.y(), i, j, rgb);

        deltaE += 2;
        if (d < 0)
        {
            d += deltaE;
            deltaSE += 2;
        }
        else
        {
            d += deltaSE;
            deltaSE += 4;
            --j;
        }
    }
}

//-------------------------------------------------------------------------

void
raspifb16::
circleFilled(
    Interface565& image,
    const Interface565Point& p,
    int r,
    uint16_t rgb)
{
    int j = r;
    int d = 1 - r;

    for (int i = 0 ; i <= j ; ++i)
    {
        if (d < 0)
        {
            d += (2 * i) + 3;
        }
        else
        {
            circleLines(image, p.x(), p.y(), i - 1, j, rgb);

            d += (2 * (i - j)) + 5;
            --j;
        }
    }

    int i = j;

    while (j > 0)
    {
        circleLines(image, p.x(), p.y(), i, j, rgb);

        if (d < 0)
        {
            d += (2 * (i - j)) + 5;
            ++i;
        }
        else
        {
            d += 3 - (2 * j);
        }
        --j;
    }

    circleLines(image, p.x(), p.y(), r - 1, 0, rgb);
}

