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
#include <string>

#include <linux/fb.h>

#include "point.h"
#include "rgb565.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

using CFB565Point = CPoint<int32_t>;

//-------------------------------------------------------------------------

class CImage565;

//-------------------------------------------------------------------------

class CFrameBuffer565
{
public:

    static const size_t bytesPerPixel{2};

    explicit CFrameBuffer565(const std::string& device = "/dev/fb0");

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

    bool
    setPixel(
        const CFB565Point& p,
        const CRGB565& rgb) const
    {
        return setPixel(p, rgb.get565());
    }

    bool setPixel(const CFB565Point& p, uint16_t rgb) const;

    bool getPixel(const CFB565Point& p, CRGB565& rgb) const;
    bool getPixel(const CFB565Point& p, uint16_t& rgb) const;

    bool putImage(const CFB565Point& p, const CImage565& image) const;

private:

    bool
    putImagePartial(
        const CFB565Point& p,
        const CImage565& image) const;

    bool
    validPixel(const CFB565Point& p) const
    {
        return (p.x() >= 0) &&
               (p.y() >= 0) &&
               (p.x() < static_cast<int32_t>(m_vinfo.xres)) &&
               (p.y() < static_cast<int32_t>(m_vinfo.yres));
    }

    int m_fbfd;
    int m_consolefd;

    struct fb_fix_screeninfo m_finfo;
    struct fb_var_screeninfo m_vinfo;

    int32_t m_lineLengthPixels;

    uint16_t* m_fbp;
};

//-------------------------------------------------------------------------

} // namespace raspifb16

//-------------------------------------------------------------------------

#endif
