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

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rgb565.h"
#include "interface565.h"
#include "point.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

using Image565Point = Point<int>;

//-------------------------------------------------------------------------

class Image565
:
    public Interface565
{
public:

    Image565();
    Image565(int width,
             int height,
             uint8_t numberOfFrames = 1);

    Image565(int width,
             int height,
             const std::vector<uint16_t>& buffer,
             uint8_t numberOfFrames = 1);

    Image565(const Image565&) = default;
    Image565(Image565&&) = default;
    Image565& operator=(const Image565&) = default;
    Image565& operator=(Image565&&) = default;

    int getWidth() const override { return m_width; }
    int getHeight() const override { return m_height; }

    uint8_t getFrame() const { return m_frame; }
    uint8_t getNumberOfFrames() const { return m_numberOfFrames; }
    void setFrame(uint8_t frame);

    void clear(const RGB565& rgb) override { clear(rgb.get565()); }
    void clear(uint16_t rgb) override;

    bool
    setPixelRGB(
        const Image565Point& p,
        const RGB565& rgb)
    {
        return setPixel(p, rgb.get565());
    }

    bool setPixel(const Image565Point& p, uint16_t rgb) override;

    std::optional<RGB565> getPixelRGB(const Image565Point& p) const override;
    std::optional<uint16_t> getPixel(const Image565Point& p) const override;

    const uint16_t* getRow(int y) const;

private:

    bool
    validPixel(const Image565Point& p) const
    {
        return ((p.x() >= 0) and
                (p.y() >= 0) and
                (p.x() < m_width) and
                (p.y() < m_height));
    }

    size_t offset(const Image565Point& p) const;

    int m_width;
    int m_height;

    uint8_t m_frame;
    uint8_t m_numberOfFrames;

    std::vector<uint16_t> m_buffer;
};

//-------------------------------------------------------------------------

} // namespace raspifb16

//-------------------------------------------------------------------------

