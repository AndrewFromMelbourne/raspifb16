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

#ifndef FRAMEBUFFER565_H
#define FRAMEBUFFER565_H

//-------------------------------------------------------------------------

#include <cstdint>

#include <linux/fb.h>

#include "rgb565.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

class CImage565;

//-------------------------------------------------------------------------

class CFrameBuffer565
{
public:

    static const size_t bytesPerPixel{2};

    explicit CFrameBuffer565(const char* device = "/dev/fb0");

    ~CFrameBuffer565();

    CFrameBuffer565(const CFrameBuffer565& fb) = delete;
    CFrameBuffer565& operator=(const CFrameBuffer565& fb) = delete;

    CFrameBuffer565(CFrameBuffer565&& fb) = delete;
    CFrameBuffer565& operator=(CFrameBuffer565&& fb) = delete;

    int32_t getWidth() const { return m_vinfo.xres; }
    int32_t getHeight() const { return m_vinfo.yres; }

    bool hideCursor();

    void clear(const CRGB565& rgb) const { clear(rgb.get565()); }
    void clear(uint16_t rgb = 0) const;

    bool setPixel(int32_t x, int32_t y, const CRGB565& rgb) const;
    bool setPixel(int32_t x, int32_t y, uint16_t rgb) const;

    bool getPixel(int32_t x, int32_t y, CRGB565& rgb) const;
    bool getPixel(int32_t x, int32_t y, uint16_t& rgb) const;

    bool putImage(int32_t x, int32_t y, const CImage565& image) const;

private:

    bool putImagePartial(int32_t x, int32_t y, const CImage565& image) const;

    bool
    validPixel(
        int32_t x,
        int32_t y) const
    {
        return (x >= 0) &&
               (y >= 0) &&
               (x < static_cast<int32_t>(m_vinfo.xres)) &&
               (y < static_cast<int32_t>(m_vinfo.yres));
    }

    int m_fbfd;
    int m_consolefd;

    struct fb_fix_screeninfo m_finfo;
    struct fb_var_screeninfo m_vinfo;

    int32_t m_lineLengthPixels;

    uint16_t* m_fbp;
};

//-------------------------------------------------------------------------

}; // namespace raspifb16

//-------------------------------------------------------------------------

#endif
