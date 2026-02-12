//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2015 Andrew Duncan
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

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <syslog.h>
#include <unistd.h>

#include <bsd/libutil.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <array>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <print>
#include <thread>
#include <vector>

#include "image565Font8x16.h"
#include "image565FreeType.h"

#include "cpuTrace.h"
#include "dynamicInfo.h"
#include "interface565Factory.h"
#include "memoryTrace.h"
#include "networkTrace.h"

//-------------------------------------------------------------------------

using namespace std::chrono_literals;

//-------------------------------------------------------------------------

namespace
{
volatile static std::sig_atomic_t run{1};
volatile static std::sig_atomic_t display{1};
}

//-------------------------------------------------------------------------

void
messageLog(
    bool isDaemon,
    const std::string& name,
    int priority,
    const std::string& message)
{
    if (isDaemon)
    {
        ::syslog(LOG_MAKEPRI(LOG_USER, priority), "%s", message.c_str());
    }
    else
    {
        const auto now = floor<std::chrono::seconds>(std::chrono::system_clock::now());
        const auto localTime = std::chrono::current_zone()->to_local(now);

        std::print(std::cerr, "{:%b %e %T} ", localTime);
        std::print(std::cerr, "{} ", "localhost");
        std::print(std::cerr, "{}[{}]:", name, getpid());

        const static std::map<int, std::string> priorityMap
        {
            { LOG_EMERG, "emergency" },
            { LOG_ALERT, "alert" },
            { LOG_CRIT, "critical" },
            { LOG_ERR, "error" },
            { LOG_WARNING, "warning" },
            { LOG_NOTICE, "notice" },
            { LOG_INFO, "info" },
            { LOG_DEBUG, "debug" }
        };

        if (const auto it = priorityMap.find(priority); it != priorityMap.end())
        {
            std::print(std::cerr, "{}", it->second);
        }
        else
        {
            std::print(std::cerr, "unknown({})", priority);
        }

        std::println(std::cerr, ":{}", message);
    }
}

//-------------------------------------------------------------------------

void
perrorLog(
    bool isDaemon,
    const std::string& name,
    const std::string& s)
{
    messageLog(isDaemon, name, LOG_ERR, s + " - " + ::strerror(errno));
}

//-------------------------------------------------------------------------

void
printUsage(
    std::ostream& stream,
    const std::string& name)
{
    std::println(stream, "");
    std::println(stream, "Usage: {}", name);
    std::println(stream, "");
    std::println(stream, "    --daemon,-D - start in the background as a daemon");
    std::println(stream, "    --device,-d - device to use");
    std::println(stream, "    --font,-f - font file to use[:pixel height]");
    std::println(stream, "    --help,-h - print usage and exit");
    std::println(stream, "    --kmsdrm,-k - use KMS/DRM dumb buffer");
    std::println(stream, "    --off,-o - do not display at start");
    std::println(stream, "    --pidfile,-p <pidfile> - create and lock PID file");
    std::println(stream, "");
}

//-------------------------------------------------------------------------

static void
signalHandler(
    int signalNumber) noexcept
{
    switch (signalNumber)
    {
    case SIGINT:
    case SIGTERM:

        run = 0;
        break;

    case SIGUSR1:

        display = 0;
        break;

    case SIGUSR2:

        display = 1;
        break;
    };
}

//-------------------------------------------------------------------------

