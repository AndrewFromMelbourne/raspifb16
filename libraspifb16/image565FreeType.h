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

#include <ft2build.h>
#include <freetype/freetype.h>

#include <cstdint>
#include <string>

#include "image565.h"
#include "interface565Font.h"
#include "point.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

using FontPoint = Point<int>;

//-------------------------------------------------------------------------

class RGB565;

//-------------------------------------------------------------------------

class Image565FreeType
:
    public Interface565Font
{
public:

    Image565FreeType(const std::string& fontFile, int pixelSize);
    ~Image565FreeType() override;

    Image565FreeType(const Image565FreeType&) = delete;
    Image565FreeType(Image565FreeType&&) = delete;
    Image565FreeType& operator=(const Image565FreeType&) = delete;
    Image565FreeType& operator=(Image565FreeType&&) = delete;

	std::string getFontFamilyName() const;
	std::string getFontStyleName() const;

	int getPixelHeight() const override;
	int getPixelWidth() const override;

    int getPixelSize() const
	{
	    return m_pixelSize;
	}

	bool setPixelSize(int pixelSize);

    FontPoint
    drawChar(
        const Image565Point& p,
        uint8_t c,
        const RGB565& rgb,
        Interface565& image) override;

    FontPoint
    drawChar(
        const Image565Point& p,
        uint8_t c,
        uint16_t rgb,
        Interface565& image) override;

    FontPoint
    drawString(
        const Image565Point& p,
        const char* string,
        const RGB565& rgb,
        Interface565& image) override;

    FontPoint
    drawString(
        const Image565Point& p,
        const char* string,
        uint16_t rgb,
        Interface565& image) override;

    FontPoint
    drawString(
        const Image565Point& p,
        const std::string& string,
        const RGB565& rgb,
        Interface565& image) override;

    FontPoint
    drawString(
        const Image565Point& p,
        const std::string& string,
        uint16_t rgb,
        Interface565& image) override;

private:


    void
    drawChar(
        int xOffset,
        int yOffset,
        const FT_Bitmap& bitmap,
        const RGB565& rgb,
        Interface565& image);

    int m_pixelSize;

    FT_Face m_face;
    FT_Library m_library;
};

//-------------------------------------------------------------------------

} // namespace raspifb16

//-------------------------------------------------------------------------

