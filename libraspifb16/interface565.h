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

#include <cstdint>
#include <utility>

#include "point.h"
#include "rgb565.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

using Interface565Point = Point<int>;

//-------------------------------------------------------------------------

class Interface565
{
public:

    virtual ~Interface565() = 0;

    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;

    virtual void clear(const RGB565& rgb) = 0;
    virtual void clear(uint16_t rgb = 0) = 0;

    virtual bool
    setPixelRGB(
        const Interface565Point& p,
        const RGB565& rgb) = 0;

    virtual bool setPixel(const Interface565Point& p, uint16_t rgb) = 0;

    virtual std::pair<bool, RGB565> getPixelRGB(const Interface565Point& p) const = 0;
    virtual std::pair<bool, uint16_t> getPixel(const Interface565Point& p) const = 0;
};

//-------------------------------------------------------------------------

} // namespace raspifb16

