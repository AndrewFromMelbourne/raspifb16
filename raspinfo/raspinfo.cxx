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

#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <system_error>
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include <bcm_host.h>
#pragma GCC diagnostic pop

#include "cpuTrace.h"
#include "dynamicInfo.h"
#include "framebuffer565.h"
#include "memoryTrace.h"

//-------------------------------------------------------------------------

volatile bool run = true;

//-------------------------------------------------------------------------

static const char* defaultDevice = "/dev/fb1";

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
        syslog(LOG_MAKEPRI(LOG_USER, priority), message.c_str());
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

        std::cerr << ":" << message << "\n";
    }
}

//-------------------------------------------------------------------------

void
perrorLog(
    bool isDaemon,
    const std::string& name,
    const std::string& s)
{
    messageLog(isDaemon, name, LOG_ERR, s + " - " + strerror(errno));
}

//-------------------------------------------------------------------------


void
printUsage(
    std::ostream& os,
    const std::string& name)
{
    os << "\n";
    os << "Usage: " << name << " <options>\n";
    os << "\n";
    os << "    --daemon - start in the background as a daemon\n";
    os << "    --device - framebuffer device to use";
    os << " (default is " << defaultDevice << ")\n";
    os << "    --help - print usage and exit\n";
    os << "    --pidfile <pidfile> - create and lock PID file";
    os << " (if being run as a daemon)\n";
    os << "\n";
}

//-------------------------------------------------------------------------

static void
signalHandler(
    int signalNumber)
{
    switch (signalNumber)
    {
    case SIGINT:
    case SIGTERM:

        run = false;
        break;
    };
}

//-------------------------------------------------------------------------

int
main(
    int argc,
    char *argv[])
{
    const char* device = defaultDevice;
    char* program = basename(argv[0]);
    char* pidfile = nullptr;
    bool isDaemon =  false;

    //---------------------------------------------------------------------

    static const char* sopts = "dhp:D:";
    static struct option lopts[] = 
    {
        { "daemon", no_argument, nullptr, 'd' },
        { "help", no_argument, nullptr, 'h' },
        { "pidfile", required_argument, nullptr, 'p' },
        { "device", required_argument, nullptr, 'D' },
        { nullptr, no_argument, nullptr, 0 }
    };

    int opt = 0;

    while ((opt = getopt_long(argc, argv, sopts, lopts, nullptr)) != -1)
    {
        switch (opt)
        {
        case 'd':

            isDaemon = true;

            break;

        case 'h':

            printUsage(std::cout, program);
            exit(EXIT_SUCCESS);

            break;

        case 'p':

            pidfile = optarg;

            break;

        case 'D':

            device = optarg;

            break;

        default:

            printUsage(std::cerr, program);
            exit(EXIT_FAILURE);

            break;
        }
    }

    //---------------------------------------------------------------------

    struct pidfh* pfh = nullptr;

    if (isDaemon)
    {
        if (pidfile != nullptr)
        {
            pid_t otherpid;
            pfh = pidfile_open(pidfile, 0600, &otherpid);

            if (pfh == nullptr)
            {
                std::cerr
                    << program
                    << " is already running "
                    << otherpid
                    << "\n";

                exit(EXIT_FAILURE);
            }
        }
        
        if (daemon(0, 0) == -1)
        {
            std::cerr << "Cannot daemonize\n";

            if (pfh)
            {
                pidfile_remove(pfh);
            }

            exit(EXIT_FAILURE);
        }

        if (pfh)
        {
            pidfile_write(pfh);
        }

        openlog(program, LOG_PID, LOG_USER);
    }

    //---------------------------------------------------------------------

    bcm_host_init();

    //---------------------------------------------------------------------

    if (signal(SIGINT, signalHandler) == SIG_ERR)
    {
        if (pfh)
        {
            pidfile_remove(pfh);
        }

        perrorLog(isDaemon, program, "installing SIGINT signal handler");
        exit(EXIT_FAILURE);
    }

    //---------------------------------------------------------------------

    if (signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        if (pfh)
        {
            pidfile_remove(pfh);
        }

        perrorLog(isDaemon, program, "installing SIGTERM signal handler");
        exit(EXIT_FAILURE);
    }

    //---------------------------------------------------------------------

    try
    {
        raspifb16::CFrameBuffer565 fb(device);

        fb.clear(raspifb16::CRGB565(0, 0, 0));

        //-----------------------------------------------------------------

        int16_t traceHeight = 100;

        // FIXME - need a better way to work out height of trace windows.

        if (fb.getHeight() == 240)
        {
            traceHeight = 80;
        }

        int16_t gridHeight = traceHeight / 5;

        using APanels = std::vector<std::unique_ptr<CPanel>>;

        APanels panels;

        auto panelTop = [](const APanels& panels) -> int16_t
        {
            if (panels.empty())
            {
                return 0;
            }
            else
            {
                return panels.back()->getBottom() + 1;
            }
        };

        panels.push_back(
            std::make_unique<CDynamicInfo>(fb.getWidth(),
                                           panelTop(panels)));

        panels.push_back(
            std::make_unique<CCpuTrace>(fb.getWidth(),
                                        traceHeight,
                                        panelTop(panels),
                                        gridHeight));

        panels.push_back(
            std::make_unique<CMemoryTrace>(fb.getWidth(),
                                           traceHeight,
                                           panelTop(panels),
                                           gridHeight));

        //-----------------------------------------------------------------

        constexpr auto oneSecond(std::chrono::seconds(1));

        std::this_thread::sleep_for(oneSecond);

        while (run)
        {
            auto now = std::chrono::system_clock::now();
            auto seconds = std::chrono::system_clock::to_time_t(now) % 60;

            for (auto& panel : panels)
            {
                panel->show(fb, seconds);
            }

            std::this_thread::sleep_for(oneSecond);
        }

        fb.clear();
    }
    catch (std::system_error& error)
    {
        std::cerr << "Error: " << error.what() << "\n";
        exit(EXIT_FAILURE);
    }

    //---------------------------------------------------------------------

    messageLog(isDaemon, program, LOG_INFO, "exiting");

    if (isDaemon)
    {
        closelog();
    }

    if (pfh)
    {
        pidfile_remove(pfh);
    }

    //---------------------------------------------------------------------

    return 0 ;
}

