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

#include "framebuffer565.h"

//-------------------------------------------------------------------------


CFrameBuffer565:: CFrameBuffer565(
    const char* device)
:
    m_fbfd(open(device, O_RDWR)),
    m_consolefd(-1),
    m_fbp(nullptr)
{
    if (m_fbfd == -1)
    {
        perror("Error: cannot open framebuffer device");
        exit(EXIT_FAILURE);
    }

    if (ioctl(m_fbfd, FBIOGET_FSCREENINFO, &(m_finfo)) == -1)
    {
        perror("Error: reading fixed frame buffer information");
        exit(EXIT_FAILURE);
    }

    if (ioctl(m_fbfd, FBIOGET_VSCREENINFO, &(m_vinfo)) == -1)
    {
        perror("Error: reading variable frame buffer information");
        exit(EXIT_FAILURE);
    }

    //---------------------------------------------------------------------

    void* fbp = mmap(nullptr,
                m_finfo.smem_len,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                m_fbfd,
                0);

    if (fbp == MAP_FAILED)
    {
        perror("Error: failed to map framebuffer device to memory");
        exit(EXIT_FAILURE);
    }

    m_fbp = static_cast<uint16_t*>(fbp);
}

//-------------------------------------------------------------------------

CFrameBuffer565:: ~CFrameBuffer565()
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

int
CFrameBuffer565:: getWidth() const
{
    return m_vinfo.xres;
}

//-------------------------------------------------------------------------

int
CFrameBuffer565:: getHeight() const
{
    return m_vinfo.yres;
}

//-------------------------------------------------------------------------

bool
CFrameBuffer565:: hideCursor()
{
    const char* consoleDevice = "/dev/console";

    char* name = ttyname(0);

    if (strstr(name, "/dev/pts"))
    {
        m_consolefd = open(consoleDevice, O_NONBLOCK);

        if (m_consolefd == -1)
        {
            return false;
        }
    }
    else
    {
        m_consolefd = 0;
    }

    ioctl(m_consolefd, KDSETMODE, KD_GRAPHICS);
}

//-------------------------------------------------------------------------

void
CFrameBuffer565:: clear() const
{
    memset(m_fbp, 0, m_finfo.smem_len);
}

//-------------------------------------------------------------------------

void
CFrameBuffer565:: clear(
    const CRGB565& rgb) const
{
    clear(rgb.get565());
}

//-------------------------------------------------------------------------

void
CFrameBuffer565:: clear(
    uint16_t rgb) const
{
    uint16_t* fbp = m_fbp;

    for (
        size_t location = 0 ;
        location < m_finfo.smem_len / 2 ;
        ++location)
    {
        *(fbp++) = rgb;
    }
}

//-------------------------------------------------------------------------

bool
CFrameBuffer565:: setPixel(
    int x,
    int y,
    const CRGB565& rgb) const
{
    bool isValid = validPixel(x, y);

    if (isValid)
    {
        size_t location = x + y * (m_finfo.line_length / 2);

        m_fbp[location] = rgb.get565();
    }

    return isValid;
}

//-------------------------------------------------------------------------

bool
CFrameBuffer565:: setPixel(
    int x,
    int y,
    uint16_t rgb) const
{
    bool isValid = validPixel(x, y);

    if (isValid)
    {
        size_t location = x + y * (m_finfo.line_length / 2);

        m_fbp[location] = rgb;
    }

    return isValid;
}

//-------------------------------------------------------------------------

bool
CFrameBuffer565:: getPixel(
    int x,
    int y,
    CRGB565& rgb) const
{
    bool isValid = validPixel(x, y);

    if (isValid)
    {
        size_t location = x + y * (m_finfo.line_length / 2);

        rgb.set565(m_fbp[location]);
    }

    return isValid;
}

//-------------------------------------------------------------------------

bool
CFrameBuffer565:: getPixel(
    int x,
    int y,
    uint16_t& rgb) const
{
    bool isValid = validPixel(x, y);

    if (isValid)
    {
        size_t location = x + y * (m_finfo.line_length / 2);

        rgb = m_fbp[location];
    }

    return isValid;
}

//-------------------------------------------------------------------------

bool
CFrameBuffer565:: putImage(
    int x,
    int y,
    const CImage565& image) const
{
    if ((x < 0) || ((x + image.getWidth()) >  m_vinfo.xres))
    {
        return putImagePartial(x, y, image);
    }

    if ((y < 0) || ((y + image.getHeight()) > m_vinfo.yres))
    {
        return putImagePartial(x, y, image);
    }

    size_t rowSize = image.getWidth() * sizeof(uint16_t);

    for (int j = 0 ; j < image.getHeight() ; ++j)
    {
        memcpy(m_fbp + ((j + y) * (m_finfo.line_length / 2)) + x,
               image.getRow(j),
               rowSize);
    }
}

//-------------------------------------------------------------------------

bool
CFrameBuffer565:: putImagePartial(
    int x,
    int y,
    const CImage565& image) const
{
    int xStart = 0;
    int xEnd = image.getWidth() - 1;

    int yStart = 0;
    int yEnd = image.getHeight() - 1;

    if (x < 0)
    {
        xStart = -x;
        x = 0;
    }

    if ((x - xStart + image.getWidth()) > m_vinfo.xres)
    {
        xEnd = m_vinfo.xres - 1 - (x - xStart);
    }

    if (y < 0)
    {
        yStart = -y;
        y = 0;
    }

    if ((y - yStart + image.getHeight()) > m_vinfo.yres)
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

    size_t rowSize = (xEnd - xStart + 1) * sizeof(uint16_t);

    for (int j = yStart ; j <= yEnd ; ++j)
    {
        memcpy(m_fbp + ((j + y) * (m_finfo.line_length / 2 )) + x,
               image.getRow(j) + xStart,
               rowSize);
    }
}

//-------------------------------------------------------------------------

bool
CFrameBuffer565:: validPixel(
    int x,
    int y) const
{
    if ((x < 0) || (y < 0) || (x >= m_vinfo.xres) || (y >= m_vinfo.yres))
    {
        return false;
    }
    else
    {
        return true;
    }
}

