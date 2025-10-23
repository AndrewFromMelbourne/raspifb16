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
using Point = raspifb16::Interface565Point;

//=========================================================================

namespace {

//-------------------------------------------------------------------------

class AccumulateRGB565
{
public:

    void add(const raspifb16::RGB565& rgb) noexcept
    {
        m_red += rgb.getRed();
        m_green += rgb.getGreen();
        m_blue += rgb.getBlue();
    }

    void subtract(const raspifb16::RGB565& rgb) noexcept
    {
        m_red -= rgb.getRed();
        m_green -= rgb.getGreen();
        m_blue -= rgb.getBlue();
    }

    [[nodiscard]] raspifb16::RGB565 average(int count) noexcept
    {
        return raspifb16::RGB565{static_cast<uint8_t>(m_red / count),
                             static_cast<uint8_t>(m_green / count),
                             static_cast<uint8_t>(m_blue / count)};
    }

private:

    int m_red{0};
    int m_green{0};
    int m_blue{0};
};

//-------------------------------------------------------------------------

}

//=========================================================================

void
boxBlurRows(
    const raspifb16::Interface565& input,
    raspifb16::Image565& rb,
    int radius,
    int jStart,
    int jEnd)
{
    auto clamp = [](int value, int end) -> int
    {
        return std::clamp(value, 0, end - 1);
    };

    const auto diameter = 2 * radius + 1;
    const auto width = input.getWidth();
    auto inputi = input.getBuffer().data();
    auto rbi = rb.getBuffer().data();

    for (auto j = jStart ; j < jEnd ; ++j)
    {
        AccumulateRGB565 argb;

        for (auto k = -radius - 1 ; k < radius ; ++k)
        {
            const Point p{clamp(k, width), j};
            argb.add(raspifb16::RGB565(*(inputi + input.offset(p))));
        }

        for (auto i = 0 ; i < width ; ++i)
        {
            Point p{clamp(i + radius, width), j};
            argb.add(raspifb16::RGB565(*(inputi + input.offset(p))));

            p = Point(clamp(i - radius - 1, width), j);
            argb.subtract(raspifb16::RGB565(*(inputi + input.offset(p))));

            p = Point(i, j);
            *(rbi + rb.offset(p)) = argb.average(diameter).get565();
        }
    }
}

void
boxBlurColumns(
    const raspifb16::Image565& rb,
    raspifb16::Image565& output,
    int radius,
    int iStart,
    int iEnd)
{
    auto clamp = [](int value, int end) -> int
    {
        return std::clamp(value, 0, end - 1);
    };

    const auto diameter = 2 * radius + 1;
    const auto height = rb.getHeight();
    const auto rbi = rb.getBuffer().data();
    auto outputi = output.getBuffer().data();

    for (auto i = iStart ; i < iEnd ; ++i)
    {
        AccumulateRGB565 argb;

        for (auto k = -radius - 1 ; k < radius ; ++k)
        {
            const Point p{i, clamp(k, height)};
            argb.add(raspifb16::RGB565(*(rbi + rb.offset(p))));
        }

        for (auto j = 0 ; j < height ; ++j)
        {
            Point p{i, clamp(j + radius, height)};
            argb.add(raspifb16::RGB565(*(rbi + rb.offset(p))));

            p = Point(i, clamp(j - radius - 1, height));
            argb.subtract(raspifb16::RGB565(*(rbi + rb.offset(p))));

            p = Point(i, j);
            *(outputi + output.offset(p)) = argb.average(diameter).get565();
        }
    }
}

//-------------------------------------------------------------------------

raspifb16::Image565
raspifb16::boxBlur(
    const raspifb16::Interface565& input,
    int radius)
{
    const auto width = input.getWidth();
    const auto height = input.getHeight();

    raspifb16::Image565 rb{width, height};
    raspifb16::Image565 output{width, height};

    boxBlurRows(input, rb, radius, 0, height);
    boxBlurColumns(rb, output, radius, 0, width);

    return output;
}

//-------------------------------------------------------------------------

