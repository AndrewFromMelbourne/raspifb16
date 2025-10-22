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

#include <array>
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

    //---------------------------------------------------------------------

    static constexpr std::size_t c_bytesPerPixel{2};

    //---------------------------------------------------------------------

    struct DumbBuffer
    {
        uint16_t* m_fbp{nullptr};
        uint32_t m_fbId{0};
        uint32_t m_fbHandle{0};
        int m_length{0};
        int m_lineLengthPixels{0};
    };

    //---------------------------------------------------------------------

    explicit DumbBuffer565(
        const std::string& device = "",
        uint32_t connectorId = 0);

    ~DumbBuffer565() override;

    DumbBuffer565(const DumbBuffer565& fb) = delete;
    DumbBuffer565& operator=(const DumbBuffer565& fb) = delete;

    DumbBuffer565(DumbBuffer565&& fb) = delete;
    DumbBuffer565& operator=(DumbBuffer565&& fb) = delete;

    [[nodiscard]] int getWidth() const noexcept override { return m_width; }
    [[nodiscard]] int getHeight() const noexcept override { return m_height; }

    [[nodiscard]] std::span<uint16_t> getBuffer() noexcept override;
    [[nodiscard]] std::span<const uint16_t> getBuffer() const noexcept override;
    [[nodiscard]] std::size_t getBufferSize() const noexcept;
    [[nodiscard]] int getLineLengthPixels() const noexcept override;
    [[nodiscard]] std::size_t offset(const Interface565Point& p) const noexcept override;

    bool update() override;

private:

    void createDumbBuffer(int index);
    void destroyDumbBuffer(int index);

    void findResources(uint32_t connectorId);

    int m_width;
    int m_height;

    FileDescriptor m_fd;

    std::array<DumbBuffer, 2> m_dbs;
    int m_dbFront;
    int m_dbBack;

    uint32_t m_connectorId;
    uint32_t m_crtcId;
    drmModeModeInfo m_mode;
    drm::drmModeCrtc_ptr m_originalCrtc;
};

//-------------------------------------------------------------------------

} // namespace raspifb16

