//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2025 Andrew Duncan
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
#include <functional>
#include <numbers>
#include <stdexcept>

#include "image565.h"
#include "image565Process.h"

//-------------------------------------------------------------------------

using size_type = std::vector<uint32_t>::size_type;
using Point = fb16::Point565;

//=========================================================================

namespace {

//-------------------------------------------------------------------------

class AccumulateRGB565
{
public:

    void add(const fb16::RGB565& rgb) noexcept
    {
        const auto rgb8 = rgb.getRGB8();
        m_red += rgb8.red;
        m_green += rgb8.green;
        m_blue += rgb8.blue;
    }

    void subtract(const fb16::RGB565& rgb) noexcept
    {
        const auto rgb8 = rgb.getRGB8();
        m_red -= rgb8.red;
        m_green -= rgb8.green;
        m_blue -= rgb8.blue;
    }

    [[nodiscard]] fb16::RGB565 average(int count) noexcept
    {
        return fb16::RGB565{static_cast<uint8_t>(m_red / count),
                                 static_cast<uint8_t>(m_green / count),
                                 static_cast<uint8_t>(m_blue / count)};
    }

private:

    int m_red{0};
    int m_green{0};
    int m_blue{0};
};

//-------------------------------------------------------------------------

void
rowsRotate(
    const fb16::Image565& image,
    fb16::Image565& output,
    double sinAngle,
    double cosAngle,
    int jStart,
    int jEnd)
{
    const auto id = image.getDimensions();
    const auto od = output.getDimensions();

    const auto y00 = id.height() * cosAngle;

    for (int j = jStart ; j < jEnd ; ++j)
    {
        const auto b = y00 - j;
        const auto bSinAngle = b * sinAngle;
        const auto bCosAngle = b * cosAngle;

        for (int i = 0 ; i < od.width() ; ++i)
        {
            const auto x = (i * cosAngle) - bSinAngle;
            const auto y = (i * sinAngle) + bCosAngle;

            const auto x0 = static_cast<int>(floor(x));
            const auto y0 = static_cast<int>(floor(y));

            const auto x1 = static_cast<int>(ceil(x));
            const auto y1 = static_cast<int>(ceil(y));

            const auto pixel00 = image.getPixel(Point{x0, id.height() - 1 - y0});
            const auto pixel01 = image.getPixel(Point{x0, id.height() - 1 - y1});
            const auto pixel10 = image.getPixel(Point{x1, id.height() - 1 - y0});
            const auto pixel11 = image.getPixel(Point{x1, id.height() - 1 - y1});

            if (pixel00.has_value() and
                pixel01.has_value() and
                pixel10.has_value() and
                pixel11.has_value())
            {
                const auto xWeight = x - x0;
                const auto yWeight = y - y0;

                const auto aWeight = (1.0 - xWeight) * (1.0 - yWeight);
                const auto bWeight = (1.0 - xWeight) * yWeight;
                const auto cWeight = xWeight * (1.0 - yWeight);
                const auto dWeight = xWeight * yWeight;

                auto evaluate = [&](const uint8_t fb16::RGB8::* channel) -> uint8_t
                {
                    double value = fb16::RGB8(pixel00.value()).*channel * aWeight
                                 + fb16::RGB8(pixel01.value()).*channel * bWeight
                                 + fb16::RGB8(pixel10.value()).*channel * cWeight
                                 + fb16::RGB8(pixel11.value()).*channel * dWeight;

                    return static_cast<uint8_t>(std::clamp(value, 0.0, 255.0));
                };

                fb16::RGB565 rgb{evaluate(&fb16::RGB8::red),
                                      evaluate(&fb16::RGB8::green),
                                      evaluate(&fb16::RGB8::blue)};

                output.setPixel(Point{i, j}, rgb.get565());
            }
        }
    }
}

//-------------------------------------------------------------------------

void
rowsToGrey(
    const fb16::Interface565Base& input,
    fb16::Image565& output,
    int jStart,
    int jEnd)
{
    const auto id = input.getDimensions();
    auto inputi = input.getBuffer().data() + (jStart * id.width());

    for (auto j = jStart ; j < jEnd ; ++j)
    {
        for (int i = 0 ; i < id.width() ; ++i)
        {
            auto pixel = *(inputi++);
            Point p{i, j};
            output.setPixel(p,fb16::RGB565(pixel).toGrey().get565());
        }
    }
}

//-------------------------------------------------------------------------

}

//=========================================================================

