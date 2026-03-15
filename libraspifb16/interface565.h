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

#include "dimensions.h"
#include "point.h"
#include "rgb565.h"

//-------------------------------------------------------------------------

namespace fb16
{

//-------------------------------------------------------------------------

using Dimensions565 = Dimensions<int>;
using Point565 = Point<int>;

//-------------------------------------------------------------------------

class Interface565
{
public:

    static constexpr auto c_bytesPerPixel{2};

    virtual ~Interface565() = default;

    [[nodiscard]] virtual Dimensions565 getDimensions() const noexcept = 0;

    virtual void clear(const RGB565& rgb) = 0;
    virtual void clear(uint16_t rgb = 0) = 0;

    [[nodiscard]] virtual std::optional<RGB565> getPixelRGB(const Point565 p) const = 0;
    [[nodiscard]] virtual std::optional<RGB8> getPixelRGB8(const Point565 p) const = 0;
    [[nodiscard]] virtual std::optional<uint16_t> getPixel(const Point565 p) const = 0;

    virtual bool setPixelRGB(const Point565 p, const RGB565& rgb) = 0;
    virtual bool setPixelRGB8(const Point565 p, RGB8 rgb) = 0;
    virtual bool setPixel(const Point565 p, uint16_t rgb) = 0;

    [[nodiscard]] virtual bool validPixel(const Point565 p) const = 0;
};

//-------------------------------------------------------------------------

inline Point565
center(
    const Interface565& frame,
    const Interface565& image) noexcept
{
    const auto fd = frame.getDimensions();
    const auto id = image.getDimensions();

    return {(fd.width() - id.width()) / 2,
            (fd.height() - id.height()) / 2};
}


//-------------------------------------------------------------------------

} // namespace fb16

