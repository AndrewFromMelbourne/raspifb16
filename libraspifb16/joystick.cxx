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

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdlib>
#include <fstream>
#include <map>
#include <optional>
#include <ranges>
#include <regex>
#include <system_error>

#include "joystick.h"

//=========================================================================

namespace
{

//-------------------------------------------------------------------------

std::optional<js_event>
readJoystickEvent(
    const fd::FileDescriptor& joystickFd)
{
    js_event event{};
    const auto bytes{::read(joystickFd.fd(), &event, sizeof(event))};

    if ((bytes == -1) or (bytes != sizeof(event)))
    {
        return {};
    }

    return event;
}

//-------------------------------------------------------------------------

std::string
    spaceToUnderscore(
        std::string_view s)
{
    std::string result;
    auto function = [](char c) { return (c == ' ') ? '_' : c; };
    std::ranges::copy(std::views::transform(s, function), std::back_inserter(result));
    return result;
}

//-------------------------------------------------------------------------

}

//=========================================================================

fb16::Joystick::Joystick(ReadType readType)
:
    Joystick("/dev/input/js0", readType)
{
}

//-------------------------------------------------------------------------

fb16::Joystick::Joystick(const std::string& device, ReadType readType)
:
    m_joystickFd{::open(device.c_str(), O_RDONLY | ((readType == ReadType::BLOCKING) ? 0 : O_NONBLOCK))},
    m_readType(readType),
    m_buttonCount(0),
    m_joystickCount(0),
    m_buttons(BUTTON_COUNT),
    m_rawButtons(),
    m_joysticks(),
    m_joystickDpad(0),
    m_buttonNumbers{
        BUTTON_X,
        BUTTON_A,
        BUTTON_B,
        BUTTON_Y,
        BUTTON_LEFT_SHOULDER,
        BUTTON_RIGHT_SHOULDER,
        BUTTON_DPAD_UP,
        BUTTON_DPAD_DOWN,
        BUTTON_SELECT,
        BUTTON_START,
        BUTTON_DPAD_LEFT,
        BUTTON_DPAD_RIGHT,
    }
{
    init();
}

//-------------------------------------------------------------------------

std::string
fb16::Joystick::configurationDirectory() noexcept
{
    return std::getenv("HOME") + std::string{"/.config/raspifb16"};
}

//-------------------------------------------------------------------------

std::string
fb16::Joystick::configurationFile() const noexcept
{
    return configurationDirectory() + "/joystick_" + spaceToUnderscore(m_name) + ".conf";
}

//-------------------------------------------------------------------------

void
fb16::Joystick::buttonsClear()
{
    for (auto& button : m_buttons)
    {
        button.pressed = false;
    }
}

//-------------------------------------------------------------------------

bool
fb16::Joystick::buttonDown(int button) const
{
    if (not isValidButton(button))
    {
        return false;
    }

    return m_buttons.at(button).down;
}

//-------------------------------------------------------------------------

bool
fb16::Joystick::buttonPressed(int button)
{
    if (not isValidButton(button))
    {
        return false;
    }

    const auto pressed = m_buttons.at(button).pressed;

    if (pressed)
    {
        m_buttons[button].pressed = false;
    }

    return pressed;
}

//-------------------------------------------------------------------------

fb16::JoystickAxes
fb16::Joystick::getAxes(int joystickNumber) const
{
    return m_joysticks.at(joystickNumber);
}

//-------------------------------------------------------------------------

void
fb16::Joystick::init()
{
    if (m_joystickFd.fd() == -1)
    {
        throw std::system_error{errno,
                                std::system_category(),
                                "cannot open joystick device"};
    }

    char joystickCount = 0;
    if (ioctl(m_joystickFd.fd(), JSIOCGAXES, &joystickCount) == -1)
    {
        throw std::system_error{errno,
                                std::system_category(),
                                "reading number of axes"};
    }
    m_joystickCount = joystickCount / 2;

    char buttonCount = 0;
    if (ioctl(m_joystickFd.fd(), JSIOCGBUTTONS, &buttonCount) == -1)
    {
        throw std::system_error{errno,
                                std::system_category(),
                                "reading number of buttons"};
    }
    m_buttonCount = buttonCount;

    char jsname[128];
    if (ioctl(m_joystickFd.fd(), JSIOCGNAME(sizeof(jsname)), jsname) != -1)
    {
        m_name = jsname;
    }
    else
    {
        m_name = "Unknown Joystick";
    }

    m_buttons.resize(m_buttonCount, ButtonState{ false, false });
    m_rawButtons.resize(m_buttonCount, ButtonState{ false, false });
    m_joysticks.resize(m_joystickCount, JoystickAxes{ 0, 0 });

    readConfig();
}

