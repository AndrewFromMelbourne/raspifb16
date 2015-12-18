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

#include <cstdarg>
#include <cstddef>
#include <cstdbool>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <system_error>
#include <vector>

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
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
    const char *name,
    int priority,
    const char *format,
    ...)
{
    va_list args;
    va_start(args, format);

    if (isDaemon)
    {
        vsyslog(LOG_MAKEPRI(LOG_USER, priority), format, args);
    }
    else
    {
        fprintf(stderr, "%s[%d]:", name, getpid());

        switch (priority)
        {
        case LOG_DEBUG:

            fprintf(stderr, "debug");
            break;

        case LOG_INFO:

            fprintf(stderr, "info");
            break;

        case LOG_NOTICE:

            fprintf(stderr, "notice");
            break;

        case LOG_WARNING:

            fprintf(stderr, "warning");
            break;

        case LOG_ERR:

            fprintf(stderr, "error");
            break;

        default:

            fprintf(stderr, "unknown(%d)", priority);
            break;
        }

        fprintf(stderr, ":");

        vfprintf(stderr, format, args);

        fprintf(stderr, "\n");
    }

    va_end(args);
}

//-------------------------------------------------------------------------

void
perrorLog(
    bool isDaemon,
    const char *name,
    const char *s)
{
    messageLog(isDaemon,
               name,
               LOG_ERR,
               "%s -  %s",
               s,
               strerror(errno));
}

//-------------------------------------------------------------------------


void
printUsage(
    FILE *fp,
    const char *name)
{
    fprintf(fp, "\n");
    fprintf(fp, "Usage: %s <options>\n", name);
    fprintf(fp, "\n");
    fprintf(fp, "    --daemon - start in the background as a daemon\n");
    fprintf(fp, "    --device - framebuffer device to use");
    fprintf(fp, " (default is %s)\n", defaultDevice);
    fprintf(fp, "    --help - print usage and exit\n");
    fprintf(fp, "    --pidfile <pidfile> - create and lock PID file (if being run as a daemon)\n");
    fprintf(fp, "\n");
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
    char* pidfile = NULL;
    bool isDaemon =  false;

    //---------------------------------------------------------------------

    static const char* sopts = "dhp:D:";
    static struct option lopts[] = 
    {
        { "daemon", no_argument, NULL, 'd' },
        { "help", no_argument, NULL, 'h' },
        { "pidfile", required_argument, NULL, 'p' },
        { "device", required_argument, NULL, 'D' },
        { NULL, no_argument, NULL, 0 }
    };

    int opt = 0;

    while ((opt = getopt_long(argc, argv, sopts, lopts, NULL)) != -1)
    {
        switch (opt)
        {
        case 'd':

            isDaemon = true;

            break;

        case 'h':

            printUsage(stdout, program);
            exit(EXIT_SUCCESS);

            break;

        case 'p':

            pidfile = optarg;

            break;

        case 'D':

            device = optarg;

            break;

        default:

            printUsage(stderr, program);
            exit(EXIT_FAILURE);

            break;
        }
    }

    //---------------------------------------------------------------------

    struct pidfh* pfh = NULL;

    if (isDaemon)
    {
        if (pidfile != NULL)
        {
            pid_t otherpid;
            pfh = pidfile_open(pidfile, 0600, &otherpid);

            if (pfh == NULL)
            {
                fprintf(stderr,
                        "%s is already running %jd\n",
                        program,
                        (intmax_t)otherpid);
                exit(EXIT_FAILURE);
            }
        }
        
        if (daemon(0, 0) == -1)
        {
            fprintf(stderr, "Cannot daemonize\n");

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
            std::unique_ptr<CPanel>(
                new CDynamicInfo(fb.getWidth(),
                                 panelTop(panels))));

        panels.push_back(
            std::unique_ptr<CPanel>(
                new CCpuTrace(fb.getWidth(),
                              traceHeight,
                              panelTop(panels),
                              gridHeight)));

        panels.push_back(
            std::unique_ptr<CPanel>(
                new CMemoryTrace(fb.getWidth(),
                                 traceHeight,
                                 panelTop(panels),
                                 gridHeight)));

        //-----------------------------------------------------------------

        sleep(1);

        while (run)
        {
            struct timeval now;
            gettimeofday(&now, NULL);

            //-------------------------------------------------------------

            for (auto& panel : panels)
            {
                panel->show(fb, now.tv_sec);
            }

            //-------------------------------------------------------------

            gettimeofday(&now, NULL);
            usleep(1000000L - now.tv_usec);
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

