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

#include "interface565.h"
#include "point.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

class RGB565;

//-------------------------------------------------------------------------

class Interface565Font
{
public:

    Interface565Font();
    virtual ~Interface565Font() = 0;

    Interface565Font(const Interface565Font&) = delete;
    Interface565Font(Interface565Font&&) = delete;
    Interface565Font& operator=(const Interface565Font&) = delete;
    Interface565Font& operator=(Interface565Font&&) = delete;

    virtual int getPixelHeight() const = 0;
    virtual int getPixelWidth() const = 0;

    virtual Interface565Point
    drawChar(
        const Interface565Point& p,
        uint8_t c,
        const RGB565& rgb,
        Interface565& image) = 0;

    virtual Interface565Point
    drawChar(
        const Interface565Point& p,
        uint8_t c,
        uint16_t rgb,
        Interface565& image) = 0;

    virtual Interface565Point
    drawString(
        const Interface565Point& p,
        const char* string,
        const RGB565& rgb,
        Interface565& image) = 0;

    virtual Interface565Point
    drawString(
        const Interface565Point& p,
        const std::string& string,
        const RGB565& rgb,
        Interface565& image) = 0;
};

//-------------------------------------------------------------------------

} // namespace raspifb16

//-------------------------------------------------------------------------

