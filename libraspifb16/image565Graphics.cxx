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

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <span>

#include "interface565.h"
#include "image565Graphics.h"
#include "point.h"

//=========================================================================

namespace
{

//-------------------------------------------------------------------------

void
trim(
    const raspifb16::Interface565& iface,
    raspifb16::Interface565Point& p1,
    raspifb16::Interface565Point& p2)
{
    if ((p1.x() == p2.x()) or (p1.y() == p2.y()))
    {
        return;
    }

    const auto minX = std::min(p1.x(), p2.x());
    const auto maxX = std::max(p1.x(), p2.x());

    const auto minY = std::min(p1.y(), p2.y());
    const auto maxY = std::max(p1.y(), p2.y());

    if ((minX > 0) and
        (maxX < iface.getWidth()) and
        (minY > 0) and
        (maxY < iface.getHeight()))
    {
        return;
    }

    if (p1.y() > p2.y())
    {
        std::swap(p1, p2);
    }

    auto rise = p2.y() - p1.y();
    auto run = p2.x() - p1.x();
    auto c = p1.y() - (rise * p1.x()) / run;

    if (p1.y() < 0)
    {
        p1.setY(0);
        p1.setX((p1.y() - c) * run / rise);
    }

    if (p2.y() >= iface.getHeight())
    {
        p2.setY(iface.getHeight() - 1);
        p2.setX((p2.y() - c) * run / rise);
    }

    if (p1.x() > p2.x())
    {
        std::swap(p1, p2);

    }

    rise = p2.y() - p1.y();
    run = p2.x() - p1.x();
    c = p1.y() - (rise * p1.x()) / run;

    if (p1.x() < 0)
    {
        p1.setX(0);
        p1.setY((rise * p1.x()) / run + c);
    }

    if (p2.x() >= iface.getWidth())
    {
        p2.setX(iface.getWidth() - 1);
        p2.setY((rise * p2.x()) / run + c);
    }
}

//-------------------------------------------------------------------------

} // namespace

//=========================================================================

namespace raspifb16
{

//-------------------------------------------------------------------------

void
box(
    Interface565& iface,
    Interface565Point p1,
    Interface565Point p2,
    uint16_t rgb)
{
    trim(iface, p1, p2);
    verticalLine(iface, p1.x(), p1.y(), p2.y(), rgb);
    horizontalLine(iface, p1.x(), p2.x(), p1.y(), rgb);
    verticalLine(iface, p2.x(), p1.y(), p2.y(), rgb);
    horizontalLine(iface, p1.x(), p2.x(), p2.y(), rgb);
}

//-------------------------------------------------------------------------

void
boxFilled(
    Interface565& iface,
    Interface565Point p1,
    Interface565Point p2,
    uint16_t rgb)
{
    trim(iface, p1, p2);

    if (p1.y() > p2.y())
    {
        std::swap(p1, p2);
    }

    for (auto y = p1.y(); y <= p2.y(); ++y)
    {
         horizontalLine(iface, p1.x(), p2.x(), y, rgb);
    }
}

//-------------------------------------------------------------------------

void
boxFilled(
    Interface565& iface,
    Interface565Point p1,
    Interface565Point p2,
    const RGB565& rgb,
    uint8_t alpha)
{
    trim(iface, p1, p2);

    Interface565Point pa{std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y())};
    Interface565Point pb{std::max(p1.x(), p2.x()), std::max(p1.y(), p2.y())};

    for (auto j = pa.y() ; j <= pb.y() ; ++j)
    {
        for (auto i = pa.x() ; i <= pb.x() ; ++i)
        {
            const Point p{i, j};
            auto background = iface.getPixelRGB(p);

            if (background.has_value())
            {
                iface.setPixelRGB(p, rgb.blend(alpha, *background));
            }
        }
    }
}

//=========================================================================

