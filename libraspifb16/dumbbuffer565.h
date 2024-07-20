//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2024 Andrew Duncan
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

#include "drmMode.h"
#include "point.h"
#include "fileDescriptor.h"
#include "interface565.h"
#include "rgb565.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

using Interface565Point = Point<int32_t>;

//-------------------------------------------------------------------------

class Image565;

//-------------------------------------------------------------------------

class DumbBuffer565
:
    public Interface565
{
public:

    static constexpr size_t bytesPerPixel{2};

    explicit DumbBuffer565(const std::string& device = "");

    ~DumbBuffer565() override;

    DumbBuffer565(const DumbBuffer565& fb) = delete;
    DumbBuffer565& operator=(const DumbBuffer565& fb) = delete;

    DumbBuffer565(DumbBuffer565&& fb) = delete;
    DumbBuffer565& operator=(DumbBuffer565&& fb) = delete;

    int getWidth() const override { return m_width; }
    int getHeight() const override { return m_height; }

    uint16_t* getBuffer() override { return m_fbp; };
    const uint16_t* getBuffer() const override { return m_fbp; };
    int getLineLengthPixels() const override { return m_lineLengthPixels; };
    size_t offset(const Interface565Point& p) const override;

private:

    int m_width;
    int m_height;
    int m_length;
    int m_lineLengthPixels;

    FileDescriptor m_fd;
    uint16_t* m_fbp;
    uint32_t m_fbId;
    uint32_t m_fbHandle;

    uint32_t m_connectorId;
    drm::drmModeCrtc_ptr m_originalCrtc;
};

//-------------------------------------------------------------------------

} // namespace raspifb16

