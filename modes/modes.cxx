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

#include <fcntl.h>
#include <fmt/format.h>
#include <sys/stat.h>

#include "drmMode.h"

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

    const auto resources = drm::drmModeGetResources(fd);

    for (auto i = 0 ; i < resources->count_connectors ; ++i)
    {
        const auto connectorId = resources->connectors[i];
        const auto connector = drm::drmModeGetConnector(fd, connectorId);
        const bool connected = (connector->connection == DRM_MODE_CONNECTED);

        if (connected and (connector->count_modes > 0))
        {
            for (auto j = 0 ; j < connector->count_encoders ; ++j)
            {
                const auto encoderId = connector->encoders[j];
                const auto encoder = drm::drmModeGetEncoder(fd, encoderId);

                for (auto k = 0 ; k < resources->count_crtcs ; ++k)
                {
                    const uint32_t currentCrtc = 1 << k;

                    if (encoder->possible_crtcs & currentCrtc)
                    {
                        const auto crtcId = resources->crtcs[k];
                        const auto crtc = drm::drmModeGetCrtc(fd, crtcId);
                        if ((crtc->mode.hdisplay > 0) and (crtc->mode.vdisplay > 0))
                        {
                            fmt::print(
                                "device = {}, connector = {}, encoder = {}, crtc = {}, mode = {}x{}\n",
                                device,
                                connectorId,
                                encoderId,
                                crtcId,
                                crtc->mode.hdisplay,
                                crtc->mode.vdisplay);
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
        const auto device = devices.getDevice(i);

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