void
boxBlurRows(
    const fb16::Interface565Base& input,
    fb16::Image565& rb,
    int radius,
    int jStart,
    int jEnd)
{
    auto clamp = [](int value, int end) -> int
    {
        return std::clamp(value, 0, end - 1);
    };

    const auto diameter = 2 * radius + 1;
    const auto width = input.getDimensions().width();
    auto inputi = input.getBuffer().data();
    auto rbi = rb.getBuffer().data();

    for (auto j = jStart ; j < jEnd ; ++j)
    {
        AccumulateRGB565 argb;

        for (auto k = -radius - 1 ; k < radius ; ++k)
        {
            const Point p{clamp(k, width), j};
            argb.add(fb16::RGB565(*(inputi + input.offset(p))));
        }

        for (auto i = 0 ; i < width ; ++i)
        {
            Point p{clamp(i + radius, width), j};
            argb.add(fb16::RGB565(*(inputi + input.offset(p))));

            p = Point(clamp(i - radius - 1, width), j);
            argb.subtract(fb16::RGB565(*(inputi + input.offset(p))));

            p = Point(i, j);
            *(rbi + rb.offset(p)) = argb.average(diameter).get565();
        }
    }
}

void
boxBlurColumns(
    const fb16::Image565& rb,
    fb16::Image565& output,
    int radius,
    int iStart,
    int iEnd)
{
    auto clamp = [](int value, int end) -> int
    {
        return std::clamp(value, 0, end - 1);
    };

    const auto diameter = 2 * radius + 1;
    const auto height = rb.getDimensions().height();
    const auto rbi = rb.getBuffer().data();
    auto outputi = output.getBuffer().data();

    for (auto i = iStart ; i < iEnd ; ++i)
    {
        AccumulateRGB565 argb;

        for (auto k = -radius - 1 ; k < radius ; ++k)
        {
            const Point p{i, clamp(k, height)};
            argb.add(fb16::RGB565(*(rbi + rb.offset(p))));
        }

        for (auto j = 0 ; j < height ; ++j)
        {
            Point p{i, clamp(j + radius, height)};
            argb.add(fb16::RGB565(*(rbi + rb.offset(p))));

            p = Point(i, clamp(j - radius - 1, height));
            argb.subtract(fb16::RGB565(*(rbi + rb.offset(p))));

            p = Point(i, j);
            *(outputi + output.offset(p)) = argb.average(diameter).get565();
        }
    }
}

//-------------------------------------------------------------------------

fb16::Image565
fb16::boxBlur(
    const fb16::Interface565Base& input,
    int radius)
{
    const auto d = input.getDimensions();

    fb16::Image565 rb{d};
    fb16::Image565 output{d};

    boxBlurRows(input, rb, radius, 0, d.height());
    boxBlurColumns(rb, output, radius, 0, d.width());

    return output;
}

//-------------------------------------------------------------------------

fb16::Image565
fb16::enlighten(
    const fb16::Interface565Base& input,
    double strength)
{
    auto flerp = [](double value1, double value2, double alpha)->double
    {
        return (value1 * (1.0 - alpha)) + (value2 * alpha);
    };

    auto scaled = [](uint8_t channel, double scale)->uint8_t
    {
        return static_cast<uint8_t>(std::clamp(channel * scale, 0.0, 255.0));
    };

    const auto mb = fb16::boxBlur(fb16::maxRGB(input), 12);

    fb16::Image565 output{input.getDimensions()};

    const auto strength2 = strength * strength;
    const auto minI = 1.0 / flerp(1.0, 10.0, strength2);
    const auto maxI = 1.0 / flerp(1.0, 1.111, strength2);

    auto mbi = mb.getBuffer().data();
    auto outputi = output.getBuffer().data();

    for (auto pixel : input.getBuffer())
    {
        fb16::RGB565 c{pixel};
        const auto rgb8 = c.getRGB8();
        const auto max = fb16::RGB8(*(mbi++)).red;
        const auto illumination = std::clamp(max / 255.0, minI, maxI);

        if (illumination < maxI)
        {
            const auto r = illumination / maxI;
            const auto scale = (0.4 + (r * 0.6)) / r;

            c.setRGB(scaled(rgb8.red, scale),
                     scaled(rgb8.green, scale),
                     scaled(rgb8.blue, scale));
        }

        *(outputi++) = c.get565();
    }

    return output;
}

//-------------------------------------------------------------------------