int
main(
    int argc,
    char *argv[])
{
    std::string device{};
    const std::string program{basename(argv[0])};
    std::string pidfile{};
    bool isDaemon{false};
    auto interfaceType{raspifb16::InterfaceType565::FRAME_BUFFER_565};
    raspifb16::FontConfig fontConfig{};

    //---------------------------------------------------------------------

    static const char* sopts = "d:f:hp:kD";
    static option lopts[] =
    {
        { "daemon", no_argument, nullptr, 'D' },
        { "device", required_argument, nullptr, 'd' },
        { "font", required_argument, nullptr, 'f' },
        { "help", no_argument, nullptr, 'h' },
        { "kmsdrm", no_argument, nullptr, 'k' },
        { "off", no_argument, nullptr, 'o' },
        { "pidfile", required_argument, nullptr, 'p' },
        { nullptr, no_argument, nullptr, 0 }
    };

    int opt{0};

    while ((opt = ::getopt_long(argc, argv, sopts, lopts, nullptr)) != -1)
    {
        switch (opt)
        {
        case 'd':

            device = optarg;

            break;

        case 'f':

            fontConfig = raspifb16::parseFontConfig(optarg, 16);

            break;

        case 'h':

            printUsage(std::cout, program);
            ::exit(EXIT_SUCCESS);

            break;

        case 'k':

            interfaceType = raspifb16::InterfaceType565::KMSDRM_DUMB_BUFFER_565;

            break;

        case 'o':

            display = 0;

            break;

        case 'p':

            pidfile = optarg;

            break;

        case 'D':

            isDaemon = true;

            break;

        default:

            printUsage(std::cerr, program);
            ::exit(EXIT_FAILURE);

            break;
        }
    }

    //---------------------------------------------------------------------

    using pidFile_ptr = std::unique_ptr<pidfh, decltype(&pidfile_remove)>;
    pidFile_ptr pfh{nullptr, &pidfile_remove};

    if (isDaemon)
    {
        if (not pidfile.empty())
        {
            pid_t otherpid;
            pfh.reset(::pidfile_open(pidfile.c_str(), 0600, &otherpid));

            if (not pfh)
            {
                std::println(
                    std::cerr,
                    "{} is already running with pid {}",
                    program,
                    otherpid);
                ::exit(EXIT_FAILURE);
            }
        }

        if (::daemon(0, 0) == -1)
        {
            std::println(std::cerr, "Cannot daemonize");
            ::exit(EXIT_FAILURE);
        }

        if (pfh)
        {
            ::pidfile_write(pfh.get());
        }

        ::openlog(program.c_str(), LOG_PID, LOG_USER);
    }

    //---------------------------------------------------------------------

    for (auto signal : { SIGINT, SIGTERM, SIGUSR1, SIGUSR2 })
    {
        if (std::signal(signal, signalHandler) == SIG_ERR)
        {
            std::println(
                std::cerr,
                "Error: installing {} signal handler : {}",
                strsignal(signal),
                strerror(errno));

            ::exit(EXIT_FAILURE);
        }
    }

    //---------------------------------------------------------------------

    try
    {
        auto fb{raspifb16::createInterface565(interfaceType, device)};

        //-----------------------------------------------------------------

        std::unique_ptr<raspifb16::Interface565Font> ft{std::make_unique<raspifb16::Image565Font8x16>()};

        if (not fontConfig.m_fontFile.empty())
        {
            try
            {
                ft = std::make_unique<raspifb16::Image565FreeType>(fontConfig);
            }
            catch (std::exception& error)
            {
                std::println(std::cerr, "Warning: {}", error.what());
            }
        }

        //-----------------------------------------------------------------

        const int traceHeight = (fb->getHeight() == 240) ? 80 : 100;
        const int gridHeight = traceHeight / 5;

        using Panels = std::vector<std::unique_ptr<Panel>>;

        Panels panels;

        auto panelTop = [](const Panels& panels) -> int
        {
            if (panels.empty())
            {
                return 0;
            }
            else
            {
                return panels.back()->getBottom();
            }
        };

        panels.push_back(
            std::make_unique<DynamicInfo>(fb->getWidth(),
                                          ft->getPixelHeight(),
                                          panelTop(panels)));

        panels.push_back(
            std::make_unique<CpuTrace>(fb->getWidth(),
                                       traceHeight,
                                       ft->getPixelHeight(),
                                       panelTop(panels),
                                       gridHeight));

        panels.push_back(
            std::make_unique<MemoryTrace>(fb->getWidth(),
                                          traceHeight,
                                          ft->getPixelHeight(),
                                          panelTop(panels),
                                          gridHeight));

        if (fb->getHeight() >= 400)
        {
            panels.push_back(
                std::make_unique<NetworkTrace>(fb->getWidth(),
                                               traceHeight,
                                               ft->getPixelHeight(),
                                               panelTop(panels),
                                               gridHeight));
        }

        //-----------------------------------------------------------------

        for (auto& panel : panels)
        {
            panel->init(*ft);
        }

        //-----------------------------------------------------------------

        std::this_thread::sleep_for(1s);

        while (run)
        {
            const auto now = std::chrono::system_clock::now();
            const auto now_t = std::chrono::system_clock::to_time_t(now);

            for (auto& panel : panels)
            {
                panel->update(now_t, *ft);

                if (display)
                {
                    panel->show(*fb);
                }
            }

            fb->update();

            const auto nextSecond = std::chrono::round<std::chrono::seconds>(now) + 1s;
            std::this_thread::sleep_until(nextSecond);
        }
    }
    catch (std::exception& error)
    {
        std::println(std::cerr, "Error: {}", error.what());
        exit(EXIT_FAILURE);
    }

    //---------------------------------------------------------------------

    messageLog(isDaemon, program, LOG_INFO, "exiting");

    if (isDaemon)
    {
        ::closelog();
    }

    //---------------------------------------------------------------------

    return 0 ;
}

