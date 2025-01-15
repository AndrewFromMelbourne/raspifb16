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
#include <unistd.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <algorithm>
#include <chrono>
#include <string>
#include <system_error>
#include <thread>

#include "framebuffer565.h"
#include "image565.h"
#include "point.h"

//-------------------------------------------------------------------------

raspifb16::FrameBuffer565::FrameBuffer565(
    const std::string& device)
:
    m_consolefd{-1},
    m_finfo{},
    m_vinfo{},
    m_lineLengthPixels{0},
    m_fbp{nullptr}
{
    FileDescriptor fbfd{::open(device.c_str(), O_RDWR)};

    if (fbfd.fd() == -1)
    {
        throw std::system_error{errno,
                                std::system_category(),
                                "cannot open framebuffer device " + device};
    }

    if (ioctl(fbfd.fd(), FBIOGET_FSCREENINFO, &(m_finfo)) == -1)
    {
        throw std::system_error{errno,
                                std::system_category(),
                                "reading fixed framebuffer information"};
    }

    if (ioctl(fbfd.fd(), FBIOGET_VSCREENINFO, &(m_vinfo)) == -1)
    {
        throw std::system_error{errno,
                                std::system_category(),
                                "reading variable framebuffer information"};
    }

    //---------------------------------------------------------------------

    if (m_vinfo.bits_per_pixel != 16)
    {
        throw std::invalid_argument{"expected 16 bits per pixel, found " +
                                    std::to_string(m_vinfo.bits_per_pixel)};
    }

    //---------------------------------------------------------------------

    m_lineLengthPixels = m_finfo.line_length / bytesPerPixel;

    //---------------------------------------------------------------------

    void* fbp = ::mmap(nullptr,
                       m_finfo.smem_len,
                       PROT_READ | PROT_WRITE,
                       MAP_SHARED,
                       fbfd.fd(),
                       0);

    if (fbp == MAP_FAILED)
    {
        throw std::system_error(errno,
                                std::system_category(),
                                "mapping framebuffer device to memory");
    }

    m_fbp = static_cast<uint16_t*>(fbp);
}

//-------------------------------------------------------------------------

raspifb16::FrameBuffer565::~FrameBuffer565()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ::munmap(m_fbp, m_finfo.smem_len);

    if (m_consolefd.fd() != -1)
    {
        ::ioctl(m_consolefd.fd(), KDSETMODE, KD_TEXT);
    }
}

//-------------------------------------------------------------------------

bool
raspifb16::FrameBuffer565::hideCursor() noexcept
{
    std::string name{::ttyname(0)};
    bool result = true;

    if (name.find("/dev/pts") != std::string::npos)
    {
        static const std::string consoleDevice{"/dev/console"};

        m_consolefd = FileDescriptor{::open(consoleDevice.c_str(),
                                            O_NONBLOCK)};

        if (m_consolefd.fd() == -1)
        {
            result = false;
        }
    }
    else
    {
        auto closeIf = [](int) -> bool { return false; };

        m_consolefd = FileDescriptor{0, closeIf};
    }

    if (m_consolefd.fd() != -1)
    {
        ::ioctl(m_consolefd.fd(), KDSETMODE, KD_GRAPHICS);
    }

    return result;
}

//-------------------------------------------------------------------------

size_t
raspifb16::FrameBuffer565::offset(
    const Interface565Point& p) const noexcept
{
    return p.x() + p.y() * m_lineLengthPixels;
}