void
line(
    Interface565& iface,
    Interface565Point p1,
    Interface565Point p2,
    uint16_t rgb)
{
    trim(iface, p1, p2);

    const auto minX = std::min(p1.x(), p2.x());
    const auto maxX = std::max(p1.x(), p2.x());

    if ((maxX < 0) or (minX >= iface.getWidth()))
    {
        return;
    }

    const auto minY = std::min(p1.y(), p2.y());
    const auto maxY = std::max(p1.y(), p2.y());

    if ((maxY < 0) or (minY >= iface.getHeight()))
    {
        return;
    }

    if (p1.y() == p2.y())
    {
        horizontalLine(iface, p1.x(), p2.x(), p1.y(), rgb);
        return;
    }

    if (p1.x() == p2.x())
    {
        verticalLine(iface, p1.x(), p1.y(), p2.y(), rgb);
        return;
    }

    const auto dx = std::abs(p2.x() - p1.x());
    const auto dy = std::abs(p2.y() - p1.y());

    if (dx > dy)
    {
        if (p1.x() > p2.x())
        {
            std::swap(p1, p2);
        }

        const auto sign_y = (p1.y() <= p2.y()) ? 1 : -1;
        auto y = p1.y();
        auto d = 2 * dy - dx;
        const auto incrE = 2 * dy;
        const auto incrNE = 2 * (dy - dx);

        if (d > 0)
        {
            iface.setPixel(p1, rgb);
        }

        for (auto x = p1.x(); x != p2.x(); ++x)
        {
            if (d <= 0)
            {
                d += incrE;
            }
            else
            {
                d += incrNE;
                y += sign_y;
            }

            iface.setPixel(Point(x, y), rgb);
        }
    }
    else
    {
        if (p1.y() > p2.y())
        {
            std::swap(p1, p2);
        }

        const auto sign_x = (p1.x() <= p2.x()) ? 1 : -1;
        auto x = p1.x();
        auto d = 2 * dx - dy;
        const auto incrN = 2 * dx;
        const auto incrNE = 2 * (dx - dy);

        if (d > 0)
        {
            iface.setPixel(p1, rgb);
        }

        for (auto y = p1.y(); y != p2.y(); ++y)
        {
            if (d <= 0)
            {
                d += incrN;
            }
            else
            {
                d += incrNE;
                x += sign_x;
            }

            iface.setPixel(Point(x, y), rgb);
        }
    }
}

//-------------------------------------------------------------------------

void
horizontalLine(
    Interface565& iface,
    int x1,
    int x2,
    int y,
    uint16_t rgb)
{
    if ((y < 0) or (y >= iface.getHeight()))
    {
        return;
    }

    if (x1 > x2)
    {
        std::swap(x1, x2);
    }

    x1 = std::max(x1, 0);
    x2 = std::min(x2, iface.getWidth() - 1);

    if ((x1 >= iface.getWidth()) or (x2 < 0))
    {
        return;
    }

    std::span<uint16_t> row = iface.getRow(y).subspan(x1, x2 - x1 + 1);
    std::fill(row.begin(), row.end(), rgb);
}

//-------------------------------------------------------------------------

void
verticalLine(
    Interface565& iface,
    int x,
    int y1,
    int y2,
    uint16_t rgb)
{
    if (x < 0 or x >= iface.getWidth())
    {
        return;
    }

    if (y1 > y2)
    {
        std::swap(y1, y2);
    }

    y1 = std::max(y1, 0);
    y2 = std::min(y2, iface.getHeight() - 1);

    if ((y1 >= iface.getHeight()) or (y2 < 0))
    {
        return;
    }

    for (auto y = y1; y <= y2; ++y)
    {
        iface.setPixel(Point(x, y), rgb);
    }
}

//=========================================================================

void
circleLines(
    Interface565& iface,
    int x,
    int y,
    int i,
    int j,
    uint16_t rgb)
{
    horizontalLine(iface, x + i, x - i, y + j, rgb);
    horizontalLine(iface, x + i, x - i, y - j, rgb);
}

//-------------------------------------------------------------------------

void
circlePoints(
    Interface565& iface,
    int x,
    int y,
    int i,
    int j,
    uint16_t rgb)
{
    iface.setPixel(Point(x + i, y + j), rgb);
    iface.setPixel(Point(x - i, y + j), rgb);
    iface.setPixel(Point(x + i, y - j), rgb);
    iface.setPixel(Point(x - i, y - j), rgb);

    if (i != j)
    {
        iface.setPixel(Point(x + j, y + i), rgb);
        iface.setPixel(Point(x + j, y - i), rgb);
        iface.setPixel(Point(x - j, y + i), rgb);
        iface.setPixel(Point(x - j, y - i), rgb);
    }
}

//=========================================================================

