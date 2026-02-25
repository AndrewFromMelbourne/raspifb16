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

#include <compare>
#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

#include "framebuffer565.h"
#include "interface565.h"
#include "interface565Font.h"
#include "joystick.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

class Interface565Menu
{
public:

    //---------------------------------------------------------------------

    struct MenuItem
    {
        MenuItem(
            std::size_t id,
            std::string_view title,
            std::size_t value,
            std::vector<std::string>&& values)
        :
            m_id(id),
            m_title(title),
            m_value(value),
            m_values(std::move(values))
        {}

        //-----------------------------------------------------------------

        bool decrementValue() noexcept;
        bool incrementValue() noexcept;

        auto operator<=>(const MenuItem& rhs) const
        {
            return m_id <=> rhs.m_id;
        }

        std::size_t m_id;
        std::string m_title;
        std::size_t m_value;
        std::vector<std::string> m_values;
    };

    //---------------------------------------------------------------------

    enum Update
    {
        NO_UPDATE,
        MENU_UPDATE,
        VALUE_UPDATE
    };

    //---------------------------------------------------------------------

    Interface565Menu(
        RGB565 forgroundColour,
        RGB565 backgroundColour,
        RGB565 selectionColour,
        std::initializer_list<MenuItem> items);

    void draw(raspifb16::FrameBuffer565& fb, Interface565Font& font) const;
    std::size_t getValue(std::size_t id) const;
    Update update(raspifb16::Joystick& js);
    bool setValue(std::size_t id, std::size_t value);

private:

    void decrementSelected() noexcept;
    void incrementSelected() noexcept;

    RGB565 m_foregroundColour;
    RGB565 m_backgroundColour;
    RGB565 m_selectionColour;
    std::size_t m_selected;
    std::vector<MenuItem> m_items;
    std::size_t m_titleMaximum;
    std::size_t m_valueMaximum;
};

//-------------------------------------------------------------------------

} // namespace raspifb16

//-------------------------------------------------------------------------

