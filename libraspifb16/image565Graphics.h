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

#pragma once

//-------------------------------------------------------------------------

#include <cstdint>

#include "image565.h"
#include "point.h"
#include "rgb565.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

void
box(
    Interface565& image,
    const Interface565Point& p1,
    const Interface565Point& p2,
    uint16_t rgb);

inline void
box(
    Interface565& image,
    const Interface565Point& p1,
    const Interface565Point& p2,
    const RGB565& rgb)
{
    box(image, p1, p2, rgb.get565());
}

//-------------------------------------------------------------------------

void
boxFilled(
    Interface565& image,
    const Interface565Point& p1,
    const Interface565Point& p2,
    uint16_t rgb);

inline void
boxFilled(
    Interface565& image,
    const Interface565Point& p1,
    const Interface565Point& p2,
    const RGB565& rgb)
{
    boxFilled(image, p1, p2, rgb.get565());
}

//-------------------------------------------------------------------------

void
line(
    Interface565& image,
    const Interface565Point& p1,
    const Interface565Point& p2,
    uint16_t rgb);

inline void
line(
    Interface565& image,
    const Interface565Point& p1,
    const Interface565Point& p2,
    const RGB565& rgb)
{
    line(image, p1, p2, rgb.get565());
}

//-------------------------------------------------------------------------

void
horizontalLine(
    Interface565& image,
    int x1,
    int x2,
    int y,
    uint16_t rgb);

inline void
horizontalLine(
    Interface565& image,
    int x1,
    int x2,
    int y,
    const RGB565& rgb)
{
    horizontalLine(image, x1, x2, y, rgb.get565());
}

//-------------------------------------------------------------------------

void
verticalLine(
    Interface565& image,
    int x,
    int y1,
    int y2,
    uint16_t rgb);

inline void
verticalLine(
    Interface565& image,
    int x,
    int y1,
    int y2,
    const RGB565& rgb)
{
    verticalLine(image, x, y1, y2, rgb.get565());
}

//-------------------------------------------------------------------------

void
circle(
    Interface565& image,
    const Interface565Point& p,
    int r,
    uint16_t rgb);

inline void
circle(
    Interface565& image,
    const Interface565Point& p,
    int r,
    const RGB565& rgb)
{
    circle(image, p, r, rgb.get565());
}

//-------------------------------------------------------------------------

void
circleFilled(
    Interface565& image,
    const Interface565Point& p,
    int r,
    uint16_t rgb);

inline void
circleFilled(
    Interface565& image,
    const Interface565Point& p,
    int r,
    const RGB565& rgb)
{
    circleFilled(image, p, r, rgb.get565());
}

//-------------------------------------------------------------------------

} // namespace raspifb16

//-------------------------------------------------------------------------

