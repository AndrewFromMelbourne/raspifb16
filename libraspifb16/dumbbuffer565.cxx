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

#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <libdrm/drm_fourcc.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <system_error>

#include "drmMode.h"
#include "dumbbuffer565.h"
#include "image565.h"
#include "point.h"

//=========================================================================

namespace
{

//-------------------------------------------------------------------------

struct FoundDrmResource
{
    bool m_found{false};
    uint32_t m_connectorId{};
    uint32_t m_crtcId{};
    drmModeModeInfo m_mode{};
};

//-------------------------------------------------------------------------

FoundDrmResource
findDrmResourcesForConnector(
    raspifb16::FileDescriptor& fd,
    uint32_t connectorId,
    const drm::drmModeRes_ptr& resources) noexcept
{
    const auto connector{drm::drmModeGetConnector(fd, connectorId)};
    const bool connected{connector->connection == DRM_MODE_CONNECTED};

    if (connected and (connector->count_modes > 0))
    {
        for (auto j = 0 ; j < connector->count_encoders ; ++j)
        {
            const auto encoderId = connector->encoders[j];
            const auto encoder = drm::drmModeGetEncoder(fd, encoderId);

            for (auto k = 0 ; k < resources->count_crtcs ; ++k)
            {
                const auto currentCrtc{1 << k};

                if (encoder->possible_crtcs & currentCrtc)
                {
                    const auto currentCrtcId = resources->crtcs[k];
                    const auto crtc{drm::drmModeGetCrtc(fd, currentCrtcId)};
                    if ((crtc->mode.hdisplay > 0) and (crtc->mode.vdisplay > 0))
                    {
                        return FoundDrmResource{
                            .m_found = true,
                            .m_connectorId = connectorId,
                            .m_crtcId = currentCrtcId,
                            .m_mode = crtc->mode
                        };
                    }
                }
            }
        }
    }

    return FoundDrmResource{ .m_found = false };
}

//-------------------------------------------------------------------------

FoundDrmResource
findDrmResourcesForConnector(
    raspifb16::FileDescriptor& fd,
    uint32_t connectorId) noexcept
{
    return findDrmResourcesForConnector(fd, connectorId, drm::drmModeGetResources(fd));
}


//-------------------------------------------------------------------------

FoundDrmResource
findDrmResources(
    raspifb16::FileDescriptor& fd,
    uint32_t connectorId) noexcept
{
    if (connectorId)
    {
        return findDrmResourcesForConnector(fd, connectorId, drm::drmModeGetResources(fd));
    }

    const auto resources = drm::drmModeGetResources(fd);

    for (int i = 0 ; i < resources->count_connectors ; ++i)
    {
        connectorId = resources->connectors[i];
        const auto resource{findDrmResourcesForConnector(fd, connectorId, resources)};

        if (resource.m_found)
        {
            return resource;
        }
    }

    return FoundDrmResource{ .m_found = false };
}

//-------------------------------------------------------------------------

std::string
findDrmDevice()
{
    drm::DrmDevices devices;

    if (devices.getDeviceCount() < 0)
    {
        return "";
    }

    for (auto i = 0 ; i < devices.getDeviceCount() ; ++i)
    {
        const auto device = devices.getDevice(i);

        if ((device->available_nodes & (1 << DRM_NODE_PRIMARY)) and
            drm::drmDeviceHasDumbBuffer(device->nodes[DRM_NODE_PRIMARY]))
        {
            return device->nodes[DRM_NODE_PRIMARY];
        }
    }

    return "";
}

//-------------------------------------------------------------------------

std::string
findDrmDeviceWithConnector(
    uint32_t connectorId)
{
    drm::DrmDevices devices;

    if (devices.getDeviceCount() < 0)
    {
        return "";
    }

    for (auto i = 0 ; i < devices.getDeviceCount() ; ++i)
    {
        const auto device = devices.getDevice(i);

        if ((device->available_nodes & (1 << DRM_NODE_PRIMARY)) and
            drm::drmDeviceHasDumbBuffer(device->nodes[DRM_NODE_PRIMARY]))
        {
            const auto card{device->nodes[DRM_NODE_PRIMARY]};
            auto fd = raspifb16::FileDescriptor{::open(card, O_RDWR)};
            const auto resources = drm::drmModeGetResources(fd);

            for (int i = 0 ; i < resources->count_connectors ; ++i)
            {
                if (connectorId == resources->connectors[i])
                {
                    return card;
                }
            }
        }
    }

    return "";
}

//-------------------------------------------------------------------------

std::string
findDrmDevice(
    uint32_t connectorId)
{
    std::string device = (connectorId) ?
                         findDrmDeviceWithConnector(connectorId) :
                         findDrmDevice();
    if (device.empty())
    {
        if (connectorId)
        {
            throw std::system_error{errno,
                                    std::system_category(),
                                    "cannot find dri device for connnector " +
                                    std::to_string(connectorId)};

        }
        else
        {
            throw std::system_error{errno,
                                    std::system_category(),
                                    "cannot find a dri device "};
        }
    }

    return device;
}

//-------------------------------------------------------------------------

}

