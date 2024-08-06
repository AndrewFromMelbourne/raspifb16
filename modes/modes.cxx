//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2023 Andrew Duncan
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

#include "drmMode.h"

#include <fcntl.h>
#include <sys/stat.h>

#include <iostream>

//-------------------------------------------------------------------------

void
findConnectedConnectors(
    const std::string& device)
{
    raspifb16::FileDescriptor fd{::open(device.c_str(), O_RDWR)};

    if (fd.fd() == -1)
    {
        return;
    }

    auto resources = drm::drmModeGetResources(fd);

    for (auto i = 0 ; i < resources->count_connectors ; ++i)
    {
        auto connectorId = resources->connectors[i];
        auto connector = drm::drmModeGetConnector(fd, connectorId);
        const bool connected = (connector->connection == DRM_MODE_CONNECTED);

        if (connected and (connector->count_modes > 0))
        {
            for (auto j = 0 ; j < connector->count_encoders ; ++j)
            {
                auto encoderId = connector->encoders[j];
                auto encoder = drm::drmModeGetEncoder(fd, encoderId);

                for (auto k = 0 ; k < resources->count_crtcs ; ++k)
                {
                    uint32_t currentCrtc = 1 << k;

                    if (encoder->possible_crtcs & currentCrtc)
                    {
                        auto crtcId = resources->crtcs[k];
                        auto crtc = drm::drmModeGetCrtc(fd, crtcId);
                        if ((crtc->mode.hdisplay > 0) and (crtc->mode.vdisplay > 0))
                        {
                            std::cout
                                << "device = " << device
                                << ", connector = " << connectorId
                                << ", encoder = " << encoderId
                                << ", ctrc = " << crtcId
                                << ", mode = " << crtc->mode.hdisplay << "x" << crtc->mode.vdisplay
                                << '\n';
                        }
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------

int
main()
{
    drm::DrmDevices devices;

    if (devices.getDeviceCount() < 0)
    {
        return 1;
    }

    for (auto i = 0 ; i < devices.getDeviceCount() ; ++i)
    {
        auto device = devices.getDevice(i);

        if (device->available_nodes & (1 << DRM_NODE_PRIMARY))
        {
            const std::string card = device->nodes[DRM_NODE_PRIMARY];
            if (drm::drmDeviceHasDumbBuffer(card))
            {
                findConnectedConnectors(card);
            }
        }
    }

    return 0;
}

