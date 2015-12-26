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

#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <algorithm>
#include <string>
#include <system_error>

#include "framebuffer565.h"
#include "image565.h"
#include "point.h"

//-------------------------------------------------------------------------

raspifb16::CFrameBuffer565:: CFrameBuffer565(
    const std::string& device)
:
    m_fbfd{open(device.c_str(), O_RDWR)},
    m_consolefd{-1},
    m_finfo{},
    m_vinfo{},
    m_lineLengthPixels{0},
    m_fbp{nullptr}
{
    if (m_fbfd == -1)
    {
        throw std::system_error{errno,
                                std::system_category(), 
                                "cannot open framebuffer device " + device};
    }

    if (ioctl(m_fbfd, FBIOGET_FSCREENINFO, &(m_finfo)) == -1)
    {
        close(m_fbfd);

        throw std::system_error{errno,
                                std::system_category(), 
                                "reading fixed framebuffer information"};
    }

    if (ioctl(m_fbfd, FBIOGET_VSCREENINFO, &(m_vinfo)) == -1)
    {
        close(m_fbfd);

        throw std::system_error{errno,
                                std::system_category(), 
                                "reading variable framebuffer information"};
    }

    //---------------------------------------------------------------------

    m_lineLengthPixels = m_finfo.line_length / bytesPerPixel;

    //---------------------------------------------------------------------

    void* fbp = mmap(nullptr,
                     m_finfo.smem_len,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     m_fbfd,
                     0);

    if (fbp == MAP_FAILED)
    {
        close(m_fbfd);

        throw std::system_error(errno,
                                std::system_category(), 
                                "mapping framebuffer device to memory");
    }

    m_fbp = static_cast<uint16_t*>(fbp);
}

//-------------------------------------------------------------------------

raspifb16::CFrameBuffer565:: ~CFrameBuffer565()
{
    munmap(m_fbp, m_finfo.smem_len);

    close(m_fbfd);

    if (m_consolefd != -1)
    {
        ioctl(m_consolefd, KDSETMODE, KD_TEXT);

        if (m_consolefd != 0)
        {
            close(m_consolefd);
        }
    }
}

//-------------------------------------------------------------------------

bool
raspifb16::CFrameBuffer565:: hideCursor()
{
    std::string name{ttyname(0)};
    bool result = true;

    if (name.find("/dev/pts") != std::string::npos)
    {
        static const std::string consoleDevice{"/dev/console"};

        m_consolefd = open(consoleDevice.c_str(), O_NONBLOCK);

        if (m_consolefd == -1)
        {
            result = false;
        }
    }
    else
    {
        m_consolefd = 0;
    }

    if (m_consolefd != -1)
    {
        ioctl(m_consolefd, KDSETMODE, KD_GRAPHICS);
    }

    return result;
}

//-------------------------------------------------------------------------

void
raspifb16::CFrameBuffer565:: clear(
    uint16_t rgb) const
{
    std::fill(m_fbp, m_fbp + (m_finfo.smem_len / bytesPerPixel), rgb);
}

//-------------------------------------------------------------------------

bool
raspifb16::CFrameBuffer565:: setPixel(
    const CFB565Point& p,
    uint16_t rgb) const
{
    bool isValid{validPixel(p)};

    if (isValid)
    {
        m_fbp[p.x() + p.y() * m_lineLengthPixels] = rgb;
    }

    return isValid;
}

//-------------------------------------------------------------------------

bool
raspifb16::CFrameBuffer565:: getPixel(
    const CFB565Point& p,
    CRGB565& rgb) const
{
    bool isValid{validPixel(p)};

    if (isValid)
    {
        rgb.set565(m_fbp[p.x() + p.y() * m_lineLengthPixels]);
    }

    return isValid;
}

//-------------------------------------------------------------------------

bool
raspifb16::CFrameBuffer565:: getPixel(
    const CFB565Point& p,
    uint16_t& rgb) const
{
    bool isValid{validPixel(p)};

    if (isValid)
    {
        rgb = m_fbp[p.x() + p.y() * m_lineLengthPixels];
    }

    return isValid;
}

//-------------------------------------------------------------------------

bool
raspifb16::CFrameBuffer565:: putImage(
    const CFB565Point& p,
    const CImage565& image) const
{
    if ((p.x() < 0) ||
        ((p.x() + image.getWidth()) > static_cast<int32_t>(m_vinfo.xres)))
    {
        return putImagePartial(p, image);
    }

    if ((p.y() < 0) ||
        ((p.y() + image.getHeight()) > static_cast<int32_t>(m_vinfo.yres)))
    {
        return putImagePartial(p, image);
    }

    for (int32_t j = 0 ; j < image.getHeight() ; ++j)
    {
        auto start = image.getRow(j);

        std::copy(start,
                  start + image.getWidth(), 
                  m_fbp + ((j + p.y()) * m_lineLengthPixels) + p.x());
    }

    return true;
}

//-------------------------------------------------------------------------

bool
raspifb16::CFrameBuffer565:: putImagePartial(
    const CFB565Point& p,
    const CImage565& image) const
{
    auto x = p.x();
    auto xStart = 0;
    auto xEnd = image.getWidth() - 1;

    auto y = p.y();
    auto yStart = 0;
    auto yEnd = image.getHeight() - 1;

    if (x < 0)
    {
        xStart = -x;
        x = 0;
    }

    if ((x - xStart + image.getWidth()) >
        static_cast<int32_t>(m_vinfo.xres))
    {
        xEnd = m_vinfo.xres - 1 - (x - xStart);
    }

    if (y < 0)
    {
        yStart = -y;
        y = 0;
    }

    if ((y - yStart + image.getHeight()) >
        static_cast<int32_t>(m_vinfo.yres))
    {
        yEnd = m_vinfo.yres - 1 - (y - yStart);
    }

    if ((xEnd - xStart) <= 0)
    {
        return false;
    }

    if ((yEnd - yStart) <= 0)
    {
        return false;
    }

    for (auto j = yStart ; j <= yEnd ; ++j)
    {
        auto start = image.getRow(j) + xStart;

        std::copy(start,
                  start + (xEnd - xStart + 1),
                  m_fbp + ((j+y) * m_lineLengthPixels) + x);
    }

    return true;
}

