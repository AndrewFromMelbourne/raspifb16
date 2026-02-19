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

#include <syslog.h>

#include <atomic>
#include <csignal>
#include <cstring>
#include <exception>
#include <format>

#include "raspinfo.h"

//-------------------------------------------------------------------------

namespace
{
static std::atomic<bool> s_run{true};
static std::atomic<bool> s_display{true};
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

        s_run = false;
        break;

    case SIGUSR1:

        s_display = false;
        break;

    case SIGUSR2:

        s_display = true;
        break;
    };
}

//-------------------------------------------------------------------------

void
setSignalHandler(
    RaspInfo& info) noexcept
{
    for (auto signal : { SIGINT, SIGTERM, SIGUSR1, SIGUSR2 })
    {
        if (std::signal(signal, signalHandler) == SIG_ERR)
        {
            info.messageLog(
                LOG_ERR,
                std::format(
                    "Error: installing {} signal handler : {}",
                    strsignal(signal),
                    strerror(errno)));

            ::exit(EXIT_FAILURE);
        }
    }
}

//-------------------------------------------------------------------------

int
main(
    int argc,
    char *argv[])
{
    RaspInfo info{&s_display, &s_run};

    //---------------------------------------------------------------------

    const auto result = info.parseCommandLine(argc, argv);

    if (result.has_value())
    {
        return result.value();
    }

    //---------------------------------------------------------------------

    setSignalHandler(info);

    //---------------------------------------------------------------------

    try
    {
        info.run();
    }
    catch (std::exception& error)
    {
        info.messageLog(LOG_ERR, std::format("Error: {}", error.what()));
        ::exit(EXIT_FAILURE);
    }
}

//-------------------------------------------------------------------------
