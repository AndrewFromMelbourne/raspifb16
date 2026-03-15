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

#include <string>
#include <string_view>

#include "interface565.h"
#include "point.h"

//-------------------------------------------------------------------------

namespace fb16
{

//-------------------------------------------------------------------------

class RGB565;

//-------------------------------------------------------------------------

class Interface565Font
{
public:

    enum class CharacterCode
    {
        DEGREE_SYMBOL
    };

    Interface565Font() = default;
    virtual ~Interface565Font() = default;

    Interface565Font(const Interface565Font&) = delete;
    Interface565Font(Interface565Font&&) = delete;
    Interface565Font& operator=(const Interface565Font&) = delete;
    Interface565Font& operator=(Interface565Font&&) = delete;

    [[nodiscard]] virtual Dimensions565 getPixelDimensions() const noexcept = 0;

    [[nodiscard]] virtual std::optional<char> getCharacterCode(CharacterCode code) const noexcept = 0;

    [[nodiscard]] virtual Dimensions565 getStringDimensions(std::string_view s) = 0;


    virtual Point565
    drawChar(
        const Point565 p,
        uint8_t c,
        const RGB565& rgb,
        Interface565& image) = 0;

    virtual Point565
    drawChar(
        const Point565 p,
        uint8_t c,
        uint16_t rgb,
        Interface565& image) = 0;

    virtual Point565
    drawString(
        const Point565 p,
        std::string_view sv,
        uint16_t rgb,
        Interface565& image) = 0;

    virtual Point565
    drawString(
        const Point565 p,
        std::string_view sv,
        const RGB565& rgb,
        Interface565& image) = 0;
};

//-------------------------------------------------------------------------

} // namespace fb16

//-------------------------------------------------------------------------

