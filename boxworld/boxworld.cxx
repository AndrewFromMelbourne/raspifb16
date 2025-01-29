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

#include "image565Font8x8.h"

#include "boxworld.h"
#include "images.h"

//-------------------------------------------------------------------------

using namespace raspifb16;

//-------------------------------------------------------------------------

Boxworld::Boxworld()
:
    m_level{0},
    m_levelSolved{false},
    m_canUndo{false},
    m_player{ 0, 0 },
    m_board(),
    m_boardPrevious(),
    m_levels(),
    m_tileBuffers(
        {
            { c_tileWidth, c_tileHeight, c_emptyImage },
            { c_tileWidth, c_tileHeight, c_passageImage },
            { c_tileWidth, c_tileHeight, c_boxImage },
            { c_tileWidth, c_tileHeight, c_playerImage, 2 },
            { c_tileWidth, c_tileHeight, c_wallImage },
            { c_tileWidth, c_tileHeight, c_passageWithTargetImage },
            { c_tileWidth, c_tileHeight, c_boxOnTargetImage },
            { c_tileWidth, c_tileHeight, c_playerOnTargetImage, 2 }
        }),
    m_topTextImage{ 240, 8 },
    m_bottomTextImage{ 320, 16 },
    m_textRGB(255, 255, 255),
    m_boldRGB(255, 255, 0),
    m_disabledRGB(170, 170, 170),
    m_solvedRGB(255, 0, 255),
    m_backgroundRGB(0, 0, 0)
{
}

//-------------------------------------------------------------------------

void
Boxworld::init()
{
    m_levelSolved = false;
    m_board = m_levels.level(m_level);
    m_boardPrevious = m_board;
    m_canUndo = false;
    findPlayer();
}

//-------------------------------------------------------------------------

void
Boxworld::update(Joystick& js)
{
    if (js.buttonPressed(Joystick::BUTTON_A))
    {
        if (m_level < (Level::c_levelCount - 1))
        {
            ++m_level;
            init();
        }
    }
    else if (js.buttonPressed(Joystick::BUTTON_B))
    {
        if (m_level > 0)
        {
            --m_level;
            init();
        }
    }
    else if (js.buttonPressed(Joystick::BUTTON_X))
    {
        if (m_canUndo)
        {
            m_board = m_boardPrevious;
            findPlayer();
            m_canUndo = false;
        }
    }
    else if (js.buttonPressed(Joystick::BUTTON_Y))
    {
        init();
    }
    else
    {
        auto value = js.getAxes(0);

        if (not value.x and not value.y)
        {
            return;
        }

        const auto dx = (value.x) ? (value.x / std::abs(value.x)) : 0;
        const auto dy = (value.y) ? (value.y / std::abs(value.y)) : 0;

        const Location next{ .x = m_player.x + dx, .y = m_player.y + dy };
        const auto piece1 = m_board[next.y][next.x] & ~c_targetMask;


        if (piece1 == PASSAGE)
        {
            swapPieces(m_player, next);
            m_player = next;
        }
        else if (piece1 == BOX)
        {
            const Location afterBox{ .x = next.x + dx, .y = next.y + dy };
            const auto piece2 = m_board[afterBox.y][afterBox.x] & ~c_targetMask;

            if (piece2 == PASSAGE)
            {
                m_boardPrevious =  m_board;
                swapPieces(next, afterBox);
                swapPieces(m_player, next);
                m_player = next;

                isLevelSolved();
                m_canUndo = not m_levelSolved;
            }
        }
    }
}

//-------------------------------------------------------------------------

void
Boxworld::draw(
    Interface565& fb,
    Interface565Font& font)
{
    drawBoard(fb);
    drawText(fb, font);
}

//-------------------------------------------------------------------------

void
Boxworld::drawBoard(Interface565& fb)
{
    const int xOffset = (fb.getWidth() - m_topTextImage.getWidth()) / 2;
    const int yOffset = 8;
    static uint8_t frame = 0;

    for (int j = 0 ; j < Level::c_levelHeight ; ++j)
    {
        for (int i = 0 ; i < Level::c_levelWidth ; ++i)
        {
            const auto piece = m_board[j][i];
            auto& tile = m_tileBuffers[piece];

            if (tile.getNumberOfFrames() > 1)
            {
                tile.setFrame(frame / 2);
            }

            fb.putImage(
                Interface565Point{
                    (i * c_tileWidth) + xOffset,
                    (j * c_tileHeight) + yOffset
                },
                tile);
        }
    }

    if (frame < 3)
    {
        ++frame;
    }
    else
    {
        frame = 0;
    }
}

