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

#include "interface565Font.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

Interface565Font::Interface565Font()
{
}

//-------------------------------------------------------------------------

Interface565Font::~Interface565Font()
{
}

//-------------------------------------------------------------------------

FontConfig
parseFontConfig(
    const std::string_view fontConfigStr,
    int defaultPixelHeight) noexcept
{
    FontConfig config;
    config.m_pixelHeight = defaultPixelHeight;

    size_t separatorPos = fontConfigStr.find(':');

    if (separatorPos == std::string_view::npos)
    {
        config.m_fontFile = std::string(fontConfigStr);
    }
    else
    {
        config.m_fontFile = std::string(fontConfigStr.substr(0, separatorPos));

        std::string_view pixelHeightStr = fontConfigStr.substr(separatorPos + 1);

        try
        {
            config.m_pixelHeight = std::stoi(std::string(pixelHeightStr));
        }
        catch (const std::exception&)
        {
            // Ignore errors and use default pixel height
        }
    }

    return config;
}

//-------------------------------------------------------------------------

} // namespace raspifb16
