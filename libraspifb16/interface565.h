//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2023 Andrew Duncan
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
#include <span>

#include "point.h"
#include "rgb565.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

using Interface565Point = Point<int>;
class Image565;

//-------------------------------------------------------------------------

class Interface565
{
public:

    static constexpr auto c_bytesPerPixel{2};

    Interface565();
    virtual ~Interface565() = 0;

    Interface565(const Interface565&) = default;
    Interface565(Interface565&&) = delete;

    Interface565& operator=(const Interface565&) = default;
    Interface565& operator=(Interface565&&) = delete;

    [[nodiscard]] virtual int getWidth() const noexcept = 0;
    [[nodiscard]] virtual int getHeight() const noexcept = 0;

    void clear(const RGB565& rgb) { clear(rgb.get565()); }
    void clear(uint16_t rgb = 0);

    void clearBuffers(const RGB565& rgb) { clear(rgb.get565()); }
    void clearBuffers(uint16_t rgb = 0);

    bool
    setPixelRGB(
        const Interface565Point& p,
        const RGB565& rgb)
    {
        return setPixel(p, rgb.get565());
    }

    bool
    setPixelRGB8(
        const Interface565Point& p,
        RGB8 rgb)
    {
        return setPixel(p, RGB565(rgb).get565());
    }

    bool setPixel(const Interface565Point& p, uint16_t rgb);

    [[nodiscard]] std::optional<RGB565> getPixelRGB(const Interface565Point& p) const;
    [[nodiscard]] virtual std::optional<RGB8> getPixelRGB8(const Interface565Point& p) const;
    [[nodiscard]] std::optional<uint16_t> getPixel(const Interface565Point& p) const;

    bool putImage(const Interface565Point&, const Image565&);

    [[nodiscard]] bool
    validPixel(const Interface565Point& p) const
    {
        return (p.x() >= 0) and
               (p.y() >= 0) and
               (p.x() < getWidth()) and
               (p.y() < getHeight());
    }

    [[nodiscard]] virtual std::span<uint16_t> getBuffer() noexcept = 0;
    [[nodiscard]] virtual std::span<const uint16_t> getBuffer() const noexcept = 0;
    [[nodiscard]] virtual int getLineLengthPixels() const noexcept = 0;
    [[nodiscard]] virtual size_t offset(const Interface565Point& p) const noexcept = 0;

    virtual bool update() { return false; }

private:

    bool
    putImagePartial(
        const Interface565Point& p,
        const Image565& image);

    std::span<uint16_t> getBufferStart() noexcept;
};

//-------------------------------------------------------------------------

Interface565Point
center(
    const Interface565& frame,
    const Interface565& image) noexcept;

//-------------------------------------------------------------------------

} // namespace raspifb16

