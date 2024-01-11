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

#include "image565.h"

//-------------------------------------------------------------------------

using size_type = std::vector<uint16_t>::size_type;

//-------------------------------------------------------------------------

raspifb16::Image565:: Image565()
:
    m_width{0},
    m_height{0},
    m_frame{0},
    m_numberOfFrames{0},
    m_buffer(0)
{
}

//-------------------------------------------------------------------------

raspifb16::Image565:: Image565(
    int width,
    int height,
    uint8_t numberOfFrames)
:
    m_width{width},
    m_height{height},
    m_frame{0},
    m_numberOfFrames{numberOfFrames},
    m_buffer(width * height * numberOfFrames)
{
}

//-------------------------------------------------------------------------

raspifb16::Image565:: Image565(
    int width,
    int height,
    const std::vector<uint16_t>& buffer,
    uint8_t numberOfFrames)
:
    m_width{width},
    m_height{height},
    m_frame{0},
    m_numberOfFrames{numberOfFrames},
    m_buffer(buffer)
{
    size_t minBufferSize = width * height * numberOfFrames;

    if (m_buffer.size() < minBufferSize)
    {
        m_buffer.resize(minBufferSize);
    }
}

//-------------------------------------------------------------------------

void
raspifb16::Image565:: setFrame(
    uint8_t frame)
{
    if (frame < m_numberOfFrames)
    {
        m_frame = frame;
    }
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
        m_buffer[offset(p)] = rgb;
    }

    return isValid;
}

//-------------------------------------------------------------------------

std::optional<raspifb16::RGB565>
raspifb16::Image565:: getPixelRGB(
    const Image565Point& p) const
{
    if (validPixel(p))
    {
        return RGB565(m_buffer[offset(p)]);
    }

    return {};
}

//-------------------------------------------------------------------------

std::optional<uint16_t>
raspifb16::Image565:: getPixel(
    const Image565Point& p) const
{
    if (validPixel(p))
    {
        return m_buffer[offset(p)];
    }

    return {};
}

//-------------------------------------------------------------------------

const uint16_t*
raspifb16::Image565:: getRow(
    int y) const
{
    const Image565Point p{0, y};

    if (validPixel(p))
    {
        return m_buffer.data() + offset(p);
    }
    else
    {
        return nullptr;
    }
}

//-------------------------------------------------------------------------

size_t
raspifb16::Image565:: offset(
    const Image565Point& p) const
{
    return p.x() + (p.y() * m_width) + (m_frame * m_width * m_height);
}

