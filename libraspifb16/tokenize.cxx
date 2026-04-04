//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2026 Andrew Duncan
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

#include "tokenize.h"

//-------------------------------------------------------------------------

std::vector<std::string_view>
fb16::tokenize(
    const std::string_view str,
    std::function<bool(char)> isSeparator)
{
    std::vector<std::string_view> result{};

    auto start = std::find_if_not(begin(str), end(str), isSeparator);
    do
    {
        auto end = std::find_if(start, std::end(str), isSeparator);
        result.emplace_back(start, end);
        start = std::find_if_not(end, std::end(str), isSeparator);
    }
    while (start != cend(str));

    return result;
}

//-------------------------------------------------------------------------

std::vector<std::string_view>
fb16::split(
    const std::string_view str)
{
    auto isSpace = [](char c) { return std::isspace(static_cast<unsigned char>(c)); };
    return tokenize(str, isSpace);
}

