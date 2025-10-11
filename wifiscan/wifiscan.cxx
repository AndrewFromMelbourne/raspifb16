//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2024 Andrew Duncan
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

#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <iwlib.h>
#include <unistd.h>

#include <linux/wireless.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <algorithm>
#include <chrono>
#include <csignal>
#include <cstring>
#include <iostream>
#include <print>
#include <string>
#include <thread>
#include <vector>

#include "image565.h"
#include "image565Font8x16.h"
#include "interface565Factory.h"
#include "scanEntry.h"

//-------------------------------------------------------------------------

using namespace raspifb16;
using namespace std::chrono_literals;

//-------------------------------------------------------------------------

namespace
{
volatile static std::sig_atomic_t run{1};
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

        run = 0;
        break;
    };
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
    std::println(stream, "    --active,-a - use active scan");
    std::println(stream, "    --device,-d - device to use");
    std::println(stream, "    --help,-h - print usage and exit");
    std::println(stream, "    --interface,-i - WiFi interface to use");
    std::println(stream, "    --kmsdrm,-k - use KMS/DRM dumb buffer");
    std::println(stream, "");
}

//-------------------------------------------------------------------------

int
main(
    int argc,
    char *argv[])
{
    std::string device{};
    std::string program{basename(argv[0])};
    auto interfaceType{raspifb16::InterfaceType565::FRAME_BUFFER_565};
    int iwScanType{IW_SCAN_TYPE_PASSIVE};
    std::string interfaceName{"wlan0"};

    //---------------------------------------------------------------------

    static const char* sopts = "ad:hi:k";
    static option lopts[] =
    {
        { "active", no_argument, nullptr, 'a' },
        { "device", required_argument, nullptr, 'd' },
        { "help", no_argument, nullptr, 'h' },
        { "interface", required_argument, nullptr, 'i' },
        { "kmsdrm", no_argument, nullptr, 'k' },
        { nullptr, no_argument, nullptr, 0 }
    };

    int opt{};

    while ((opt = ::getopt_long(argc, argv, sopts, lopts, nullptr)) != -1)
    {
        switch (opt)
        {
        case 'a':

            iwScanType = IW_SCAN_TYPE_ACTIVE;

            break;

        case 'd':

            device = optarg;

            break;

        case 'h':

            printUsage(std::cout, program);
            ::exit(EXIT_SUCCESS);

            break;

        case 'i':

            interfaceName = optarg;

            break;

        case 'k':

            interfaceType = raspifb16::InterfaceType565::KMSDRM_DUMB_BUFFER_565;

            break;

        default:

            printUsage(std::cerr, program);
            ::exit(EXIT_FAILURE);

            break;
        }
    }

    //---------------------------------------------------------------------

    for (auto signal : { SIGINT, SIGTERM  })
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
        auto sockfd = socket(AF_INET, SOCK_DGRAM, 0);

        if (sockfd == -1)
        {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        iw_range range;

        if ((iw_get_range_info(sockfd, interfaceName.c_str(), &range) < 0) or
            (range.we_version_compiled < 14))
        {
            std::println(std::cerr,  "{} doesn't support scanning", interfaceName);
            exit(EXIT_FAILURE);
        }

        __u8 wev = range.we_version_compiled;

        //-----------------------------------------------------------------

        Image565Font8x16 font;
        auto fb{raspifb16::createInterface565(interfaceType, device)};
        fb->clearBuffers();
        Image565 image{fb->getWidth(), fb->getHeight()};

        //-----------------------------------------------------------------

        while (run)
        {
            iwreq request;
            iw_scan_req scanRequest;

            memset(&request, 0, sizeof(request));
            memset(&scanRequest, 0, sizeof(scanRequest));

            scanRequest.scan_type = iwScanType;

            request.u.data.length = sizeof(scanRequest);
            request.u.data.pointer = (caddr_t)&scanRequest;
            request.u.data.flags = IW_SCAN_ALL_ESSID |
                                   IW_SCAN_ALL_FREQ |
                                   IW_SCAN_ALL_MODE |
                                   IW_SCAN_ALL_RATE;

            strncpy(request.ifr_ifrn.ifrn_name, interfaceName.c_str(), IFNAMSIZ);

            if (ioctl(sockfd, SIOCSIWSCAN, &request) == -1)
            {
                perror("ioctl(SIOCSIWSCAN)");
                break;
            }

            timeval startTime;
            timeval endTime;
            timeval diffTime = { 0, 0 };

            bool replyFound = false;
            bool timeout = false;

            char scanBuffer[0xFFFF];

            gettimeofday(&startTime, NULL);

            while ((replyFound == false) and (timeout == false))
            {
                memset(scanBuffer, 0, sizeof(scanBuffer));
                request.u.data.pointer = scanBuffer;
                request.u.data.length = sizeof(scanBuffer);
                request.u.data.flags = 0;

                if (ioctl(sockfd, SIOCGIWSCAN, &request) == 0)
                {
                    replyFound = true;
                }

                gettimeofday(&endTime, NULL);
                timersub(&endTime, &startTime, &diffTime);

                if (diffTime.tv_sec > 10)
                {
                    timeout = true;
                }
                else if (replyFound == false)
                {
                    std::this_thread::sleep_for(100ms);
                }
            }

            if (replyFound)
            {
                iw_event iwe;
                stream_descr stream;
                iw_init_event_stream(&stream,
                                     scanBuffer,
                                     request.u.data.length);

                char eventBuffer[512];

                std::vector<ScanEntry> cells;
                ScanEntry cell;
                bool firstCell = true;

                while (iw_extract_event_stream(&stream, &iwe, wev) > 0)
                {
                    switch(iwe.cmd)
                    {
                    case SIOCGIWAP:
                    {
                        if (firstCell)
                        {
                            firstCell = false;
                        }
                        else
                        {
                            cells.push_back(cell);
                            cell.clear();
                        }

                        const ether_addr *eap =
                            reinterpret_cast<ether_addr *>(iwe.u.ap_addr.sa_data);
                        iw_ether_ntop(eap, eventBuffer);

                        cell.setMac48(eventBuffer);

                        break;
                    }
                    case SIOCGIWFREQ:
                    {
                        const double frequency = iw_freq2float(&(iwe.u.freq));
                        const int channel = iw_freq_to_channel(frequency, &range);

                        if (frequency > 1.0)
                        {
                            cell.setFrequency(frequency / 1000000.0);
                        }

                        if (channel != -1)
                        {
                            cell.setChannel(channel);
                        }

                        break;
                    }
                    case SIOCGIWESSID:
                    {
                        char essid[IW_ESSID_MAX_SIZE + 10];
                        memset(essid, '\0', sizeof(essid));

                        if((iwe.u.essid.pointer) and (iwe.u.essid.length))
                        {
                            memcpy(essid,
                                   iwe.u.essid.pointer,
                                   iwe.u.essid.length);
                        }

                        if(iwe.u.essid.flags)
                        {
                            if((iwe.u.essid.flags & IW_ENCODE_INDEX) > 1)
                            {
                                char encodeIndex[10];
                                sprintf(encodeIndex,
                                        " [%d]",
                                        iwe.u.essid.flags & IW_ENCODE_INDEX);

                                strcat(essid, encodeIndex);
                            }
                        }
                        else
                        {
                            strcpy(essid, "hidden");
                        }

                        cell.setEssid(essid);

                        break;
                    }
                    case IWEVQUAL:
                    {
                        double signalLevel = 0.0;

                        if (iwe.u.qual.updated & IW_QUAL_RCPI)
                        {
                            signalLevel = (iwe.u.qual.level / 2.0) - 110.0;
                        }
                        else if (iwe.u.qual.updated & IW_QUAL_DBM)
                        {
                            int dbLevel = iwe.u.qual.level;

                            if (dbLevel >= 64)
                            {
                                dbLevel -= 0x100;
                            }

                            signalLevel = dbLevel;
                        }

                        const double signalQuality = (iwe.u.qual.qual * 100.0)
                                                   / range.max_qual.qual;

                        cell.setSignalQuality(signalQuality);
                        cell.setSignalLevel(signalLevel);

                        break;
                    }
                    }
                }

                if (firstCell == false)
                {
                    cells.push_back(cell);
                }

                std::ranges::sort(cells, std::greater<ScanEntry>());

                image.clear(RGB565{0, 0, 0});
                Interface565Point position{0, 0};
                const RGB565 white{255, 255, 255};

                for (const auto& entry : cells)
                {
                    position = font.drawString(position,
                                               entry.asString(),
                                               white,
                                               image);

                    position.set(0, position.y() + font.getPixelHeight());
                }

                fb->putImage(Interface565Point{0, 0}, image);
                fb->update();
            }
        }

        close(sockfd);
    }
    catch (std::exception& error)
    {
        std::println(std::cerr, "Error: {}", error.what());
        exit(EXIT_FAILURE);
    }

    return 0;
}
