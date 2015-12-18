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

#include "rgb565.h"

//-------------------------------------------------------------------------

raspifb16::CRGB565:: CRGB565(
    uint8_t red,
    uint8_t green,
    uint8_t blue)
:
    m_rgb{0}
{
    setRGB(red, green, blue);
}

//-------------------------------------------------------------------------

raspifb16::CRGB565:: CRGB565(
    uint16_t rgb)
:
    m_rgb{rgb}
{
}

//-------------------------------------------------------------------------

uint8_t
raspifb16::CRGB565:: getRed() const
{
    auto r5 = (m_rgb >> 11) & 0x1F;

    return (r5 << 3) | (r5 >> 2);
}

//-------------------------------------------------------------------------

uint8_t
raspifb16::CRGB565:: getGreen() const
{
    auto g6 = (m_rgb >> 5) & 0x3F;

    return (g6 << 2) | (g6 >> 4);
}

//-------------------------------------------------------------------------

uint8_t
raspifb16::CRGB565:: getBlue() const
{
    auto b5 = m_rgb & 0x1F;

    return (b5 << 3) | (b5 >> 2);
}

//-------------------------------------------------------------------------

void
raspifb16::CRGB565:: setRGB(
    uint8_t red,
    uint8_t green,
    uint8_t blue)
{
    m_rgb = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
}

//-------------------------------------------------------------------------

raspifb16::CRGB565
raspifb16::CRGB565:: blend(
    uint8_t alpha,
    const CRGB565& a,
    const CRGB565& b)
{
    auto red = (((int16_t)(a.getRed()) * alpha)
             + ((int16_t)(b.getRed()) * (255 - alpha)))
             / 255;

    auto green = (((int16_t)(a.getGreen()) * alpha)
               + ((int16_t)(b.getGreen()) * (255 - alpha)))
               / 255;

    auto blue = (((int16_t)(a.getBlue()) * alpha)
              + ((int16_t)(b.getBlue()) * (255 - alpha)))
              / 255;

    return CRGB565(red, green, blue);
}

