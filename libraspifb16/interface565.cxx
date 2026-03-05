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

#include "image565.h"
#include "interface565.h"

#include <algorithm>

//-------------------------------------------------------------------------

fb16::Interface565::Interface565()
{
}

//-------------------------------------------------------------------------

fb16::Interface565::~Interface565()
{
}

//-------------------------------------------------------------------------

void
fb16::Interface565::clear(uint16_t rgb)
{
    std::ranges::fill(getBufferStart(), rgb);
}

//-------------------------------------------------------------------------

void
fb16::Interface565::clearBuffers(uint16_t rgb)
{
    clear(rgb);

    if (update())
    {
        clear(rgb);
    }
}

//-------------------------------------------------------------------------

bool
fb16::Interface565::setPixel(
    const Point565 p,
    uint16_t rgb)
{
    bool isValid{validPixel(p)};

    if (isValid)
    {
        *(getBuffer().data() + offset(p)) = rgb;
    }

    return isValid;
}

//-------------------------------------------------------------------------

std::optional<fb16::RGB565>
fb16::Interface565::getPixelRGB(
    const Point565 p) const
{
    if (not validPixel(p))
    {
        return {};
    }

    return RGB565(*(getBuffer().data() + offset(p)));
}

//-------------------------------------------------------------------------

std::optional<fb16::RGB8>
fb16::Interface565::getPixelRGB8(
    const Point565 p) const
{
    if (not validPixel(p))
    {
        return {};
    }

    return RGB8(*(getBuffer().data() + offset(p)));
}

//-------------------------------------------------------------------------

std::optional<uint16_t>
fb16::Interface565::getPixel(
    const Point565 p) const
{
    if (not validPixel(p))
    {
        return {};
    }

    return *(getBuffer().data() + offset(p));
}
//-------------------------------------------------------------------------

std::span<uint16_t>
fb16::Interface565::getRow(
    int y)
{
    const Point565 p{0, y};

    if (validPixel(p))
    {
        return  getBuffer().subspan(offset(p), getDimensions().width());
    }
    else
    {
        return {};
    }
}

//-------------------------------------------------------------------------

std::span<const uint16_t>
fb16::Interface565::getRow(
    int y) const
{
    const Point565 p{0, y};

    if (validPixel(p))
    {
        return  getBuffer().subspan(offset(p), getDimensions().width());
    }
    else
    {
        return {};
    }
}

//-------------------------------------------------------------------------

bool
fb16::Interface565::putImage(
    const Point565 p,
    const Interface565& image)
{
    const auto id = image.getDimensions();
    const auto d = getDimensions();

    if ((p.x() < 0) or
        ((p.x() + id.width()) > d.width()))
    {
        return putImagePartial(p, image);
    }

    if ((p.y() < 0) or
        ((p.y() + id.height()) > d.height()))
    {
        return putImagePartial(p, image);
    }

    for (int j = 0 ; j < id.height() ; ++j)
    {
        auto row = image.getRow(j);
        const auto ost = offset(Point565{p.x(), j + p.y()});

        std::ranges::copy(row, begin(getBuffer().subspan(ost)));
    }

    return true;
}

//-------------------------------------------------------------------------

bool
fb16::Interface565::putImagePartial(
    const Point565 p,
    const Interface565& image)
{
    const auto id = image.getDimensions();
    const auto d = getDimensions();

    auto x = p.x();
    auto xStart = 0;
    auto xEnd = id.width() - 1;

    auto y = p.y();
    auto yStart = 0;
    auto yEnd = id.height() - 1;

    if (x < 0)
    {
        xStart = -x;
        x = 0;
    }

    if ((x - xStart + id.width()) > d.width())
    {
        xEnd = d.width() - 1 - (x - xStart);
    }

    if (y < 0)
    {
        yStart = -y;
        y = 0;
    }

    if ((y - yStart + id.height()) > d.height())
    {
        yEnd = d.height() - 1 - (y - yStart);
    }

    if ((xEnd - xStart) <= 0)
    {
        return false;
    }

    if ((yEnd - yStart) <= 0)
    {
        return false;
    }

    const auto xLength = xEnd - xStart + 1;

    for (auto j = yStart ; j <= yEnd ; ++j)
    {
        auto row = image.getRow(j).subspan(xStart, xLength);
        const auto ost = offset(Point565{x, j - yStart + y});

        std::ranges::copy(row, begin(getBuffer().subspan(ost)));
    }

    return true;
}

//-------------------------------------------------------------------------

std::span<uint16_t>
fb16::Interface565::getBufferStart() noexcept
{
    return getBuffer().subspan(offset(Point565{0, 0}),
                               getLineLengthPixels() * getDimensions().height());
}

//-------------------------------------------------------------------------

fb16::Point565
fb16::center(
    const fb16::Interface565& frame,
    const fb16::Interface565& image) noexcept
{
    const auto fd = frame.getDimensions();
    const auto id = image.getDimensions();

    return {(fd.width() - id.width()) / 2,
            (fd.height() - id.height()) / 2};
}