void
circle(
    Interface565& iface,
    Interface565Point p,
    int r,
    uint16_t rgb)
{
    int j = r;
    int d = 1 - r;
    int deltaE = 3;
    int deltaSE = -2 * r + 5;

    for (int i = 0 ;  i <= j ; ++i)
    {
        circlePoints(iface, p.x(), p.y(), i, j, rgb);

        deltaE += 2;
        if (d < 0)
        {
            d += deltaE;
            deltaSE += 2;
        }
        else
        {
            d += deltaSE;
            deltaSE += 4;
            --j;
        }
    }
}

//-------------------------------------------------------------------------

void
circleFilled(
    Interface565& iface,
    Interface565Point p,
    int r,
    uint16_t rgb)
{
    int j = r;
    int d = 1 - r;

    for (int i = 0 ; i <= j ; ++i)
    {
        if (d < 0)
        {
            d += (2 * i) + 3;
        }
        else
        {
            circleLines(iface, p.x(), p.y(), i - 1, j, rgb);

            d += (2 * (i - j)) + 5;
            --j;
        }
    }

    int i = j;

    while (j > 0)
    {
        circleLines(iface, p.x(), p.y(), i, j, rgb);

        if (d < 0)
        {
            d += (2 * (i - j)) + 5;
            ++i;
        }
        else
        {
            d += 3 - (2 * j);
        }
        --j;
    }

    circleLines(iface, p.x(), p.y(), r - 1, 0, rgb);
}

//=========================================================================

void
polygon(
    Interface565& iface,
    std::span<const Interface565Point> vertices,
    uint16_t rgb)
{
    if (vertices.size() == 0)
    {
        return;
    }

    if (vertices.size() == 1)
    {
        iface.setPixel(vertices[0], rgb);
        return;
    }

    for (std::size_t i = 0; i < vertices.size(); ++i)
    {
        const auto& p1 = vertices[i];
        const auto& p2 = vertices[(i + 1) % vertices.size()];

        line(iface, p1, p2, rgb);
    }
}

//-------------------------------------------------------------------------

void
polygonFilled(
    Interface565& iface,
    std::span<const Interface565Point> vertices,
    uint16_t rgb)
{
    if (vertices.size() == 0)
    {
        return;
    }

    if (vertices.size() == 1)
    {
        iface.setPixel(vertices[0], rgb);
        return;
    }

    auto xIntersection = [](const Interface565Point& p1,
                            const Interface565Point& p2,
                            int y) -> std::optional<int>
    {
        const auto minY = std::min(p1.y(), p2.y());
        const auto maxY = std::max(p1.y(), p2.y());

        if (y < minY or y >= maxY)
        {
            return std::nullopt;
        }

        const auto xChange = p2.x() - p1.x();
        const auto yChange = p2.y() - p1.y();

        return p1.x() + (y - p1.y()) * xChange / yChange;
    };

    auto minY = vertices[0].y();
    auto maxY = vertices[0].y();

    for (const auto& p : vertices)
    {
        minY = std::min(minY, p.y());
        maxY = std::max(maxY, p.y());
    }

    for (int y = minY; y <= maxY; ++y)
    {
        std::vector<int> intersects;

        for (auto i = 0U; i < vertices.size(); ++i)
        {
            const auto p1 = vertices[i];
            const auto p2 = vertices[(i + 1) % vertices.size()];

            const auto x = xIntersection(p1, p2, y);
            if (x.has_value())
            {
                intersects.push_back(x.value());
            }
        }

        std::ranges::sort(intersects);

        for (std::size_t i = 0; i + 1 < intersects.size(); i += 2)
        {
            const auto x1 = intersects[i];
            const auto x2 = intersects[i + 1];

            if (x2 >= 0 and x1 < iface.getWidth())
            {
                horizontalLine(iface, x1, x2, y, rgb);
            }
        }
    }
}

//-------------------------------------------------------------------------

void
polyline(
    Interface565& iface,
    std::span<const Interface565Point> vertices,
    uint16_t rgb)
{
    if (vertices.size() == 0)
    {
        return;
    }

    if (vertices.size() == 1)
    {
        iface.setPixel(vertices[0], rgb);
        return;
    }

    for (std::size_t i = 0; i < vertices.size() - 1; ++i)
    {
        const auto& p1 = vertices[i];
        const auto& p2 = vertices[i + 1];

        line(iface, p1, p2, rgb);
    }
}

//=========================================================================

} // namespace raspifb16