//-------------------------------------------------------------------------

bool
fb16::Joystick::isValidButton(int button) const noexcept
{
    return (button >= 0) and (button < numberOfButtons());
}

//-------------------------------------------------------------------------

int
fb16::Joystick::numberOfButtons() const noexcept
{
    return m_buttonCount;
}

//-------------------------------------------------------------------------

int
fb16::Joystick::numberOfAxes() const noexcept
{
    return m_joystickCount;
}

//-------------------------------------------------------------------------

void
fb16::Joystick::process(const js_event& event)
{
    switch (event.type)
    {
        case JS_EVENT_BUTTON:
        {
            if (event.number >= m_buttonNumbers.size())
            {
                return;
            }

            auto number = m_buttonNumbers[event.number];

            if (event.value)
            {
                m_buttons.at(number) = ButtonState{ .pressed = true,
                                                    .down = true };
                m_rawButtons.at(event.number) = ButtonState{ .pressed = true,
                                                             .down = true };
            }
            else
            {
                m_buttons.at(number).down = false;
                m_rawButtons.at(event.number).down = false;
            }

            break;
        }
        case JS_EVENT_AXIS:
        {
            const auto axis{event.number / 2};

            if (axis < m_joystickCount)
            {
                auto axes = m_joysticks.at(axis);

                if (event.number % 2 == 0)
                {
                    axes.x = event.value;
                }
                else
                {
                    axes.y = event.value;
                }

                m_joysticks[axis] = axes;
            }

            break;
        }
    }
}

//-------------------------------------------------------------------------

int
fb16::Joystick::rawButton(int button) const
{
    const auto it = std::find_if(m_buttonNumbers.begin(),
                                 m_buttonNumbers.end(),
                                 [button](Buttons b) { return b == button; });
    if (it != m_buttonNumbers.end())
    {
        return std::distance(m_buttonNumbers.begin(), it);
    }

    return m_buttonCount;
}

//-------------------------------------------------------------------------

bool
fb16::Joystick::rawButtonDown(int button) const
{
    if (not isValidButton(button))
    {
        return false;
    }

    return m_rawButtons.at(button).down;
}

//-------------------------------------------------------------------------

bool
fb16::Joystick::rawButtonPressed(int button)
{
    if (not isValidButton(button))
    {
        return false;
    }

    const auto pressed = m_rawButtons.at(button).pressed;

    if (pressed)
    {
        m_rawButtons[button].pressed = false;
    }

    return pressed;
}

//-------------------------------------------------------------------------

void
fb16::Joystick::read()
{
    if (m_readType == ReadType::BLOCKING)
    {
        auto event{readJoystickEvent(m_joystickFd)};
        if (event)
        {
            process(*event);
        }
    }
    else
    {
        while (auto event = readJoystickEvent(m_joystickFd))
        {
            process(*event);
        }
    }
}

//-------------------------------------------------------------------------

void
fb16::Joystick::
readConfig()
{
    std::map<std::string, Buttons> stringToButton =
    {
        {"BUTTON_X", BUTTON_X},
        {"BUTTON_A", BUTTON_A},
        {"BUTTON_B", BUTTON_B},
        {"BUTTON_Y", BUTTON_Y},
        {"BUTTON_LEFT_SHOULDER", BUTTON_LEFT_SHOULDER},
        {"BUTTON_RIGHT_SHOULDER", BUTTON_RIGHT_SHOULDER},
        {"BUTTON_SELECT", BUTTON_SELECT},
        {"BUTTON_START", BUTTON_START},
    };

    std::ifstream ifs{configurationFile().c_str()};

    if (ifs)
    {
        const std::regex pattern{R"((\w+)\s*=\s*(\d+))"};
        std::string line;

        while (std::getline(ifs, line))
        {
            try
            {
                std::smatch match;

                if (not std::regex_match(line, match, pattern))
                {
                    continue;
                }

                const auto key = match[1].str();
                const auto value = std::stoul(match[2].str());

                if (key.starts_with("BUTTON_"))
                {
                    if (value < m_buttonNumbers.size())
                    {
                        const auto button = stringToButton.at(key);
                        m_buttonNumbers[value] = button;
                    }
                }
                else if (key == "AXES_DPAD")
                {
                    const int joystickNumber = static_cast<int>(value);

                    if (joystickNumber < m_joystickCount)
                    {
                        m_joystickDpad = joystickNumber;
                    }
                }
            }
            catch (std::exception&)
            {
                // ignore
            }
        }
    }
}

