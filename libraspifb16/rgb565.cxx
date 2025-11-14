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

raspifb16::RGB565::RGB565(
    RGB8 rgb) noexcept
:
    m_rgb{rgbTo565(rgb.red, rgb.green, rgb.blue)}
{
}

//-------------------------------------------------------------------------

raspifb16::RGB565
raspifb16::RGB565::blend(
    uint8_t alpha,
    const RGB565& background) const noexcept
{
    return blend(alpha, *this, background);
}

//-------------------------------------------------------------------------

raspifb16::RGB8
raspifb16::RGB565::getRGB8() const noexcept
{
    return RGB8{ getRed(), getGreen(), getBlue() };
}

//-------------------------------------------------------------------------

void
raspifb16::RGB565::setRGB8(
    RGB8 rgb8) noexcept
{
    m_rgb = rgbTo565(rgb8.red, rgb8.green, rgb8.blue);
}

//-------------------------------------------------------------------------

raspifb16::RGB565
raspifb16::RGB565::blend(
    uint8_t alpha,
    const RGB565& a,
    const RGB565& b) noexcept
{
    auto blendChannel = [](uint8_t alpha, int a, int b) -> int
    {
        return ((a * alpha) + (b * (255 - alpha))) / 255;
    };

    //---------------------------------------------------------------------

    const auto red = blendChannel(alpha, a.getRed(), b.getRed());
    const auto green = blendChannel(alpha, a.getGreen(), b.getGreen());
    const auto blue = blendChannel(alpha, a.getBlue(), b.getBlue());

    return RGB565(red, green, blue);
}

