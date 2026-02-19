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

#pragma once

//-------------------------------------------------------------------------

#include <atomic>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <bsd/libutil.h>

#include "interface565.h"
#include "interface565Factory.h"
#include "interface565Font.h"
#include "panel.h"

//-------------------------------------------------------------------------

using pidFile_ptr = std::unique_ptr<struct pidfh, decltype(&pidfile_remove)>;

//-------------------------------------------------------------------------

class RaspInfo
{
public:

    RaspInfo(
        std::atomic<bool>* display,
        std::atomic<bool>* run);

    void messageLog(int priority, std::string_view message) const;
    std::optional<int> parseCommandLine(int argc, char* argv[]);
    void perrorLog(std::string_view s) const;
    [[nodiscard]] std::string programName() const noexcept { return m_programName; }
    void run();

private:

    std::string getHostname() const;
    void init();
    int panelTop() const;
    void printUsage(std::ostream& stream);
    void setFontConfig() noexcept;

    std::string m_device{};
    std::atomic<bool>* m_display{nullptr};
    std::unique_ptr<raspifb16::Interface565> m_fb{nullptr};
    std::unique_ptr<raspifb16::Interface565Font> m_font{nullptr};
    raspifb16::FontConfig m_fontConfig;
    std::string m_hostname{};
    raspifb16::InterfaceType565 m_interfaceType{raspifb16::InterfaceType565::FRAME_BUFFER_565};
    std::vector<std::unique_ptr<Panel>> m_panels{};
    std::string m_programName{};
    std::atomic<bool>* m_run{nullptr};
};

//-------------------------------------------------------------------------