//=========================================================================

raspifb16::DumbBuffer565::DumbBuffer565(
    const std::string& device,
    uint32_t connectorId)
:
    m_width{0},
    m_height{0},
    m_fd{},
    m_dbs{},
    m_dbFront{0},
    m_dbBack{1},
    m_connectorId{connectorId},
    m_crtcId{0},
    m_mode{},
    m_originalCrtc(nullptr, [](drmModeCrtc*){})
{
    std::string card{device};

    if (card.empty())
    {
        card = findDrmDevice(connectorId);
    }

    m_fd = FileDescriptor{::open(card.c_str(), O_RDWR)};

    //---------------------------------------------------------------------

    if (m_fd.fd() == -1)
    {
        throw std::system_error{errno,
                                std::system_category(),
                                "cannot open dri device " + device};
    }

    //---------------------------------------------------------------------

    findResources(connectorId);
    drmSetMaster(m_fd.fd());

    //---------------------------------------------------------------------

    createDumbBuffer(m_dbFront);
    createDumbBuffer(m_dbBack);

    update();
}

//-------------------------------------------------------------------------

raspifb16::DumbBuffer565::~DumbBuffer565()
{
    destroyDumbBuffer(m_dbBack);
    destroyDumbBuffer(m_dbFront);

    drmModeSetCrtc(m_fd.fd(),
                    m_originalCrtc->crtc_id,
                    m_originalCrtc->buffer_id,
                    m_originalCrtc->x,
                    m_originalCrtc->y,
                    &m_connectorId,
                    1,
                    &(m_originalCrtc->mode));

    if (drmIsMaster(m_fd.fd()))
    {
        drmDropMaster(m_fd.fd());
    }
}

//-------------------------------------------------------------------------

std::span<uint16_t>
raspifb16::DumbBuffer565::getBuffer() noexcept
{
    auto& dbb = m_dbs[m_dbBack];
    return {dbb.m_fbp, getBufferSize()};
}

//-------------------------------------------------------------------------

std::span<const uint16_t>
raspifb16::DumbBuffer565::getBuffer() const noexcept
{
    auto& dbb = m_dbs[m_dbBack];
    return {dbb.m_fbp, getBufferSize()};
}

//-------------------------------------------------------------------------

size_t
raspifb16::DumbBuffer565::getBufferSize() const noexcept
{
    return getLineLengthPixels() * m_height;
}

//-------------------------------------------------------------------------

int
raspifb16::DumbBuffer565::getLineLengthPixels() const noexcept
{
    auto& dbb = m_dbs[m_dbBack];
    return dbb.m_lineLengthPixels;
}

//-------------------------------------------------------------------------

size_t
raspifb16::DumbBuffer565::offset(
    const Interface565Point& p) const noexcept
{
    auto& dbb = m_dbs[m_dbBack];
    return p.x() + p.y() * dbb.m_lineLengthPixels;
}

//-------------------------------------------------------------------------

