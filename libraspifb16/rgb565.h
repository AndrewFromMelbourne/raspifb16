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

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

class RGB565
{
public:

    RGB565(uint8_t red, uint8_t green, uint8_t blue) noexcept;

    explicit RGB565(uint16_t rgb) noexcept;

    [[nodiscard]] RGB565 blend(uint8_t alpha, const RGB565& background) const noexcept;

    [[nodiscard]] uint8_t getRed() const noexcept;
    [[nodiscard]] uint8_t getGreen() const noexcept;
    [[nodiscard]] uint8_t getBlue() const noexcept;

    [[nodiscard]] uint16_t get565() const noexcept { return m_rgb; }

    void setRGB(uint8_t red, uint8_t green, uint8_t blue) noexcept;

    void set565(uint16_t rgb) noexcept { m_rgb = rgb; }

    [[nodiscard]] static RGB565 blend(uint8_t alpha, const RGB565& a, const RGB565& b) noexcept;

private:

    uint16_t m_rgb;
};

inline bool operator != (const RGB565& lhs, const RGB565& rhs) noexcept
{
    return lhs.get565() != rhs.get565();
}

inline bool operator == (const RGB565& lhs, const RGB565& rhs) noexcept
{
    return !(lhs != rhs);
}

//-------------------------------------------------------------------------

} // namespace raspifb16

//-------------------------------------------------------------------------

