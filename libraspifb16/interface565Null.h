//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2026 Andrew Duncan
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

using Dimensions565 = Dimensions<int>;
using Point565 = Point<int>;

//-------------------------------------------------------------------------

class Interface565Null
:
    public Interface565
{
public:

    ~Interface565Null() override = default;

    [[nodiscard]] Dimensions565 getDimensions() const noexcept override
    {
        return m_dimensions;
    }

    virtual void clear(const RGB565&) override {};
    virtual void clear([[maybe_unused]] uint16_t rgb = 0) override {};

    [[nodiscard]] virtual std::optional<RGB565> getPixelRGB(Point565 p) const override
    {
        if (validPixel(p))
        {
            return RGB565{0};
        }

        return std::nullopt;
    }

    [[nodiscard]] std::optional<RGB8> getPixelRGB8(Point565 p) const override
    {
        if (validPixel(p))
        {
            return RGB8{0, 0, 0};
        }

        return std::nullopt;
    }

    [[nodiscard]] std::optional<uint16_t> getPixel(Point565 p) const override
    {
        if (validPixel(p))
        {
            return 0;
        }

        return std::nullopt;
    }

    bool setPixelRGB(Point565 p, const RGB565&) override { return validPixel(p); }
    bool setPixelRGB8(Point565 p, RGB8)  override { return validPixel(p); }
    bool setPixel(Point565 p, uint16_t)  override { return validPixel(p); }

    bool validPixel(Point565 p) const noexcept override
    {
        const auto d = getDimensions();

        return ((p.x() >= 0) and
                (p.x() < d.width()) and
                (p.y() >= 0) and
                (p.y() < d.height()));
    }

private:

    Dimensions565 m_dimensions;
};

//-------------------------------------------------------------------------

} // namespace fb16

