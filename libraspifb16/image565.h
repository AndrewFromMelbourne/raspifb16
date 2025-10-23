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

#include "rgb565.h"
#include "interface565.h"
#include "point.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

class Image565
:
    public Interface565
{
public:

    Image565() = default;

    Image565(int width,
             int height,
             uint8_t numberOfFrames = 1);

    Image565(int width,
             int height,
             std::initializer_list<uint16_t> buffer,
             uint8_t numberOfFrames = 1);

    Image565(int width,
             int height,
             std::span<const uint16_t> buffer,
             uint8_t numberOfFrames = 1);

    ~Image565() override = default;

    Image565(const Image565&) = default;
    Image565(Image565&&) = default;
    Image565& operator=(const Image565&) = default;
    Image565& operator=(Image565&&) = default;

    [[nodiscard]] int getWidth() const noexcept override { return m_width; }
    [[nodiscard]] int getHeight() const noexcept override { return m_height; }

    [[nodiscard]] uint8_t getFrame() const noexcept { return m_frame; }
    [[nodiscard]] uint8_t getNumberOfFrames() const noexcept { return m_numberOfFrames; }
    void setFrame(uint8_t frame) noexcept;

    [[nodiscard]] std::span<const uint16_t> getRow(int y) const noexcept;

    [[nodiscard]] std::span<uint16_t> getBuffer() noexcept override { return m_buffer; };
    [[nodiscard]] std::span<const uint16_t> getBuffer() const noexcept override { return m_buffer; };
    [[nodiscard]] int getLineLengthPixels() const noexcept override { return m_width; };
    [[nodiscard]] std::size_t offset(const Interface565Point& p) const noexcept override;

private:

    int m_width{};
    int m_height{};

    uint8_t m_frame{};
    uint8_t m_numberOfFrames{};

    std::vector<uint16_t> m_buffer{};
};

//-------------------------------------------------------------------------

} // namespace raspifb16

//-------------------------------------------------------------------------

