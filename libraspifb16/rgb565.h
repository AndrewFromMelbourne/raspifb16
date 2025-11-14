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

struct RGB8
{
    constexpr RGB8(uint8_t r, uint8_t g, uint8_t b)
    :
        red{r},
        green{g},
        blue{b}
    {}

    //---------------------------------------------------------------------

    explicit constexpr RGB8(uint32_t rgb)
    :
        red{},
        green{},
        blue{}
    {
        const auto r5 = (rgb >> 11) & 0x1F;
        red = (r5 << 3) | (r5 >> 2);

        const auto g6 = (rgb >> 5) & 0x3F;
        green = (g6 << 2) | (g6 >> 4);

        const auto b5 = rgb & 0x1F;
        blue = (b5 << 3) | (b5 >> 2);
    }

    //---------------------------------------------------------------------

    uint8_t red{};
    uint8_t green{};
    uint8_t blue{};
};

//-------------------------------------------------------------------------

class RGB565
{
public:

    constexpr RGB565(uint8_t red, uint8_t green, uint8_t blue) noexcept
    :
        m_rgb{rgbTo565(red, green, blue)}
    {
    }

    //---------------------------------------------------------------------

    explicit RGB565(RGB8 rbg) noexcept;

    //---------------------------------------------------------------------

    explicit constexpr RGB565(uint16_t rgb) noexcept
    :
        m_rgb{rgb}
    {
    }

    //---------------------------------------------------------------------

    [[nodiscard]] RGB565 blend(uint8_t alpha, const RGB565& background) const noexcept;

    //---------------------------------------------------------------------

    [[nodiscard]] constexpr uint8_t getRed() const noexcept
    {
        const auto r5 = (m_rgb >> 11) & 0x1F;
        return (r5 << 3) | (r5 >> 2);
    }

    //---------------------------------------------------------------------

    [[nodiscard]] constexpr uint8_t getGreen() const noexcept
    {
        const auto g6 = (m_rgb >> 5) & 0x3F;
        return (g6 << 2) | (g6 >> 4);
    }

    //---------------------------------------------------------------------

    [[nodiscard]] constexpr uint8_t getBlue() const noexcept
    {
        const auto b5 = m_rgb & 0x1F;
        return (b5 << 3) | (b5 >> 2);
    }

    //---------------------------------------------------------------------

    [[nodiscard]] RGB8 getRGB8() const noexcept;
    [[nodiscard]] constexpr uint16_t get565() const noexcept { return m_rgb; }

    //---------------------------------------------------------------------

    [[nodiscard]] constexpr bool isGrey() const noexcept
    {
        return (getRed() == getGreen()) and (getGreen() == getBlue());
    }

    //---------------------------------------------------------------------

    void setGrey(uint8_t grey) noexcept { setRGB(grey, grey, grey); }

    //---------------------------------------------------------------------

    constexpr void setRGB(uint8_t red, uint8_t green, uint8_t blue) noexcept
    {
        m_rgb = rgbTo565(red, green, blue);
    }

    //---------------------------------------------------------------------

    void setRGB8(RGB8 rgb) noexcept;
    constexpr void set565(uint16_t rgb) noexcept { m_rgb = rgb; }

    [[nodiscard]] static RGB565 blend(uint8_t alpha, const RGB565& a, const RGB565& b) noexcept;

    //---------------------------------------------------------------------

    [[nodiscard]] constexpr static uint16_t rgbTo565(uint8_t red, uint8_t green, uint8_t blue) noexcept
    {
        return ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
    }

    //---------------------------------------------------------------------

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