fb16::Image565
fb16::maxRGB(
    const fb16::Interface565Base& input)
{
    fb16::Image565 output{input.getDimensions()};
    auto* buffer = output.getBuffer().data();

    for (const auto pixel : input.getBuffer())
    {
       fb16::RGB8 rgb8(pixel);
       const auto grey(std::max({rgb8.red, rgb8.green, rgb8.blue}));
       *(buffer++) = fb16::RGB565::rgbTo565(grey, grey, grey);
    }

    return output;
}

//-------------------------------------------------------------------------

fb16::Image565
fb16::resizeBilinearInterpolation(
    const fb16::Interface565Base& input,
    fb16::Dimensions565 d)
{
    if ((d.width() <= 0) or (d.height() <= 0))
    {
        throw std::invalid_argument("width and height must be greater than zero");
    }

    fb16::Image565 output{d};
    resizeToBilinearInterpolation(input, output);

    return output;
}

//-------------------------------------------------------------------------

fb16::Image565
fb16::resizeLanczos3Interpolation(
    const fb16::Interface565Base& input,
    fb16::Dimensions565 d)
{
    if ((d.width() <= 0) or (d.height() <= 0))
    {
        throw std::invalid_argument("width and height must be greater than zero");
    }

    fb16::Image565 output{d};
    resizeToLanczos3Interpolation(input, output);

    return output;
}

//-------------------------------------------------------------------------

fb16::Image565
fb16::resizeNearestNeighbour(
    const fb16::Interface565Base& input,
    fb16::Dimensions565 d)
{
    if ((d.width() <= 0) or (d.height() <= 0))
    {
        throw std::invalid_argument("width and height must be greater than zero");
    }

    fb16::Image565 output{d};
    resizeToNearestNeighbour(input, output);

    return output;
}

//-------------------------------------------------------------------------

void
rowsBilinearInterpolation(
    const fb16::Interface565Base& input,
    fb16::Image565& output,
    int jStart,
    int jEnd)
{
    const auto id = input.getDimensions();
    const auto od = output.getDimensions();

    const auto xScale = (od.width() > 1)
                      ? (id.width() - 1.0f) / (od.width() - 1.0f)
                      : 0.0f;
    const auto yScale = (od.height() > 1)
                      ? (id.height() - 1.0f) / (od.height() - 1.0f)
                      : 0.0f;

    for (int j = jStart; j < jEnd; ++j)
    {
        for (int i = 0; i < od.width(); ++i)
        {
            int xLow = static_cast<int>(std::floor(xScale * i));
            int yLow = static_cast<int>(std::floor(yScale * j));
            int xHigh = static_cast<int>(std::ceil(xScale * i));
            int yHigh = static_cast<int>(std::ceil(yScale * j));

            const auto xWeight = (xScale * i) - xLow;
            const auto yWeight = (yScale * j) - yLow;

            auto a = *input.getPixelRGB8(Point{xLow, yLow});
            auto b = *input.getPixelRGB8(Point{xHigh, yLow});
            auto c = *input.getPixelRGB8(Point{xLow, yHigh});
            auto d = *input.getPixelRGB8(Point{xHigh, yHigh});

            const auto aWeight = (1.0f - xWeight) * (1.0f - yWeight);
            const auto bWeight = xWeight * (1.0f - yWeight);
            const auto cWeight = (1.0f - xWeight) * yWeight;
            const auto dWeight = xWeight * yWeight;

            auto evaluate = [&](const uint8_t fb16::RGB8::* channel) -> uint8_t
            {
                float value = a.*channel * aWeight
                            + b.*channel * bWeight
                            + c.*channel * cWeight
                            + d.*channel * dWeight;

                return static_cast<uint8_t>(std::clamp(value, 0.0f, 255.0f));
            };

            fb16::RGB565 rgb{evaluate(&fb16::RGB8::red),
                                  evaluate(&fb16::RGB8::green),
                                  evaluate(&fb16::RGB8::blue)};

            output.setPixelRGB(Point{i, j}, rgb);
        }
    }
}

//-------------------------------------------------------------------------

fb16::Image565&
fb16::resizeToBilinearInterpolation(
    const fb16::Interface565Base& input,
    fb16::Image565& output)
{
    rowsBilinearInterpolation(input, output, 0, output.getDimensions().height());
    return output;
}

//-------------------------------------------------------------------------

float
lanczosKernel(float x, int a)
{
    const auto pi = std::numbers::pi_v<float>;

    if (x == 0.0f)
    {
        return 1.0f;
    }

    if (x < -a or x > a)
    {
        return 0.0f;
    }

    return (a * std::sin(pi * x) * std::sin(pi * x / a)) /
            (pi * pi * x * x);
}

