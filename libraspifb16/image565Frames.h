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

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <span>
#include <vector>

#include "dimensions.h"
#include "rgb565.h"
#include "interface565Base.h"
#include "point.h"

//-------------------------------------------------------------------------

namespace fb16
{

//-------------------------------------------------------------------------

class Image565Frames
:
    public Interface565Base
{
public:

    Image565Frames() = default;

    explicit
    Image565Frames(Dimensions565 d,
             uint8_t numberOfFrames = 1);

    Image565Frames(Dimensions565 d,
             std::initializer_list<uint16_t> buffer,
             uint8_t numberOfFrames = 1);

    Image565Frames(Dimensions565 d,
             std::span<const uint16_t> buffer,
             uint8_t numberOfFrames = 1);

    ~Image565Frames() override = default;

    Image565Frames(const Image565Frames&) = default;
    Image565Frames(Image565Frames&&) = default;
    Image565Frames& operator=(const Image565Frames&) = default;
    Image565Frames& operator=(Image565Frames&&) = default;

    [[nodiscard]] Dimensions565 getDimensions() const noexcept override { return m_dimensions; }

    [[nodiscard]] uint8_t getFrame() const noexcept { return m_frame; }
    [[nodiscard]] uint8_t getNumberOfFrames() const noexcept { return m_numberOfFrames; }
    void setFrame(uint8_t frame) noexcept;

    [[nodiscard]] std::span<uint16_t> getBuffer() noexcept override { return m_buffer; };
    [[nodiscard]] std::span<const uint16_t> getBuffer() const noexcept override { return m_buffer; };
    [[nodiscard]] int getLineLengthPixels() const noexcept override { return m_dimensions.width(); };
    [[nodiscard]] std::size_t offset(const Point565 p) const noexcept override;

private:

    Dimensions565 m_dimensions;

    uint8_t m_frame{};
    uint8_t m_numberOfFrames{};

    std::vector<uint16_t> m_buffer{};
};

//-------------------------------------------------------------------------

} // namespace fb16

//-------------------------------------------------------------------------

