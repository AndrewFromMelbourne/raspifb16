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

//-------------------------------------------------------------------------

raspifb16::Interface565::Interface565()
{
}

//-------------------------------------------------------------------------

raspifb16::Interface565::~Interface565()
{
}

//-------------------------------------------------------------------------

void
raspifb16::Interface565::clear(uint16_t rgb)
{
    auto buffer{getBufferStart()};
    const auto length{getLineLengthPixels() * getHeight()};

    std::fill(buffer, buffer + length, rgb);
}

//-------------------------------------------------------------------------

bool
raspifb16::Interface565::setPixel(
    const Interface565Point& p,
    uint16_t rgb)
{
    bool isValid{validPixel(p)};

    if (isValid)
    {
        *(getBuffer() + offset(p)) = rgb;
    }

    return isValid;
}

//-------------------------------------------------------------------------

std::optional<raspifb16::RGB565>
raspifb16::Interface565::getPixelRGB(
    const Interface565Point& p) const
{
    if (not validPixel(p))
    {
        return {};
    }

    return RGB565(*(getBuffer() + offset(p)));
}

//-------------------------------------------------------------------------

std::optional<uint16_t>
raspifb16::Interface565::getPixel(
    const Interface565Point& p) const
{
    if (not validPixel(p))
    {
        return {};
    }

    return *(getBuffer() + offset(p));
}


//-------------------------------------------------------------------------

bool
raspifb16::Interface565::putImage(
    const Interface565Point& p,
    const Image565& image)
{
    if ((p.x() < 0) or
        ((p.x() + image.getWidth()) > getWidth()))
    {
        return putImagePartial(p, image);
    }

    if ((p.y() < 0) or
        ((p.y() + image.getHeight()) > getHeight()))
    {
        return putImagePartial(p, image);
    }

    for (int32_t j = 0 ; j < image.getHeight() ; ++j)
    {
        auto start = image.getRow(j);

        std::copy(start,
                  start + image.getWidth(),
                  getBufferStart() + ((j + p.y()) * getLineLengthPixels()) + p.x());
    }

    return true;
}

//-------------------------------------------------------------------------

bool
raspifb16::Interface565::putImagePartial(
    const Interface565Point& p,
    const Image565& image)
{
    auto x = p.x();
    auto xStart = 0;
    auto xEnd = image.getWidth() - 1;

    auto y = p.y();
    auto yStart = 0;
    auto yEnd = image.getHeight() - 1;

    if (x < 0)
    {
        xStart = -x;
        x = 0;
    }

    if ((x - xStart + image.getWidth()) > getWidth())
    {
        xEnd = getWidth() - 1 - (x - xStart);
    }

    if (y < 0)
    {
        yStart = -y;
        y = 0;
    }

    if ((y - yStart + image.getHeight()) > getHeight())
    {
        yEnd = getHeight() - 1 - (y - yStart);
    }

    if ((xEnd - xStart) <= 0)
    {
        return false;
    }

    if ((yEnd - yStart) <= 0)
    {
        return false;
    }

    for (auto j = yStart ; j <= yEnd ; ++j)
    {
        auto start = image.getRow(j) + xStart;

        std::copy(start,
                  start + (xEnd - xStart + 1),
                  getBufferStart() + ((j - yStart + y) * getLineLengthPixels()) + x);
    }

    return true;
}

//-------------------------------------------------------------------------

uint16_t*
raspifb16::Interface565::getBufferStart()
{
    return getBuffer() + offset(Interface565Point{0, 0});
}