//-------------------------------------------------------------------------

void
rowsLanczos3Interpolation(
    const fb16::Interface565Base& input,
    fb16::Image565& output,
    int jStart,
    int jEnd)
{
    constexpr int a{3};

    const auto id = input.getDimensions();
    const auto od = output.getDimensions();

    const auto xScale = (od.width() > 1)
                      ? (id.width() - 1.0f) / (od.width() - 1.0f)
                      : 0.0f;
    const auto yScale = (od.height() > 1)
                      ? (id.height() - 1.0f) / (od.height() - 1.0f)
                      : 0.0f;

    for (int j = jStart; j < jEnd; ++j)
    {
        for (int i = 0; i < od.width(); ++i)
        {
            const auto xMid = i * xScale;
            const auto yMid = j * yScale;

            const auto xLow = std::max(0, static_cast<int>(std::floor(xMid)) - a + 1);
            const auto xHigh = std::min(id.width() - 1, static_cast<int>(std::floor(xMid)) + a);
            const auto yLow = std::max(0, static_cast<int>(std::floor(yMid)) - a + 1);
            const auto yHigh = std::min(id.height() - 1, static_cast<int>(std::floor(yMid)) + a);

            float weightsSum{};
            float redSum{};
            float greenSum{};
            float blueSum{};

            for (int y = yLow; y <= yHigh; ++y)
            {
                const auto dy = yMid - y;
                const auto yKernelValue = lanczosKernel(dy, a);

                for (int x = xLow; x <= xHigh; ++x)
                {
                    const auto dx = xMid - x;
                    const auto weight = lanczosKernel(dx, a) * yKernelValue;
                    weightsSum += weight;

                    auto rgb = *input.getPixelRGB(Point{x, y});
                    const auto rgb8 = rgb.getRGB8();
                    redSum += rgb8.red * weight;
                    greenSum += rgb8.green * weight;
                    blueSum += rgb8.blue * weight;
                }
            }

            const auto red = std::clamp(redSum / weightsSum, 0.0f, 255.0f);
            const auto green = std::clamp(greenSum / weightsSum, 0.0f, 255.0f);
            const auto blue = std::clamp(blueSum / weightsSum, 0.0f, 255.0f);

            fb16::RGB565 rgb{static_cast<uint8_t>(red),
                                  static_cast<uint8_t>(green),
                                  static_cast<uint8_t>(blue)};

            output.setPixelRGB(Point{i, j}, rgb);
        }
    }
}

//-------------------------------------------------------------------------

fb16::Image565&
fb16::resizeToLanczos3Interpolation(
    const fb16::Interface565Base& input,
    fb16::Image565& output)
{
    rowsLanczos3Interpolation(input, output, 0, output.getDimensions().height());
    return output;
}

//-------------------------------------------------------------------------

void
rowsNearestNeighbour(
    const fb16::Interface565Base& input,
    fb16::Image565& output,
    int jStart,
    int jEnd)
{
    const auto id = input.getDimensions();
    const auto od = output.getDimensions();

    const int a = (od.width() > id.width()) ? 0 : 1;
    const int b = (od.height() > id.height()) ? 0 : 1;

    for (int j = jStart ; j < jEnd ; ++j)
    {
        const int y = (j * (id.height() - b)) / (od.height() - b);
        for (int i = 0 ; i < od.width() ; ++i)
        {
            const int x = (i * (id.width() - a)) / (od.width() - a);
            auto pixel{input.getPixel(Point{x, y})};

            if (pixel.has_value())
            {
                output.setPixel(Point{i, j}, pixel.value());
            }
        }
    }
}

//-------------------------------------------------------------------------

fb16::Image565&
fb16::resizeToNearestNeighbour(
    const fb16::Interface565Base& input,
    fb16::Image565& output)
{
    rowsNearestNeighbour(input, output, 0, output.getDimensions().height());
    return output;
}

//-------------------------------------------------------------------------

