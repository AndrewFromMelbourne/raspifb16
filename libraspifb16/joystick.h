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

#include <linux/joystick.h>

#include <cstdint>
#include <string>
#include <vector>

#include "fileDescriptor.h"

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

struct JoystickAxes
{
    int32_t x;
    int32_t y;
};

//-------------------------------------------------------------------------

struct ButtonState
{
    bool pressed;
    bool down;
};

//-------------------------------------------------------------------------

class Joystick
{
public:

    enum Buttons
    {
        BUTTON_B = 0,
        BUTTON_A = 1,
        BUTTON_X = 3,
        BUTTON_Y = 4,
        BUTTON_LEFT = 6,
        BUTTON_RIGHT = 7,
        BUTTON_SELECT = 10,
        BUTTON_START = 11
    };

    explicit Joystick(bool blocking = false);
    explicit Joystick(const std::string& device, bool blocking = false);

    int numberOfButtons() const;
    int numberOfAxes() const;

    bool buttonPressed(int button);
    bool buttonDown(int button) const;
    JoystickAxes getAxes(int joystickNumber) const;

    void read();

private:

    void init();
    void process(const struct js_event& event);

    FileDescriptor m_joystickFd;

    bool m_blocking;

    int m_buttonCount;
    int m_joystickCount;

    std::vector<ButtonState> m_buttons;
    std::vector<JoystickAxes> m_joysticks;
};

//-------------------------------------------------------------------------

} // namespace raspifb16

