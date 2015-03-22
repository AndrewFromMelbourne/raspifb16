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

#include "image565.h"
#include "rgb565.h"

//-------------------------------------------------------------------------

class CFrameBuffer565
{
public:

    explicit CFrameBuffer565(const char* device = "/dev/fb0");

    ~CFrameBuffer565();

    CFrameBuffer565(const CFrameBuffer565& fb) = delete;
    CFrameBuffer565& operator=(const CFrameBuffer565& fb) = delete;

    int getWidth() const;
    int getHeight() const;

    bool hideCursor();

    void clear() const;
    void clear(const CRGB565& rgb) const;
    void clear(uint16_t rgb) const;

    bool setPixel(int x, int y, const CRGB565& rgb) const;
    bool setPixel(int x, int y, uint16_t rgb) const;

    bool getPixel(int x, int y, CRGB565& rgb) const;
    bool getPixel(int x, int y, uint16_t& rgb) const;

    bool putImage(int x, int y, const CImage565& image) const;

private:

    bool putImagePartial(int x, int y, const CImage565& image) const;

    bool validPixel(int x, int y) const;

    int m_fbfd;
    int m_consolefd;
    struct fb_fix_screeninfo m_finfo;
    struct fb_var_screeninfo m_vinfo;
    uint16_t* m_fbp;
};

//-------------------------------------------------------------------------

#endif
