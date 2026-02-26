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

#include <algorithm>
#include <format>
#include <stdexcept>

#include "image565Graphics.h"
#include "interface565Menu.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

bool
Interface565Menu::MenuItem::decrementValue() noexcept
{
    if (m_value > 0)
    {
        --m_value;
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------

bool
Interface565Menu::MenuItem::incrementValue() noexcept
{
    if (m_value < m_values.size() - 1)
    {
        ++m_value;
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------

Interface565Menu::Interface565Menu(
    RGB565 foregroundColour,
    RGB565 backgroundColour,
    RGB565 selectionColour,
    std::initializer_list<MenuItem> items)
:
    m_foregroundColour{foregroundColour},
    m_backgroundColour{backgroundColour},
    m_selectionColour{selectionColour},
    m_selected{0},
    m_items{items},
    m_titleMaximum{0},
    m_valueMaximum{0}
{
    std::ranges::sort(m_items, std::less());

    for (const auto& item : m_items)
    {
        const auto titleLength = item.m_title.length();
        m_titleMaximum = std::max(titleLength, m_titleMaximum);

        for (const auto& value : item.m_values)
        {
            m_valueMaximum = std::max(value.length(), m_valueMaximum);
        }
    }
}

//-------------------------------------------------------------------------

void
Interface565Menu::draw(
    raspifb16::FrameBuffer565& fb,
    Interface565Font& font) const
{
    constexpr auto characterPadding{5};
    constexpr auto padding{4};
    const auto characters = m_titleMaximum + m_valueMaximum + characterPadding;
    const auto width = characters * font.getPixelWidth();

    boxFilled(
        fb,
        raspifb16::Point565(0, 0),
        raspifb16::Point565(
            width + (padding * 2),
            (m_items.size() * font.getPixelHeight()) + (padding * 2)),
            m_backgroundColour);

    box(
        fb,
        raspifb16::Point565(0, 0),
        raspifb16::Point565(
            width + (padding * 2),
            (m_items.size() * font.getPixelHeight()) + (padding * 2)),
            m_selectionColour);

    boxFilled(
        fb,
        raspifb16::Point565(
            padding,
            (m_selected * font.getPixelHeight()) + padding),
        raspifb16::Point565(
            width + padding,
            ((m_selected + 1) * font.getPixelHeight()) + padding),
            m_selectionColour);

    int yOffset = 0;
    for (const auto& item : m_items)
    {
        font.drawString(
            raspifb16::Point565(padding, yOffset + padding),
            std::format(
                " {0:<{1}} : {2}",
                item.m_title,
                m_titleMaximum,
                item.m_values[item.m_value]),
            m_foregroundColour,
            fb);
        yOffset += font.getPixelHeight();
    }
}

//-------------------------------------------------------------------------

std::size_t
Interface565Menu::getValue(
    std::size_t id) const
{
    if (id >= m_items.size())
    {
        throw std::invalid_argument(std::format("Id {} - not in menu", id));
    }

    auto compare = [](const MenuItem& item, std::size_t id)
    {
        return item.m_id < id;
    };

    const auto item = std::lower_bound(begin(m_items), end(m_items), id, compare);

    if ((item != m_items.end()) and (item->m_id == id))
    {
        return item->m_value;
    }

    throw std::invalid_argument(std::format("Id {} - not in menu", id));
}

//-------------------------------------------------------------------------

Interface565Menu::Update
Interface565Menu::update(
    raspifb16::Joystick& js)
{
    if (js.buttonPressed(raspifb16::Joystick::BUTTON_Y))
    {
        if (m_items[m_selected].decrementValue())
        {
            return VALUE_UPDATE;
        }
    }
    if (js.buttonPressed(raspifb16::Joystick::BUTTON_A))
    {
       if (m_items[m_selected].incrementValue())
        {
            return VALUE_UPDATE;
        }
    }

    if (js.buttonPressed(raspifb16::Joystick::BUTTON_X))
    {
        decrementSelected();
        return MENU_UPDATE;
    }

    if (js.buttonPressed(raspifb16::Joystick::BUTTON_B))
    {
        incrementSelected();
        return MENU_UPDATE;
    }

    const auto value = js.getAxes(0);

    if (not value.x and not value.y)
    {
        return NO_UPDATE;
    }

    const auto dx = (value.x)
                  ? (value.x / std::abs(value.x))
                  : 0;
    const auto dy = (value.y)
                  ? (value.y / std::abs(value.y))
                  : 0;

    if (dx and dy)
    {
        return NO_UPDATE;
    }

    if (dx > 0)
    {
       if (m_items[m_selected].incrementValue())
        {
            return VALUE_UPDATE;
        }
     }
    else if (dx < 0)
    {
        if (m_items[m_selected].decrementValue())
        {
            return VALUE_UPDATE;
        }
    }
    else if (dy > 0)
    {
        incrementSelected();
        return MENU_UPDATE;
    }
    else if (dy < 0)
    {
        decrementSelected();
        return MENU_UPDATE;
    }

    return NO_UPDATE;
}

//-------------------------------------------------------------------------

bool
Interface565Menu::setValue(
    std::size_t id,
    std::size_t value)
{
    m_selected = 0;

    if (id >= m_items.size())
    {
        return false;
    }

    auto compare = [](const MenuItem& item, std::size_t id)
    {
        return item.m_id < id;
    };

    auto item = std::lower_bound(begin(m_items), end(m_items), id, compare);

    if ((item != m_items.end()) and (item->m_id == id))
    {
        item->m_value = value;
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------

void
Interface565Menu::decrementSelected() noexcept
{
    if (m_selected > 0)
    {
        --m_selected;
    }
    else
    {
        m_selected = m_items.size() - 1;
    }
}

//-------------------------------------------------------------------------

void
Interface565Menu::incrementSelected() noexcept
{
    if (m_selected < m_items.size() - 1)
    {
        ++m_selected;
    }
    else
    {
        m_selected = 0;
    }
}

//-------------------------------------------------------------------------

} // namespace raspifb16
