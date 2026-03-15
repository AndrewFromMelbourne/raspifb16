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

namespace fb16
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
    RGB565 colourForeground,
    RGB565 colourBackground,
    RGB565 colourSelection,
    const FontConfig& fontConfig,
    std::initializer_list<MenuItem> items)
:
    m_colourForeground{colourForeground},
    m_colourBackground{colourBackground},
    m_colourSelection{colourSelection},
    m_font{createFont(fontConfig)},
    m_selected{0},
    m_items{items},
    m_titleMaximumPixels{0},
    m_valueMaximumPixels{0}
{
    std::ranges::sort(m_items, std::less());

    for (const auto& item : m_items)
    {
        const auto titleDimensions = m_font->getStringDimensions(item.m_title);
        m_titleMaximumPixels = std::max(titleDimensions.width(), m_titleMaximumPixels);

        for (const auto& value : item.m_values)
        {
            const auto valueDimensions = m_font->getStringDimensions(value);
            m_valueMaximumPixels = std::max(valueDimensions.width(), m_valueMaximumPixels);
        }
    }
}

//-------------------------------------------------------------------------

void
Interface565Menu::draw(
    fb16::FrameBuffer565& fb) const
{
    constexpr auto paddingPixels{6};
    constexpr auto padding2Pixels{paddingPixels * 2};
    constexpr auto padding3Pixels{paddingPixels * 3};
    constexpr auto padding4Pixels{paddingPixels * 4};
    const auto d = m_font->getPixelDimensions();
    const auto width = m_titleMaximumPixels + m_valueMaximumPixels + padding3Pixels;

    boxFilled(
        fb,
        fb16::Point565(0, 0),
        fb16::Point565(
            width + padding2Pixels,
            (m_items.size() * d.height()) + padding2Pixels),
            m_colourBackground);

    box(
        fb,
        fb16::Point565(0, 0),
        fb16::Point565(
            width + padding2Pixels,
            (m_items.size() * d.height()) + padding2Pixels),
            m_colourSelection);

    boxFilled(
        fb,
        fb16::Point565(
            paddingPixels,
            (m_selected * d.height()) + paddingPixels),
        fb16::Point565(
            width + paddingPixels,
            ((m_selected + 1) * d.height()) + paddingPixels),
            m_colourSelection);

    int yOffset = paddingPixels;
    for (const auto& item : m_items)
    {
        m_font->drawString(
            fb16::Point565(paddingPixels, yOffset),
            std::format("{}", item.m_title),
            m_colourForeground,
            fb);

        m_font->drawChar(
            fb16::Point565{m_titleMaximumPixels + padding2Pixels, yOffset},
            '|',
            m_colourForeground,
            fb);

        m_font->drawString(
            fb16::Point565(m_titleMaximumPixels + padding4Pixels, yOffset),
            std::format("{}", item.m_values[item.m_value]),
            m_colourForeground,
            fb);

        yOffset += d.height();
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

    const auto item = std::lower_bound(cbegin(m_items), cend(m_items), id, compare);

    if ((item != cend(m_items)) and (item->m_id == id))
    {
        return item->m_value;
    }

    throw std::invalid_argument(std::format("Id {} - not in menu", id));
}

//-------------------------------------------------------------------------

Interface565Menu::Update
Interface565Menu::update(
    fb16::Joystick& js)
{
    if (js.buttonPressed(fb16::Joystick::BUTTON_Y))
    {
        if (m_items[m_selected].decrementValue())
        {
            return VALUE_UPDATE;
        }
    }
    if (js.buttonPressed(fb16::Joystick::BUTTON_A))
    {
       if (m_items[m_selected].incrementValue())
        {
            return VALUE_UPDATE;
        }
    }

    if (js.buttonPressed(fb16::Joystick::BUTTON_X))
    {
        decrementSelected();
        return MENU_UPDATE;
    }

    if (js.buttonPressed(fb16::Joystick::BUTTON_B))
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

    if ((item != cend(m_items)) and (item->m_id == id))
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

} // namespace fb16
