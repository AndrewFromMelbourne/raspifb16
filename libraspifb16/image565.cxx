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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image565.h"

//-------------------------------------------------------------------------

using size_type = std::vector<uint16_t>::size_type;

raspifb16::Image565:: Image565(
    int16_t width,
    int16_t height)
:
    m_width{width},
    m_height{height},
    m_buffer(width * height)
{
}

//-------------------------------------------------------------------------

void
raspifb16::Image565:: clear(
    uint16_t rgb)
{
    std::fill(m_buffer.begin(), m_buffer.end(), rgb);
}

//-------------------------------------------------------------------------

bool
raspifb16::Image565:: setPixel(
    const Image565Point& p,
    uint16_t rgb)
{
    bool isValid{validPixel(p)};

    if (isValid)
    {
        m_buffer[p.x() + (p.y() * m_width)] = rgb;
    }

    return isValid;
}

//-------------------------------------------------------------------------

std::pair<bool, raspifb16::RGB565>
raspifb16::Image565:: getPixelRGB(
    const Image565Point& p) const
{
    bool isValid{validPixel(p)};
    RGB565 rgb{0, 0, 0};

    if (isValid)
    {
        rgb.set565(m_buffer[p.x() + (p.y() * m_width)]);
    }

    return std::make_pair(isValid, rgb);
}

//-------------------------------------------------------------------------

std::pair<bool, uint16_t>
raspifb16::Image565:: getPixel(
    const Image565Point& p) const
{
    bool isValid{validPixel(p)};
    uint16_t rgb{0};

    if (isValid)
    {
        rgb = m_buffer[p.x() + (p.y() * m_width)];
    }

    return std::make_pair(isValid, rgb);
}

//-------------------------------------------------------------------------

const uint16_t*
raspifb16::Image565:: getRow(
    int16_t y) const
{
    if (validPixel(Image565Point{0, y}))
    {
        return m_buffer.data() + (y * m_width);
    }
    else
    {
        return nullptr;
    }
}

