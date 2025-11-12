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
#include <memory>
#include <string>
#include <system_error>

#include "drmMode.h"
#include "dumbbuffer565.h"
#include "image565.h"
#include "point.h"

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
    m_hasAtomic{false},
    m_hasUniversalPlanes{false},
    m_atomicProperties{},
    m_blobId{0},
    m_connectorId{connectorId},
    m_crtcId{0},
    m_planeId{0},
    m_mode{},
    m_originalCrtc(nullptr, [](drmModeCrtc*){})
{
    std::string card{device};

    if (card.empty())
    {
        card = drm::findDrmDevice(connectorId);
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

    m_hasUniversalPlanes = drm::setUniversalPlanes(m_fd);
    m_hasAtomic = drm::setAtomicModeSetting(m_fd);

    findResources(connectorId);
    drmSetMaster(m_fd.fd());

    if (useAtomic())
    {
        if (drmModeCreatePropertyBlob(
                m_fd.fd(),
                &m_mode,
                sizeof(m_mode),
                &m_blobId) != 0)
        {
            throw std::system_error{errno,
                                    std::system_category(),
                                    "cannot create property blob"};
        }

        createAtomicRequests();
    }

    //---------------------------------------------------------------------

    createDumbBuffer(m_dbFront);
    createDumbBuffer(m_dbBack);

    setDumbBuffer(m_dbFront);

    clear();
    update();
    clear();
}

//-------------------------------------------------------------------------

raspifb16::DumbBuffer565::~DumbBuffer565()
{
    clear();
    update();
    clear();

    if (useAtomic())
    {
        drmModeDestroyPropertyBlob(m_fd.fd(), m_blobId);
    }

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
    const auto& dbb = m_dbs[m_dbBack];
    return {dbb.m_fbp, getBufferSize()};
}

//-------------------------------------------------------------------------

std::span<const uint16_t>
raspifb16::DumbBuffer565::getBuffer() const noexcept
{
    const auto& dbb = m_dbs[m_dbBack];
    return {dbb.m_fbp, getBufferSize()};
}

//-------------------------------------------------------------------------

std::size_t
raspifb16::DumbBuffer565::getBufferSize() const noexcept
{
    return getLineLengthPixels() * m_height;
}

//-------------------------------------------------------------------------

int
raspifb16::DumbBuffer565::getLineLengthPixels() const noexcept
{
    const auto& dbb = m_dbs[m_dbBack];
    return dbb.m_lineLengthPixels;
}

//-------------------------------------------------------------------------

std::size_t
raspifb16::DumbBuffer565::offset(
    const Interface565Point& p) const noexcept
{
    const auto& dbb = m_dbs[m_dbBack];
    return p.x() + p.y() * dbb.m_lineLengthPixels;
}

//-------------------------------------------------------------------------

bool
raspifb16::DumbBuffer565::update()
{
    std::swap(m_dbFront, m_dbBack);
    const auto& dbf = m_dbs[m_dbFront];

    if (useAtomic())
    {
        auto atomicReq = drm::drmModeAtomicAlloc();
        addAtomicProperties(atomicReq, dbf.m_fbId);
        constexpr uint32_t flags = DRM_MODE_PAGE_FLIP_EVENT | DRM_MODE_ATOMIC_NONBLOCK;
        auto result =  drm::drmModeAtomicCommit(m_fd, atomicReq, flags, nullptr);

        if (result < 0)
        {
            throw std::system_error(errno,
                                    std::system_category(),
                                    "unable to flip using atomic");
        }
    }
    else
    {

        drmModePageFlip(m_fd.fd(),
                        m_crtcId,
                        dbf.m_fbId,
                        DRM_MODE_PAGE_FLIP_EVENT,
                        nullptr);
    }

    drmEventContext ev{
        .version = DRM_EVENT_CONTEXT_VERSION,
        .vblank_handler = nullptr,
        .page_flip_handler = nullptr,
        .page_flip_handler2 = nullptr,
        .sequence_handler = nullptr
    };

    drmHandleEvent(m_fd.fd(), &ev);

    return true;
}

//-------------------------------------------------------------------------

void
raspifb16::DumbBuffer565::createDumbBuffer(
    int index)
{
    auto& db = m_dbs[index];

    drm_mode_create_dumb dmcb;
    dmcb.height = m_mode.vdisplay;
    dmcb.width = m_mode.hdisplay;
    dmcb.bpp = 16;
    dmcb.flags = 0;
    dmcb.handle = 0;
    dmcb.pitch = 0;
    dmcb.size = 0;

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

    drm_mode_map_dumb dmmd;
    dmmd.handle = db.m_fbHandle;

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
    const auto& db = m_dbs[index];

    ::munmap(db.m_fbp, db.m_length);
    drmModeRmFB(m_fd.fd(), db.m_fbId);

    drm_mode_destroy_dumb dmdd;
    dmdd.handle = db.m_fbHandle;

    drmIoctl(m_fd.fd(), DRM_IOCTL_MODE_DESTROY_DUMB, &dmdd);
}


//-------------------------------------------------------------------------

void
raspifb16::DumbBuffer565::setDumbBuffer(
    int index)
{
    const auto& db = m_dbs[index];

    if (useAtomic())
    {
        auto atomicReq = drm::drmModeAtomicAlloc();
        addAtomicProperties(atomicReq, db.m_fbId);
        constexpr uint32_t flags = DRM_MODE_ATOMIC_ALLOW_MODESET | DRM_MODE_PAGE_FLIP_EVENT;
        const auto result = drm::drmModeAtomicCommit(m_fd, atomicReq, flags, nullptr);

        if (result < 0)
        {
            throw std::system_error(errno,
                                    std::system_category(),
                                    "unable to set crtc with dumb buffer using atomic");
        }

        drmEventContext ev{
            .version = DRM_EVENT_CONTEXT_VERSION,
            .vblank_handler = nullptr,
            .page_flip_handler = nullptr,
            .page_flip_handler2 = nullptr,
            .sequence_handler = nullptr
        };

        drmHandleEvent(m_fd.fd(), &ev);
    }
    else
    {
        auto setCrtcResult = drmModeSetCrtc(m_fd.fd(),
                                            m_crtcId,
                                            db.m_fbId,
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
}

//-------------------------------------------------------------------------

void
raspifb16::DumbBuffer565::addAtomicRequest(
    uint32_t objectId,
    uint32_t objectType,
    const std::string& propertyName,
    uint64_t value)
{
    auto propertyId{
        drm::findDrmPropertyId(
            m_fd,
            objectId,
            objectType,
            propertyName)};

    if (propertyId)
    {
        m_atomicProperties.push_back(
            AtomicProperty{
                .m_objectId = objectId,
                .m_objectType = objectType,
                .m_propertyId = propertyId,
                .m_propertyName = propertyName,
                .m_value = value });
    }
}

//-------------------------------------------------------------------------

void
raspifb16::DumbBuffer565::addAtomicProperties(
    drm::drmModeAtomicReq_ptr& atomicRequest,
    uint32_t fbId)
{
    for (auto prop : m_atomicProperties)
    {
        if (prop.m_propertyName == "FB_ID")
        {
            prop.m_value = fbId;
        }

        drm::drmModeAtomicAddProperty(
            atomicRequest,
            prop.m_objectId,
            prop.m_propertyId,
            prop.m_value);
    }
}

//-------------------------------------------------------------------------

void
raspifb16::DumbBuffer565::createAtomicRequests()
{
    addAtomicRequest(m_connectorId, DRM_MODE_OBJECT_CONNECTOR, "CRTC_ID", m_crtcId);

    addAtomicRequest(m_crtcId, DRM_MODE_OBJECT_CRTC, "MODE_ID", m_blobId);
    addAtomicRequest(m_crtcId, DRM_MODE_OBJECT_CRTC, "ACTIVE", 1);

    addAtomicRequest(m_planeId, DRM_MODE_OBJECT_PLANE, "FB_ID", 0);
    addAtomicRequest(m_planeId, DRM_MODE_OBJECT_PLANE, "CRTC_ID", m_crtcId);
    addAtomicRequest(m_planeId, DRM_MODE_OBJECT_PLANE, "SRC_X", 0);
    addAtomicRequest(m_planeId, DRM_MODE_OBJECT_PLANE, "SRC_Y", 0);
    addAtomicRequest(m_planeId, DRM_MODE_OBJECT_PLANE, "SRC_W", static_cast<uint64_t>(m_mode.hdisplay) << 16);
    addAtomicRequest(m_planeId, DRM_MODE_OBJECT_PLANE, "SRC_H", static_cast<uint64_t>(m_mode.vdisplay) << 16);
    addAtomicRequest(m_planeId, DRM_MODE_OBJECT_PLANE, "CRTC_X", 0);
    addAtomicRequest(m_planeId, DRM_MODE_OBJECT_PLANE, "CRTC_Y", 0);
    addAtomicRequest(m_planeId, DRM_MODE_OBJECT_PLANE, "CRTC_W", m_mode.hdisplay);
    addAtomicRequest(m_planeId, DRM_MODE_OBJECT_PLANE, "CRTC_H", m_mode.vdisplay);
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

    const auto resource{drm::findDrmResources(m_fd, connectorId)};

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
    m_planeId = resource.m_planeId;
    m_originalCrtc = drm::drmModeGetCrtc(m_fd, resource.m_crtcId);
}

