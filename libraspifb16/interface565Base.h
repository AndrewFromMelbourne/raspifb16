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

#include "dimensions.h"
#include "interface565.h"
#include "point.h"
#include "rgb565.h"

//-------------------------------------------------------------------------

namespace fb16
{

//-------------------------------------------------------------------------

class Interface565Base
:
    public Interface565
{
public:

    ~Interface565Base() override = default;

    void clear(const RGB565& rgb) override { clear(rgb.get565()); }
    void clear(uint16_t rgb = 0) override;

    void clearBuffers(const RGB565& rgb) { clearBuffers(rgb.get565()); }
    virtual void clearBuffers(uint16_t rgb = 0);

    [[nodiscard]] std::optional<RGB565> getPixelRGB(const Point565 p) const override;
    [[nodiscard]] virtual std::optional<RGB8> getPixelRGB8(const Point565 p) const override;
    [[nodiscard]] std::optional<uint16_t> getPixel(const Point565 p) const override;

    [[nodiscard]] std::span<uint16_t> getRow(int y);
    [[nodiscard]] std::span<const uint16_t> getRow(int y) const;

    bool
    setPixelRGB(
        const Point565 p,
        const RGB565& rgb) override
    {
        return setPixel(p, rgb.get565());
    }

    bool
    setPixelRGB8(
        const Point565 p,
        RGB8 rgb) override
    {
        return setPixel(p, RGB565(rgb).get565());
    }

    bool setPixel(const Point565 p, uint16_t rgb) override;

    bool putImage(const Point565, const Interface565Base&);

    [[nodiscard]] bool
    validPixel(const Point565 p) const override
    {
        const auto d = getDimensions();

        return (p.x() >= 0) and
               (p.y() >= 0) and
               (p.x() < d.width()) and
               (p.y() < d.height());
    }

    [[nodiscard]] virtual std::span<uint16_t> getBuffer() noexcept = 0;
    [[nodiscard]] virtual std::span<const uint16_t> getBuffer() const noexcept = 0;
    [[nodiscard]] virtual int getLineLengthPixels() const noexcept = 0;
    [[nodiscard]] virtual size_t offset(const Point565 p) const noexcept = 0;

    [[nodiscard]] virtual bool ownable() const noexcept { return false; }
    [[nodiscard]] virtual bool owned() noexcept { return false; }
    virtual void own() noexcept {}
    virtual void disown() noexcept {}

    virtual bool update() { return false; }

private:

    bool putImagePartial(const Point565 p, const Interface565Base& image);

    std::span<uint16_t> getBufferStart() noexcept;
};

//-------------------------------------------------------------------------

} // namespace fb16

