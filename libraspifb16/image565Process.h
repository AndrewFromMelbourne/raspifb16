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

#pragma once

//-------------------------------------------------------------------------

#include "image565.h"
#include "interface565Base.h"
#include "rgb565.h"
#include "point.h"

//-------------------------------------------------------------------------

namespace fb16
{

//-------------------------------------------------------------------------

[[nodiscard]] Image565
boxBlur(
    const Interface565Base& input,
    int radius);

[[nodiscard]] Image565
enlighten(
    const Interface565Base& input,
    double strength);

[[nodiscard]] Image565
maxRGB(
    const Interface565Base& input);

[[nodiscard]] Image565
resizeBilinearInterpolation(
    const Interface565Base& input,
    fb16::Dimensions565 d);

[[nodiscard]] Image565
resizeLanczos3Interpolation(
    const Interface565Base& input,
    fb16::Dimensions565 d);

[[nodiscard]] Image565
resizeNearestNeighbour(
    const Interface565Base& input,
    fb16::Dimensions565 d);

Image565&
resizeToBilinearInterpolation(
    const Interface565Base& input,
    Image565& output);

Image565&
resizeToLanczos3Interpolation(
    const Interface565Base& input,
    Image565& output);

Image565&
resizeToNearestNeighbour(
    const Interface565Base& input,
    Image565& output);

[[nodiscard]] Image565
rotate(
    const Interface565Base& input,
    uint32_t background,
    double angle);

[[nodiscard]] inline Image565
rotate(
    const Interface565Base& input,
    const RGB565& background,
    double angle)
{
    return rotate(input, background.get565(), angle);
}

[[nodiscard]] inline Image565
rotate(
    const Interface565Base& input,
    double angle)
{
    return rotate(input, 0, angle);
}

[[nodiscard]] Image565
rotate90(
    const Interface565Base& input);

[[nodiscard]] Image565
rotate180(
    const Interface565Base& input);

[[nodiscard]] Image565
rotate270(
    const Interface565Base& input);

[[nodiscard]] Image565
scaleUp(
    const Interface565Base& input,
    uint8_t scale);

[[nodiscard]] Image565
toGrey(
    const Interface565Base& input);

//-------------------------------------------------------------------------

} // namespace fb16