//-------------------------------------------------------------------------

void
Boxworld::drawText(
    Interface565& fb,
    Interface565Font& font)
{
    m_topTextImage.clear(m_backgroundRGB);

    Interface565Point position{ 2, 0 };
    position = font.drawString(position,
                               "level: ",
                               m_boldRGB,
                               m_topTextImage);
    position = font.drawString(position,
                               std::to_string(m_level + 1),
                               m_textRGB,
                               m_topTextImage);

    if (m_levelSolved)
    {
        position = font.drawString(position,
                                   " [solved]",
                                   m_solvedRGB,
                                   m_topTextImage);
    }

    int xOffset = (fb.getWidth() - m_topTextImage.getWidth()) / 2;
    fb.putImage(Interface565Point{ xOffset, 0 }, m_topTextImage);

    //---------------------------------------------------------------------

    position = Interface565Point{ 2, 0 };
    auto& undoRGB = ((m_canUndo) ? m_textRGB : m_disabledRGB);

    position = font.drawString(position,
                               "(X): ",
                               m_boldRGB,
                               m_bottomTextImage);
    position = font.drawString(position,
                               "undox box move",
                               undoRGB,
                               m_bottomTextImage);

    position = Interface565Point{ 2, 8 };

    position = font.drawString(position,
                               "(Y): ",
                               m_boldRGB,
                               m_bottomTextImage);
    position = font.drawString(position,
                               "restart level",
                               m_textRGB,
                               m_bottomTextImage);

    const int halfWidth = 2 + (m_bottomTextImage.getWidth() / 2);

    position = Interface565Point{ halfWidth, 2 };
    auto& nextRGB = ((m_level < (Level::c_levelCount - 1))
                  ? m_textRGB
                  : m_disabledRGB);

    position = font.drawString(position,
                               "(A): ",
                               m_boldRGB,
                               m_bottomTextImage);
    position = font.drawString(position,
                               "next level",
                               nextRGB,
                               m_bottomTextImage);

    position = Interface565Point{ halfWidth, 8 };
    auto& previousRGB = ((m_level > 0)
                      ? m_textRGB
                      : m_disabledRGB);

    position = font.drawString(position,
                               "(B): ",
                               m_boldRGB,
                               m_bottomTextImage);
    position = font.drawString(position,
                               "previous level",
                               previousRGB,
                               m_bottomTextImage);

    xOffset = (fb.getWidth() - m_bottomTextImage.getWidth()) / 2;
    fb.putImage(Interface565Point{xOffset, 220}, m_bottomTextImage);
}

//-------------------------------------------------------------------------

void
Boxworld::findPlayer()
{
    bool found = false;

    for (int j = 0 ; (j < Level::c_levelHeight) and not found ; ++j)
    {
        for (int i = 0 ; (i < Level::c_levelWidth) and not found ; ++i)
        {
            if ((m_board[j][i] & ~c_targetMask) == PLAYER)
            {
                found = true;
                m_player.x = i;
                m_player.y = j;
            }
        }
    }
}

//-------------------------------------------------------------------------

void
Boxworld::swapPieces(const Location& location1, const Location& location2)
{
    const auto piece1 = m_board[location1.y][location1.x] & ~c_targetMask;
    const auto piece2 = m_board[location2.y][location2.x] & ~c_targetMask;

    m_board[location1.y][location1.x] = (m_board[location1.y][location1.x] & c_targetMask) | piece2;
    m_board[location2.y][location2.x] = (m_board[location2.y][location2.x] & c_targetMask) | piece1;
}

//-------------------------------------------------------------------------

void
Boxworld::isLevelSolved()
{
    m_levelSolved = true;

    for (int j = 0 ; (j < Level::c_levelHeight) and m_levelSolved ; ++j)
    {
        for (int i = 0 ; (i < Level::c_levelWidth) and m_levelSolved ; ++i)
        {
            if (m_board[j][i] == BOX)
            {
                m_levelSolved = false;
            }
        }
    }
}