raspifb16::Image565
raspifb16::enlighten(
    const raspifb16::Interface565& input,
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

    const auto mb = raspifb16::boxBlur(raspifb16::maxRGB(input), 12);

    raspifb16::Image565 output{input.getWidth(), input.getHeight()};

    const auto strength2 = strength * strength;
    const auto minI = 1.0 / flerp(1.0, 10.0, strength2);
    const auto maxI = 1.0 / flerp(1.0, 1.111, strength2);

    auto mbi = mb.getBuffer().data();
    auto outputi = output.getBuffer().data();

    for (auto pixel : input.getBuffer())
    {
        auto c = raspifb16::RGB565(pixel);
        const auto max = raspifb16::RGB565(*(mbi++)).getRed();
        const auto illumination = std::clamp(max / 255.0, minI, maxI);

        if (illumination < maxI)
        {
            const auto r = illumination / maxI;
            const auto scale = (0.4 + (r * 0.6)) / r;

            c.setRGB(scaled(c.getRed(), scale),
                     scaled(c.getGreen(), scale),
                     scaled(c.getBlue(), scale));
        }

        *(outputi++) = c.get565();
    }

    return output;
}

//-------------------------------------------------------------------------

raspifb16::Image565
raspifb16::maxRGB(
    const raspifb16::Interface565& input)
{
    raspifb16::Image565 output{input.getWidth(), input.getHeight()};
    auto* buffer = output.getBuffer().data();

    for (const auto pixel : input.getBuffer())
    {
        raspifb16::RGB565 rgb(pixel);
        rgb.setGrey(std::max({rgb.getRed(), rgb.getGreen(), rgb.getBlue()}));
        *(buffer++) = rgb.get565();
    }

    return output;
}

//-------------------------------------------------------------------------

raspifb16::Image565
raspifb16::resizeBilinearInterpolation(
    const raspifb16::Interface565& input,
    int width,
    int height)
{
    if ((width <= 0) or (height <= 0))
    {
        throw std::invalid_argument("width and height must be greater than zero");
    }

    raspifb16::Image565 output{width, height};
    resizeToBilinearInterpolation(input, output);

    return output;
}

//-------------------------------------------------------------------------

raspifb16::Image565
raspifb16::resizeLanczos3Interpolation(
    const raspifb16::Interface565& input,
    int width,
    int height)
{
    if ((width <= 0) or (height <= 0))
    {
        throw std::invalid_argument("width and height must be greater than zero");
    }

    raspifb16::Image565 output{width, height};
    resizeToLanczos3Interpolation(input, output);

    return output;
}

//-------------------------------------------------------------------------

raspifb16::Image565
raspifb16::resizeNearestNeighbour(
    const raspifb16::Interface565& input,
    int width,
    int height)
{
    if ((width <= 0) or (height <= 0))
    {
        throw std::invalid_argument("width and height must be greater than zero");
    }

    raspifb16::Image565 output{width, height};
    resizeToNearestNeighbour(input, output);

    return output;
}

//-------------------------------------------------------------------------

void
rowsBilinearInterpolation(
    const raspifb16::Interface565& input,
    raspifb16::Image565& output,
    int jStart,
    int jEnd)
{
    const auto xScale = (output.getWidth() > 1)
                      ? (input.getWidth() - 1.0f) / (output.getWidth() - 1.0f)
                      : 0.0f;
    const auto yScale = (output.getHeight() > 1)
                      ? (input.getHeight() - 1.0f) / (output.getHeight() - 1.0f)
                      : 0.0f;

    for (int j = jStart; j < jEnd; ++j)
    {
        for (int i = 0; i < output.getWidth(); ++i)
        {
            int xLow = static_cast<int>(std::floor(xScale * i));
            int yLow = static_cast<int>(std::floor(yScale * j));
            int xHigh = static_cast<int>(std::ceil(xScale * i));
            int yHigh = static_cast<int>(std::ceil(yScale * j));

            const auto xWeight = (xScale * i) - xLow;
            const auto yWeight = (yScale * j) - yLow;

            auto a = *input.getPixelRGB(Point{xLow, yLow});
            auto b = *input.getPixelRGB(Point{xHigh, yLow});
            auto c = *input.getPixelRGB(Point{xLow, yHigh});
            auto d = *input.getPixelRGB(Point{xHigh, yHigh});

            const auto aWeight = (1.0f - xWeight) * (1.0f - yWeight);
            const auto bWeight = xWeight * (1.0f - yWeight);
            const auto cWeight = (1.0f - xWeight) * yWeight;
            const auto dWeight = xWeight * yWeight;

            typedef  uint8_t (raspifb16::RGB565::*RGB565MemFn)() const;

            auto evaluate = [&](RGB565MemFn get) -> uint8_t
            {
                float value = std::invoke(get, a) * aWeight +
                                std::invoke(get, b) * bWeight +
                                std::invoke(get, c) * cWeight +
                                std::invoke(get, d) * dWeight;

                return static_cast<uint8_t>(std::clamp(value, 0.0f, 255.0f));
            };

            raspifb16::RGB565 rgb{evaluate(&raspifb16::RGB565::getRed),
                                evaluate(&raspifb16::RGB565::getGreen),
                                evaluate(&raspifb16::RGB565::getBlue)};

            output.setPixelRGB(Point{i, j}, rgb);
        }
    }
}

