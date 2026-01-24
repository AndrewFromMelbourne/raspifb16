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
#include <optional>
#include <string>
#include <utility>

#include <linux/fb.h>

#include "fileDescriptor.h"
#include "interface565.h"
#include "point.h"
#include "rgb565.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

class Image565;

//-------------------------------------------------------------------------

class FrameBuffer565
:
    public Interface565
{
public:

    static constexpr std::size_t c_bytesPerPixel{2};

    explicit FrameBuffer565(const std::string& device);

    ~FrameBuffer565();

    FrameBuffer565(const FrameBuffer565& fb) = delete;
    FrameBuffer565& operator=(const FrameBuffer565& fb) = delete;

    FrameBuffer565(FrameBuffer565&& fb) = delete;
    FrameBuffer565& operator=(FrameBuffer565&& fb) = delete;

    [[nodiscard]] int32_t getWidth() const noexcept override { return m_vinfo.xres; }
    [[nodiscard]] int32_t getHeight() const noexcept override { return m_vinfo.yres; }

    bool hideCursor() noexcept;

    [[nodiscard]] std::span<uint16_t> getBuffer() noexcept override { return {m_fbp, getBufferSize()}; };
    [[nodiscard]] std::span<const uint16_t> getBuffer() const noexcept override { return {m_fbp, getBufferSize()}; }
    [[nodiscard]] std::size_t getBufferSize() const noexcept { return m_lineLengthPixels * getHeight(); }
    [[nodiscard]] int getLineLengthPixels() const noexcept override { return m_lineLengthPixels; }
    [[nodiscard]] std::size_t offset(const Interface565Point p) const noexcept override;

private:

    fd::FileDescriptor m_consolefd;

    struct fb_fix_screeninfo m_finfo;
    struct fb_var_screeninfo m_vinfo;

    int32_t m_lineLengthPixels;

    uint16_t* m_fbp;
};

//-------------------------------------------------------------------------

} // namespace raspifb16

//-------------------------------------------------------------------------

