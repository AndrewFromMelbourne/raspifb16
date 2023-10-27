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

#include <cstdint>
#include <string>
#include <utility>

#include <linux/fb.h>

#include "fileDescriptor.h"
#include "point.h"
#include "rgb565.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

using FB565Point = Point<int32_t>;

//-------------------------------------------------------------------------

class Image565;

//-------------------------------------------------------------------------

class FrameBuffer565
{
public:

    static constexpr size_t bytesPerPixel{2};

    explicit FrameBuffer565(const std::string& device);

    ~FrameBuffer565();

    FrameBuffer565(const FrameBuffer565& fb) = delete;
    FrameBuffer565& operator=(const FrameBuffer565& fb) = delete;

    FrameBuffer565(FrameBuffer565&& fb) = delete;
    FrameBuffer565& operator=(FrameBuffer565&& fb) = delete;

    int32_t getWidth() const { return m_vinfo.xres; }
    int32_t getHeight() const { return m_vinfo.yres; }

    bool hideCursor();

    void clear(const RGB565& rgb) const { clear(rgb.get565()); }
    void clear(uint16_t rgb = 0) const;

    bool
    setPixelRGB(
        const FB565Point& p,
        const RGB565& rgb) const
    {
        return setPixel(p, rgb.get565());
    }

    bool setPixel(const FB565Point& p, uint16_t rgb) const;

    std::pair<bool, RGB565> getPixelRGB(const FB565Point& p) const;
    std::pair<bool, uint16_t> getPixel(const FB565Point& p) const;

    bool putImage(const FB565Point& p, const Image565& image) const;

private:

    bool
    putImagePartial(
        const FB565Point& p,
        const Image565& image) const;

    bool
    validPixel(const FB565Point& p) const
    {
        return (p.x() >= 0) &&
               (p.y() >= 0) &&
               (p.x() < static_cast<int32_t>(m_vinfo.xres)) &&
               (p.y() < static_cast<int32_t>(m_vinfo.yres));
    }

    FileDescriptor m_consolefd;

    struct fb_fix_screeninfo m_finfo;
    struct fb_var_screeninfo m_vinfo;

    int32_t m_lineLengthPixels;

    uint16_t* m_fbp;
};

//-------------------------------------------------------------------------

} // namespace raspifb16

//-------------------------------------------------------------------------

