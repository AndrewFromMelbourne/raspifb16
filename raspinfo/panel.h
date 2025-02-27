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

#include <cstdint>

#include <sys/types.h>

#include "interface565.h"
#include "interface565Font.h"
#include "image565.h"

//-------------------------------------------------------------------------

class Panel
{
public:

    Panel(
        int width,
        int height,
        int yPosition)
    :
        m_yPosition{yPosition},
        m_image{width, height}
    { }


    virtual ~Panel() = default;

    [[nodiscard]] int getBottom() const noexcept { return m_yPosition + m_image.getHeight(); }

    [[nodiscard]] raspifb16::Image565& getImage() noexcept { return m_image; }
    [[nodiscard]] const raspifb16::Image565& getImage() const noexcept { return m_image; }

    void show(raspifb16::Interface565& fb) const;
    virtual void init(raspifb16::Interface565Font& font) = 0;
    virtual void update(time_t now, raspifb16::Interface565Font& font) = 0;

private:

    int m_yPosition;
    raspifb16::Image565 m_image;
};

//-------------------------------------------------------------------------

