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

#include <algorithm>
#include <random>

#include "images.h"
#include "puzzle.h"

//-------------------------------------------------------------------------

using namespace raspifb16;

//-------------------------------------------------------------------------

Puzzle::Puzzle()
:
    m_board
    {
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C,
        0x0D, 0x0E, 0x0F, 0x00
    },
    m_tileBuffers(
        {
            { c_tileWidth, c_tileHeight, c_piece0 },
            { c_tileWidth, c_tileHeight, c_piece1 },
            { c_tileWidth, c_tileHeight, c_piece2 },
            { c_tileWidth, c_tileHeight, c_piece3 },
            { c_tileWidth, c_tileHeight, c_piece4 },
            { c_tileWidth, c_tileHeight, c_piece5 },
            { c_tileWidth, c_tileHeight, c_piece6 },
            { c_tileWidth, c_tileHeight, c_piece7 },
            { c_tileWidth, c_tileHeight, c_piece8 },
            { c_tileWidth, c_tileHeight, c_piece9 },
            { c_tileWidth, c_tileHeight, c_piece10 },
            { c_tileWidth, c_tileHeight, c_piece11 },
            { c_tileWidth, c_tileHeight, c_piece12 },
            { c_tileWidth, c_tileHeight, c_piece13 },
            { c_tileWidth, c_tileHeight, c_piece14 },
            { c_tileWidth, c_tileHeight, c_piece15 },
        }),
    m_tileSolved{ c_tileWidth, c_tileHeight, c_smiley },
    m_blankLocation{ 3, 3 }
{
}

//-------------------------------------------------------------------------

int
Puzzle::getInversionCount() const
{
    int inversions{};

    for (auto i = 0 ; i < c_boardSize - 1 ; ++i)
    {
        for (auto j = i + 1 ; j < c_boardSize ; ++j)
        {
            if (m_board[i] and m_board[j] and (m_board[i] > m_board[j]))
            {
                ++inversions;
            }
        }
    }

    return inversions;
}

//-------------------------------------------------------------------------

bool
Puzzle::isSolvable() const
{
    const auto inversions = getInversionCount();
    bool solvable = inversions & 1;

    if (m_blankLocation.y & 1)
    {
        solvable = not solvable;
    }

    return solvable;
}

//-------------------------------------------------------------------------

bool
Puzzle::isSolved() const
{
    bool solved = true;

    for (auto i = 0 ; i < c_boardSize - 1 ; ++i)
    {
        if (m_board[i] != (i + 1))
        {
            solved = false;
        }
    }

    return solved;
}

//-------------------------------------------------------------------------

void
Puzzle::init()
{
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());

    do
    {
        std::ranges::shuffle(m_board, generator);

        for (auto i = 0 ; i < c_boardSize ; ++i)
        {
            if (m_board[i] == 0)
            {
                m_blankLocation.x = i % c_puzzleWidth;
                m_blankLocation.y = i / c_puzzleWidth;
                break;
            }
        }
    }
    while (not isSolvable() or isSolved());
}

//-------------------------------------------------------------------------

bool
Puzzle::update(Joystick& js)
{
    if (js.buttonPressed(Joystick::BUTTON_A))
    {
        init();
        return true;
    }

    const auto value = js.getAxes(0);

    if (not value.x and not value.y)
    {
        return false;
    }

    const auto dx = (value.x) ? (value.x / std::abs(value.x)) : 0;
    const auto dy = (value.y) ? (value.y / std::abs(value.y)) : 0;

    const Location newLocation = {.x = m_blankLocation.x - dx,
                                  .y = m_blankLocation.y - dy};

    if ((newLocation.x >= 0) and
        (newLocation.x < c_puzzleWidth) and
        (newLocation.y >= 0) and
        (newLocation.y < c_puzzleHeight))
    {
        const auto indexNew = newLocation.x + (newLocation.y * c_puzzleWidth);
        const auto indexBlank = m_blankLocation.x + (m_blankLocation.y * c_puzzleWidth);
        std::swap(m_board[indexNew], m_board[indexBlank]);

        m_blankLocation = newLocation;
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------

void
Puzzle::draw(Interface565& fb)
{
    constexpr int width = c_puzzleWidth * c_tileWidth;
    constexpr int height = c_puzzleHeight * c_tileHeight;
    const int xOffset = (fb.getWidth() - width) / 2;
    const int yOffset = (fb.getHeight() - height) / 2;

    for (int j = 0 ; j < c_puzzleHeight ; ++j)
    {
        for (int i = 0 ; i < c_puzzleWidth ; ++i)
        {
            const Interface565Point p{ xOffset + (i * c_tileWidth),
                                       yOffset + (j * c_tileHeight) };
            const int tile = m_board[i + (j * c_puzzleWidth)];

            if ((tile == 0) and isSolved())
            {
                fb.putImage(p, m_tileSolved);
            }
            else
            {
                fb.putImage(p, m_tileBuffers[tile]);
            }
        }
    }
}

