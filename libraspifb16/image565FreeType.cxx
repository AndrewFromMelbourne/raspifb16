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

#include <stdexcept>

#include "image565FreeType.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------


Image565FreeType::Image565FreeType(
    const std::string& fontFile,
    int pixelSize)
:
    Image565FreeType()
{
    if (FT_Init_FreeType(&m_library) != 0)
    {
        throw std::invalid_argument("FreeType initialization faied");
    }

    if (FT_New_Face(m_library, fontFile.c_str(), 0, &m_face) != 0)
    {
        throw std::invalid_argument("FreeType could not open " + fontFile);
    }

    setPixelSize(pixelSize);
}

//-------------------------------------------------------------------------

Image565FreeType::~Image565FreeType()
{
    FT_Done_FreeType(m_library);
}

//-------------------------------------------------------------------------

std::string
Image565FreeType::getFontFamilyName() const noexcept
{
    return m_face->family_name;
}

//-------------------------------------------------------------------------

std::string
Image565FreeType::getFontStyleName() const noexcept
{
    return m_face->style_name;
}

//-------------------------------------------------------------------------

int
Image565FreeType::getPixelHeight() const noexcept
{
    return (m_face->size->metrics.ascender +
            abs(m_face->size->metrics.descender)) >> 6;
}

//-------------------------------------------------------------------------

int
Image565FreeType::getPixelWidth() const noexcept
{
    return m_face->size->metrics.max_advance >> 6;
}

//-------------------------------------------------------------------------

std::optional<char>
Image565FreeType::getCharacterCode(Interface565Font::CharacterCode code) const noexcept
{
    switch (code)
    {
        using enum Interface565Font::CharacterCode;

    case DEGREE_SYMBOL:

        return char(0xB0);
    }

    return {};
}

//-------------------------------------------------------------------------

bool
Image565FreeType::setPixelSize(
    int pixelSize) noexcept
{
    if (pixelSize == m_pixelSize)
    {
        return true;
    }

    if (FT_Set_Pixel_Sizes(m_face, 0, pixelSize) == 0)
    {
        m_pixelSize = pixelSize;

        return true;
    }

    return false;
}

//-------------------------------------------------------------------------

Interface565Point
Image565FreeType::drawChar(
    const Interface565Point& p,
    uint8_t c,
    const RGB565& rgb,
    Interface565& image)
{
    return drawString(p, std::string(1, c), rgb, image);
}

//-------------------------------------------------------------------------

Interface565Point
Image565FreeType::drawChar(
    const Interface565Point& p,
    uint8_t c,
    uint16_t rgb,
    Interface565& image)
{
    return drawChar(p, c, RGB565(rgb), image);
}

//-------------------------------------------------------------------------

Interface565Point
Image565FreeType::drawWideChar(
    const Interface565Point& p,
    uint32_t c,
    const RGB565& rgb,
    Interface565& image)
{
    Interface565Point position{p};
    position.incrY(m_face->size->metrics.ascender >> 6);
    const auto glyph_index{FT_Get_Char_Index(m_face, c)};

    if (FT_Load_Glyph(m_face, glyph_index, FT_LOAD_RENDER) == 0)
    {
        const auto slot{m_face->glyph};

        drawChar(position.x() + slot->bitmap_left,
                 position.y() - slot->bitmap_top,
                 slot->bitmap,
                 rgb,
                 image);

        position.incrX(slot->advance.x >> 6);
    }

    position.incrY(-(m_face->size->metrics.ascender >> 6));
    return position;
}

//-------------------------------------------------------------------------

Interface565Point
Image565FreeType::drawString(
    const Interface565Point& p,
    std::string_view sv,
    const RGB565& rgb,
    Interface565& image)
{
    Interface565Point position{p};
    position.incrY(m_face->size->metrics.ascender >> 6);

    const auto slot{m_face->glyph};
    const auto use_kerning{FT_HAS_KERNING(m_face)};
    FT_UInt previous{0};

    for (const auto c : sv)
    {
        if (c == '\n')
        {
            position.set(p.x(), position.y() + getPixelHeight());
        }
        else
        {
            const auto glyph_index{FT_Get_Char_Index(m_face, c)};

            if (use_kerning and previous and glyph_index)
            {
                FT_Vector delta;

                FT_Get_Kerning(m_face,
                               previous,
                               glyph_index,
                               ft_kerning_default,
                               &delta);

                position.incrX(delta.x >> 6);
            }

            if (FT_Load_Glyph(m_face, glyph_index, FT_LOAD_RENDER) == 0)
            {
                const auto slot{m_face->glyph};

                drawChar(position.x() + slot->bitmap_left,
                        position.y() - slot->bitmap_top,
                        slot->bitmap,
                        rgb,
                        image);

                position.incrX(slot->advance.x >> 6);
                previous = glyph_index;
            }
        }
    }

    //-----------------------------------------------------------------

    const auto advance{slot->bitmap.width - (slot->advance.x >> 6)};

    if (advance > 0)
    {
        position.incrX(advance);
    }

    position.incrY(-(m_face->size->metrics.ascender >> 6));

    return position;
}

//-------------------------------------------------------------------------

Interface565Point
Image565FreeType::drawString(
    const Interface565Point& p,
    std::string_view sv,
    uint16_t rgb,
    Interface565& image)
{
    return drawString(p, sv, RGB565(rgb), image);
}

//-------------------------------------------------------------------------

void
Image565FreeType::drawChar(
        int xOffset,
        int yOffset,
        const FT_Bitmap& bitmap,
        const RGB565& rgb,
        Interface565& image)
{
    for (unsigned j = 0 ; j < bitmap.rows ; ++j)
    {
        const auto row{bitmap.buffer + (j * bitmap.pitch)};

        for (unsigned i = 0 ; i < bitmap.width ; ++i)
        {
            if (row[i])
            {
                const Interface565Point p{static_cast<int>(i + xOffset),
                                      static_cast<int>(j + yOffset)};
                auto background{image.getPixelRGB(p)};

                if (background)
                {
                    image.setPixelRGB(p, rgb.blend(row[i], *background));
                }
            }
        }
    }
}

//-------------------------------------------------------------------------

} // namespace raspifb16

