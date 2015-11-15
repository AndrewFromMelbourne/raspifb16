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

CImage565:: CImage565(
    int16_t width,
    int16_t height)
:
    m_width(width),
    m_height(height),
    m_buffer(width * height)
{
}

//-------------------------------------------------------------------------

void
CImage565:: clear(
    uint16_t rgb)
{
    std::fill(m_buffer.begin(), m_buffer.end(), rgb);
}

//-------------------------------------------------------------------------

bool
CImage565:: setPixel(
    int16_t x,
    int16_t y,
    uint16_t rgb)
{
    bool isValid = validPixel(x, y);

    if (isValid)
    {
        m_buffer[x + (y * m_width)] = rgb;
    }

    return isValid;
}

//-------------------------------------------------------------------------

bool
CImage565:: getPixel(
    int16_t x,
    int16_t y,
    CRGB565& rgb) const
{
    bool isValid = validPixel(x, y);

    if (isValid)
    {
        rgb.set565(m_buffer[x + (y * m_width)]);
    }

    return isValid;
}

//-------------------------------------------------------------------------

bool
CImage565:: getPixel(
    int16_t x,
    int16_t y,
    uint16_t& rgb) const
{
    bool isValid = validPixel(x, y);

    if (isValid)
    {
        rgb = m_buffer[x + (y * m_width)];
    }

    return isValid;
}

//-------------------------------------------------------------------------

const uint16_t*
CImage565:: getRow(
    int16_t y) const
{
    if (validPixel(0, y))
    {
        return m_buffer.data() + (y * m_width);
    }
    else
    {
        return nullptr;
    }
}

//-------------------------------------------------------------------------

bool
CImage565:: validPixel(
    int16_t x,
    int16_t y) const
{
    if ((x < 0) || (y < 0) || (x >= m_width) || (y >= m_height))
    {
        return false;
    }
    else
    {
        return true;
    }
}

