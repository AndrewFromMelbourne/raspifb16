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

#include <array>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <syslog.h>
#include <unistd.h>

#include <bsd/libutil.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

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
        std::cerr << name << "[" << getpid() << "]:";

        switch (priority)
        {
        case LOG_DEBUG:

            std::cerr << "debug";
            break;

        case LOG_INFO:

            std::cerr << "info";
            break;

        case LOG_NOTICE:

            std::cerr << "notice";
            break;

        case LOG_WARNING:

            std::cerr << "warning";
            break;

        case LOG_ERR:

            std::cerr << "error";
            break;

        default:

            std::cerr << "unknown(" << priority << ")";
            break;
        }

        std::cerr << ":" << message << '\n';
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
    std::ostream& os,
    const std::string& name)
{
    os << '\n';
    os << "Usage: " << name << " <options>\n";
    os << '\n';
    os << "    --daemon,-D - start in the background as a daemon\n";
    os << "    --device,-d - device to use\n";
    os << "    --font,-f - font file to use\n";
    os << "    --help,-h - print usage and exit\n";
    os << "    --kmsdrm,-k - use KMS/DRM dumb buffer\n";
    os << "    --off,-o - do not display at start\n";
    os << "    --pidfile,-p <pidfile> - create and lock PID file";
    os << " (if being run as a daemon)\n";
    os << '\n';
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
    std::string program{basename(argv[0])};
    std::string pidfile{};
    bool isDaemon{false};
    auto interfaceType{raspifb16::InterfaceType565::FRAME_BUFFER_565};
    std::string font{};

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

            font = optarg;

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

    pidfh* pfh{};

    if (isDaemon)
    {
        if (not pidfile.empty())
        {
            pid_t otherpid;
            pfh = ::pidfile_open(pidfile.c_str(), 0600, &otherpid);

            if (not pfh)
            {
                std::cerr
                    << program
                    << " is already running "
                    << otherpid
                    << '\n';

                ::exit(EXIT_FAILURE);
            }
        }

        if (::daemon(0, 0) == -1)
        {
            std::cerr << "Cannot daemonize\n";

            if (pfh)
            {
                ::pidfile_remove(pfh);
            }

            ::exit(EXIT_FAILURE);
        }

        if (pfh)
        {
            ::pidfile_write(pfh);
        }

        ::openlog(program.c_str(), LOG_PID, LOG_USER);
    }

    //---------------------------------------------------------------------

    for (auto signal : { SIGINT, SIGTERM, SIGUSR1, SIGUSR2 })
    {
        if (std::signal(signal, signalHandler) == SIG_ERR)
        {
            if (pfh)
            {
                ::pidfile_remove(pfh);
            }

            std::string message {"installing "};
            message += strsignal(signal);
            message += " signal handler";

            perrorLog(isDaemon, program, message);
            ::exit(EXIT_FAILURE);
        }
    }

    //---------------------------------------------------------------------

    try
    {
        auto fb{raspifb16::createInterface565(interfaceType, device)};

        fb->clear(raspifb16::RGB565{0, 0, 0});

        //-----------------------------------------------------------------

        std::unique_ptr<raspifb16::Interface565Font> ft{std::make_unique<raspifb16::Image565Font8x16>()};

        if (not font.empty())
        {
            try
            {
                ft = std::make_unique<raspifb16::Image565FreeType>(font, 16);
            }
            catch (std::exception& error)
            {
                std::cerr << "Warning: " << error.what() << '\n';
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

        fb->clear();
    }
    catch (std::exception& error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        exit(EXIT_FAILURE);
    }

    //---------------------------------------------------------------------

    messageLog(isDaemon, program, LOG_INFO, "exiting");

    if (isDaemon)
    {
        ::closelog();
    }

    if (pfh)
    {
        ::pidfile_remove(pfh);
    }

    //---------------------------------------------------------------------

    return 0 ;
}

