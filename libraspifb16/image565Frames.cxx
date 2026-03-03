//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2026 Andrew Duncan
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

#include "image565Frames.h"

//-------------------------------------------------------------------------

using size_type = std::vector<uint16_t>::size_type;

//-------------------------------------------------------------------------

fb16::Image565Frames::Image565Frames(
    fb16::Dimensions565 d,
    uint8_t numberOfFrames)
:
    m_dimensions{d},
    m_frame{0},
    m_numberOfFrames{numberOfFrames},
    m_buffer(d.width() * d.height() * numberOfFrames)
{
}

//-------------------------------------------------------------------------

fb16::Image565Frames::Image565Frames(
    fb16::Dimensions565 d,
    std::initializer_list<uint16_t> buffer,
    uint8_t numberOfFrames)
:
    m_dimensions{d},
    m_frame{0},
    m_numberOfFrames{numberOfFrames},
    m_buffer{buffer}
{
    const std::size_t minBufferSize = d.width() * d.height() * numberOfFrames;

    if (m_buffer.size() < minBufferSize)
    {
        m_buffer.reserve(minBufferSize);
    }
}

//-------------------------------------------------------------------------
fb16::Image565Frames::Image565Frames(
    fb16::Dimensions565 d,
    std::span<const uint16_t> buffer,
    uint8_t numberOfFrames)
:
    m_dimensions{d},
    m_frame{0},
    m_numberOfFrames{numberOfFrames},
    m_buffer{}
{
    m_buffer.assign(buffer.begin(), buffer.end());

    const std::size_t minBufferSize = d.width() * d.height() * numberOfFrames;

    if (m_buffer.size() < minBufferSize)
    {
        m_buffer.reserve(minBufferSize);
    }
}

//-------------------------------------------------------------------------

void
fb16::Image565Frames::setFrame(
    uint8_t frame) noexcept
{
    if (frame < m_numberOfFrames)
    {
        m_frame = frame;
    }
}

//-------------------------------------------------------------------------

std::size_t
fb16::Image565Frames::offset(
    const Point565 p) const noexcept
{
    return p.x() + (p.y() * m_dimensions.width()) +
                    (m_frame * m_dimensions.width() * m_dimensions.height());
}