void
raspifb16::DumbBuffer565::update()
{
    std::swap(m_dbFront, m_dbBack);
    auto& dbf = m_dbs[m_dbFront];

    drmVBlank vbl;
    vbl.request.type = DRM_VBLANK_RELATIVE;
    vbl.request.sequence = 1;
    drmWaitVBlank(m_fd.fd(), &vbl);

    auto setCrtcResult = drmModeSetCrtc(m_fd.fd(),
                                        m_crtcId,
                                        dbf.m_fbId,
                                        0,
                                        0,
                                        &m_connectorId,
                                        1,
                                        &m_mode);
    if (setCrtcResult < 0)
    {
        throw std::system_error(errno,
                                std::system_category(),
                                "unable to set crtc with frame buffer");
    }
}

//-------------------------------------------------------------------------

void
raspifb16::DumbBuffer565::createDumbBuffer(
    int index)
{
    auto& db = m_dbs[index];

    drm_mode_create_dumb dmcb =
    {
        .height = m_mode.vdisplay,
        .width = m_mode.hdisplay,
        .bpp = 16,
        .flags = 0,
        .handle = 0,
        .pitch = 0,
        .size = 0
    };

    if (drmIoctl(m_fd.fd(), DRM_IOCTL_MODE_CREATE_DUMB, &dmcb) < 0)
    {
        throw std::system_error{errno,
                                std::system_category(),
                                "Cannot create a DRM dumb buffer"};
    }

    //---------------------------------------------------------------------

    db.m_length = dmcb.size;
    db.m_lineLengthPixels = dmcb.pitch / c_bytesPerPixel;
    db.m_fbHandle = dmcb.handle;

    uint32_t handles[4] = { dmcb.handle };
    uint32_t strides[4] = { dmcb.pitch };
    uint32_t offsets[4] = { 0 };

    if (drmModeAddFB2(m_fd.fd(),
                      m_mode.hdisplay,
                      m_mode.vdisplay,
                      DRM_FORMAT_RGB565,
                      handles,
                      strides,
                      offsets,
                      &db.m_fbId,
                      0) < 0)
    {
        throw std::system_error{errno,
                                std::system_category(),
                                "Cannot add frame buffer"};
    }

    //---------------------------------------------------------------------

    drm_mode_map_dumb dmmd =
    {
        .handle = db.m_fbHandle
    };

    if (drmIoctl(m_fd.fd(), DRM_IOCTL_MODE_MAP_DUMB, &dmmd) < 0)
    {
        throw std::system_error{errno,
                                std::system_category(),
                                "Cannot map dumb buffer"};
    }

    void* fbp = mmap(0,
                     db.m_length,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     m_fd.fd(),
                     dmmd.offset);

    if (fbp == MAP_FAILED)
    {
        throw std::system_error(errno,
                                std::system_category(),
                                "mapping framebuffer device to memory");
    }

    db.m_fbp = static_cast<uint16_t*>(fbp);
}

//-------------------------------------------------------------------------

void
raspifb16::DumbBuffer565::destroyDumbBuffer(
    int index)
{
    auto& db = m_dbs[index];

    ::munmap(db.m_fbp, db.m_length);
    drmModeRmFB(m_fd.fd(), db.m_fbId);

    drm_mode_destroy_dumb dmdd =
    {
        .handle = db.m_fbHandle
    };

    drmIoctl(m_fd.fd(), DRM_IOCTL_MODE_DESTROY_DUMB, &dmdd);
}

//-------------------------------------------------------------------------

void
raspifb16::DumbBuffer565::findResources(
    uint32_t connectorId)
{
    uint64_t hasDumb;
    if ((drmGetCap(m_fd.fd(), DRM_CAP_DUMB_BUFFER, &hasDumb) < 0) or not hasDumb)
    {
        throw std::system_error{errno,
                                std::system_category(),
                                "no DRM dumb buffer capability"};
    }

    //---------------------------------------------------------------------

    const auto resource{findDrmResources(m_fd, connectorId)};

    if (not resource.m_found)
    {
        if (connectorId)
        {
            throw std::logic_error(
                "cannot find connector " +
                std::to_string(connectorId));
        }
        throw std::logic_error("no connected CRTC found");
    }

    //---------------------------------------------------------------------

    m_mode = resource.m_mode;
    m_width = m_mode.hdisplay;
    m_height = m_mode.vdisplay;

    m_connectorId = resource.m_connectorId;
    m_crtcId = resource.m_crtcId;
    m_originalCrtc = drm::drmModeGetCrtc(m_fd, resource.m_crtcId);
}

