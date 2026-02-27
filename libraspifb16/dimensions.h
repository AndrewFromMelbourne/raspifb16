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

namespace fb16
{

//-------------------------------------------------------------------------

template<typename T>
class Dimensions
{
public:

    constexpr Dimensions() noexcept
    :
        m_width{},
        m_height{}
    {
    }

    constexpr Dimensions(
        T width,
        T height) noexcept
    :
        m_width(width),
        m_height(height)
    {
    }

    [[nodiscard]] constexpr T area() const noexcept { return m_width * m_height; }

    [[nodiscard]] constexpr T width() const noexcept { return m_width; }
    [[nodiscard]] constexpr T height() const noexcept { return m_height; }

    constexpr void
    set(
        T width,
        T height) noexcept
    {
        m_width = width;
        m_height = height;
    }

    constexpr void setWidth(T width) noexcept
    {
        m_width = width;
    }

    constexpr void setHeight(T height) noexcept
    {
        m_height = height;
    }

    friend bool operator<=>(const Dimensions& lhs, const Dimensions& rhs) = default;

private:

    T m_width;
    T m_height;
};

//-------------------------------------------------------------------------

} // namespace fb16

