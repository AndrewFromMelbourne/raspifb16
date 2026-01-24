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

namespace raspifb16
{

//-------------------------------------------------------------------------

template<typename T>
class Point
{
public:

    constexpr Point(
        T x,
        T y) noexcept
    :
        m_x(x),
        m_y(y)
    {
    }

    [[nodiscard]] constexpr T x() const  noexcept{ return m_x; }
    [[nodiscard]] constexpr T y() const  noexcept{ return m_y; }

    constexpr void
    set(
        T x,
        T y) noexcept
    {
        m_x = x;
        m_y = y;
    }

    constexpr void
    translate(
        T dx,
        T dy) noexcept
    {
        m_x += dx;
        m_y += dy;
    }

    constexpr void
    translateX(
        T dx) noexcept
    {
        m_x += dx;
    }

    constexpr void
    translateY(
        T dy) noexcept
    {
        m_y += dy;
    }

    constexpr void setX(
        T x) noexcept
    {
        m_x = x;
    }

    constexpr void setY(
        T y) noexcept
    {
        m_y = y;
    }

    friend bool operator<=>(const Point& lhs, const Point& rhs) = default;

private:

    T m_x;
    T m_y;
};

//-------------------------------------------------------------------------

} // namespace raspifb16

//-------------------------------------------------------------------------

