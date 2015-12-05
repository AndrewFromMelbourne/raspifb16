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

#ifndef IMAGE565_H
#define IMAGE565_H

//-------------------------------------------------------------------------

#include <cstdint>
#include <vector>

#include "rgb565.h"

//-------------------------------------------------------------------------

class CImage565
{
public:

    CImage565(int16_t width, int16_t height);

    int16_t getWidth() const { return m_width; }
    int16_t getHeight() const { return m_height; }

    void clear(const CRGB565& rgb) { clear(rgb.get565()); }
    void clear(uint16_t rgb);

    bool setPixel(int16_t x, int16_t y, const CRGB565& rgb) { return setPixel(x, y, rgb.get565()); }
    bool setPixel(int16_t x, int16_t y, uint16_t rgb);

    bool getPixel(int16_t x, int16_t y, CRGB565& rgb) const;
    bool getPixel(int16_t x, int16_t y, uint16_t& rgb) const;

    const uint16_t* getRow(int16_t y) const;

private:

    bool
    validPixel(
        int16_t x,
        int16_t y) const
    {
        return ((x >= 0) && (y >= 0) && (x < m_width) && (y < m_height));
    }

    int16_t m_width;
    int16_t m_height;
    std::vector<uint16_t> m_buffer;
};

//-------------------------------------------------------------------------

#endif