fb16::Image565
fb16::rotate(
    const fb16::Interface565Base& input,
    uint32_t background,
    double angle)
{
    if (angle >= 360.0)
    {
        angle = fmod(angle, 360.0);
    }
    else if (angle < 0.0)
    {
        angle = 360.0 + fmod(angle, 360.0);
    }

    // rotate so angle is in the range 0 to 90

    Image565 image;

    if (angle >= 270.0)
    {
        image = rotate270(input);
        angle -= 270.0;
    }
    else if (angle >= 180.0)
    {
        image = rotate180(input);
        angle -= 180.0;
    }
    else if (angle >= 90.0)
    {
        image = rotate90(input);
        angle -= 90.0;
    }
    else
    {
        image = input;
    }

    // now angle is in the range 0 to 90
    if (std::min(angle, 90.0 - angle) < 0.01)
    {
        return image;
    }

    //---------------------------------------------------------------------
    //
    // (x0,y0) +-------+ (x1,y0)
    //         |       |
    //         |       |
    //         |       |
    // (x0,y1) +-------+ (x1,y1)
    //
    // x' =  x * cos(angle) + y * sin(angle)
    // y' = -x * sin(angle) + y * cos(angle)
    //
    // x = x' * cos(angle) - y' * sin(angle)
    // y = x' * sin(angle) + y' * cos(angle)
    //
    //---------------------------------------------------------------------

    const auto radians = angle * (std::numbers::pi_v<double> / 180.0);
    const auto cosAngle = std::cos(radians);
    const auto sinAngle = std::sin(radians);

    const auto id = image.getDimensions();

    const auto x10 = (id.width() * cosAngle) + (id.height() * sinAngle);
    const auto y00 = id.height() * cosAngle;
    const auto y11 = -(id.width() * sinAngle);

    const Dimensions565 od
    {
        static_cast<int>(std::ceil(x10)),
        static_cast<int>(std::ceil(y00 - y11 + 1.0))
    };

    Image565 output{od};
    output.clear(background);

    rowsRotate(image, output, sinAngle, cosAngle, 0, output.getDimensions().height());

    return output;
}

//-------------------------------------------------------------------------

fb16::Image565
fb16::rotate90(
    const fb16::Interface565Base& input)
{
    const auto id = input.getDimensions();
    const Dimensions565 od{ id.height(), id.width()};
    Image565 output{od};

    for (auto j = 0 ; j < id.height() ; ++j)
    {
        for (auto i = 0 ; i < id.width() ; ++i)
        {
            const auto pixel{input.getPixel(Point{i, j})};

            if (pixel.has_value())
            {
                output.setPixel(Point{id.height() - j - 1, i}, pixel.value());
            }
        }
    }

    return output;
}

//-------------------------------------------------------------------------

fb16::Image565
fb16::rotate180(
    const fb16::Interface565Base& input)
{
    const auto d = input.getDimensions();
    Image565 output{d};

    for (auto j = 0 ; j < d.height() ; ++j)
    {
        for (auto i = 0 ; i < d.width() ; ++i)
        {
            const auto pixel{input.getPixel(Point{i, j})};

            if (pixel.has_value())
            {
                output.setPixel(Point{d.width() - i - 1, d.height() - j - 1}, pixel.value());
            }
        }
    }

    return output;
}

//-------------------------------------------------------------------------

fb16::Image565
fb16::rotate270(
    const fb16::Interface565Base& input)
{
    const auto id = input.getDimensions();
    const Dimensions565 od{ id.height(), id.width()};
    Image565 output{od};

    for (auto j = 0 ; j < id.height() ; ++j)
    {
        for (auto i = 0 ; i < id.width() ; ++i)
        {
            const auto pixel{input.getPixel(Point{i, j})};

            if (pixel.has_value())
            {
                output.setPixel(Point{j, id.width() - i - 1}, pixel.value());
            }
        }
    }

    return output;
}

//-------------------------------------------------------------------------

fb16::Image565
fb16::scaleUp(
    const fb16::Interface565Base& input,
    uint8_t scale)
{
    const auto id = input.getDimensions();
    const Dimensions565 od{id.width() * scale, id.height() * scale};
    auto inputi = input.getBuffer().data();
    fb16::Image565 output{od};

    for (int j = 0 ; j < id.height() ; ++j)
    {
        for (int i = 0 ; i < id.width() ; ++i)
        {
            auto pixel = *(inputi++);
            for (int b = 0 ; b < scale ; ++b)
            {
                for (int a = 0 ; a < scale ; ++a)
                {
                    Point p{(i * scale) + a, (j * scale) + b};
                    output.setPixel(p, pixel);
                }
            }
        }
    }

    return output;
}

//-------------------------------------------------------------------------

fb16::Image565
fb16::toGrey(
    const Interface565Base& input)
{
    const auto id = input.getDimensions();
    Image565 output{id};

    rowsToGrey(input, output, 0, id.height());

    return output;
}

