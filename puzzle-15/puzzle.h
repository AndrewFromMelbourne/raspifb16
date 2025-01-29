//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2022 Andrew Duncan
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

#include <array>
#include <cstdint>

#include "image565.h"
#include "interface565.h"
#include "joystick.h"

#include "images.h"

//-------------------------------------------------------------------------

class Puzzle
{
public:

    struct Location
    {
        int x;
        int y;
    };

    Puzzle();

    void init();
    bool update(raspifb16::Joystick& js);
    void draw(raspifb16::Interface565& fb);

private:

    [[nodiscard]] int getInversionCount() const;
    [[nodiscard]] bool isSolvable() const;
    [[nodiscard]] bool isSolved() const;

    static constexpr int c_puzzleWidth{4};
    static constexpr int c_puzzleHeight{4};
    static constexpr int c_boardSize{c_puzzleWidth * c_puzzleHeight};

    std::array<uint8_t, c_boardSize> m_board;
    std::array<raspifb16::Image565, c_tileCount> m_tileBuffers;
    Location m_blankLocation;
};

