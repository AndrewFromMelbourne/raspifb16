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

#ifndef IMAGE565_GRAPHICS_H
#define IMAGE565_GRAPHICS_H

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
    CImage565& image,
    const CImage565Point& p1,
    const CImage565Point& p2,
    uint16_t rgb);

inline void
box(
    CImage565& image,
    const CImage565Point& p1,
    const CImage565Point& p2,
    const CRGB565& rgb)
{
    box(image, p1, p2, rgb.get565());
}

//-------------------------------------------------------------------------

void
boxFilled(
    CImage565& image,
    const CImage565Point& p1,
    const CImage565Point& p2,
    uint16_t rgb);

inline void
boxFilled(
    CImage565& image,
    const CImage565Point& p1,
    const CImage565Point& p2,
    const CRGB565& rgb)
{
    boxFilled(image, p1, p2, rgb.get565());
}

//-------------------------------------------------------------------------

void
line(
    CImage565& image,
    const CImage565Point& p1,
    const CImage565Point& p2,
    uint16_t rgb);

inline void
line(
    CImage565& image,
    const CImage565Point& p1,
    const CImage565Point& p2,
    const CRGB565& rgb)
{
    line(image, p1, p2, rgb.get565());
}

//-------------------------------------------------------------------------

void
horizontalLine(
    CImage565& image,
    int16_t x1,
    int16_t x2,
    int16_t y,
    uint16_t rgb);

inline void
horizontalLine(
    CImage565& image,
    int16_t x1,
    int16_t x2,
    int16_t y,
    const CRGB565& rgb)
{
    horizontalLine(image, x1, x2, y, rgb.get565());
}

//-------------------------------------------------------------------------

void
verticalLine(
    CImage565& image,
    int16_t x,
    int16_t y1,
    int16_t y2,
    uint16_t rgb);

inline void
verticalLine(
    CImage565& image,
    int16_t x,
    int16_t y1,
    int16_t y2,
    const CRGB565& rgb)
{
    verticalLine(image, x, y1, y2, rgb.get565());
}

//-------------------------------------------------------------------------

} // namespace raspifb16

//-------------------------------------------------------------------------

#endif