//-------------------------------------------------------------------------

raspifb16::Image565&
raspifb16::resizeToBilinearInterpolation(
    const raspifb16::Interface565& input,
    raspifb16::Image565& output)
{
    rowsBilinearInterpolation(input, output, 0, output.getHeight());
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
    const raspifb16::Interface565& input,
    raspifb16::Image565& output,
    int jStart,
    int jEnd)
{
    constexpr int a{3};
    const auto xScale = (output.getWidth() > 1)
                      ? (input.getWidth() - 1.0f) / (output.getWidth() - 1.0f)
                      : 0.0f;
    const auto yScale = (output.getHeight() > 1)
                      ? (input.getHeight() - 1.0f) / (output.getHeight() - 1.0f)
                      : 0.0f;

    for (int j = jStart; j < jEnd; ++j)
    {
        for (int i = 0; i < output.getWidth(); ++i)
        {
            const auto xMid = i * xScale;
            const auto yMid = j * yScale;

            const auto xLow = std::max(0, static_cast<int>(std::floor(xMid)) - a + 1);
            const auto xHigh = std::min(input.getWidth() - 1, static_cast<int>(std::floor(xMid)) + a);
            const auto yLow = std::max(0, static_cast<int>(std::floor(yMid)) - a + 1);
            const auto yHigh = std::min(input.getHeight() - 1, static_cast<int>(std::floor(yMid)) + a);

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
                    redSum += rgb.getRed() * weight;
                    greenSum += rgb.getGreen() * weight;
                    blueSum += rgb.getBlue() * weight;
                }
            }

            const auto red = std::clamp(redSum / weightsSum, 0.0f, 255.0f);
            const auto green = std::clamp(greenSum / weightsSum, 0.0f, 255.0f);
            const auto blue = std::clamp(blueSum / weightsSum, 0.0f, 255.0f);

            raspifb16::RGB565 rgb{static_cast<uint8_t>(red),
                                static_cast<uint8_t>(green),
                                static_cast<uint8_t>(blue)};

            output.setPixelRGB(Point{i, j}, rgb);
        }
    }
}

//-------------------------------------------------------------------------

raspifb16::Image565&
raspifb16::resizeToLanczos3Interpolation(
    const raspifb16::Interface565& input,
    raspifb16::Image565& output)
{
    rowsLanczos3Interpolation(input, output, 0, output.getHeight());
    return output;
}

//-------------------------------------------------------------------------

void
rowsNearestNeighbour(
    const raspifb16::Interface565& input,
    raspifb16::Image565& output,
    int jStart,
    int jEnd)
{
    const auto inputWidth = input.getWidth();
    const auto inputHeight = input.getHeight();
    const auto outputWidth = output.getWidth();
    const auto outputHeight = output.getHeight();

    const int a = (outputWidth > inputWidth) ? 0 : 1;
    const int b = (output.getHeight() > inputHeight) ? 0 : 1;

    for (int j = jStart ; j < jEnd ; ++j)
    {
        const int y = (j * (inputHeight - b)) / (outputHeight - b);
        for (int i = 0 ; i < outputWidth ; ++i)
        {
            const int x = (i * (inputWidth - a)) / (outputWidth - a);
            auto pixel{input.getPixel(Point{x, y})};

            if (pixel.has_value())
            {
                output.setPixel(Point{i, j}, pixel.value());
            }
        }
    }
}

//-------------------------------------------------------------------------

raspifb16::Image565&
raspifb16::resizeToNearestNeighbour(
    const raspifb16::Interface565& input,
    raspifb16::Image565& output)
{
    rowsNearestNeighbour(input, output, 0, output.getHeight());
    return output;
}

//-------------------------------------------------------------------------

raspifb16::Image565
raspifb16::scaleUp(
    const raspifb16::Interface565& input,
    uint8_t scale)
{
    const auto width = input.getWidth();
    const auto height = input.getHeight();
    auto inputi = input.getBuffer().data();
    raspifb16::Image565 output{width * scale, height * scale};

    for (int j = 0 ; j < height ; ++j)
    {
        for (int i = 0 ; i < width ; ++i)
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

