//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2022 Andrew Duncan
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
#include <sys/stat.h>

#include "drmMode.h"
#include "fileDescriptor.h"

//=========================================================================

bool
drm::drmDeviceHasDumbBuffer(
    const std::string& device)
{
    fd::FileDescriptor fd{::open(device.c_str(), O_RDWR)};

    if (fd.fd() == -1)
    {
        return false;
    }

    //---------------------------------------------------------------------

    uint64_t hasDumb{};
    return not drm::drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &hasDumb) and hasDumb;
}

//-------------------------------------------------------------------------

drm::DrmDevices::DrmDevices()
:
    m_deviceCount{},
    m_devices{}
{
    m_deviceCount = drmGetDevices2(0, m_devices.data(), m_devices.size());
}

//-------------------------------------------------------------------------

drm::DrmDevices::~DrmDevices()
{
    if (m_deviceCount > 0)
    {
        drmFreeDevices(m_devices.data(), m_deviceCount);
    }
}

//-------------------------------------------------------------------------

drmDevicePtr
drm::DrmDevices::getDevice(int index) const
{
    if (index < 0 or index >= m_deviceCount)
    {
        return nullptr;
    }

    return m_devices[index];
}

//=========================================================================

uint64_t
drm::drmGetPropertyValue(
    fd::FileDescriptor& fd,
    uint32_t objectId,
    uint32_t objectType,
    const std::string& name) noexcept
{
    auto properties{drmModeObjectGetProperties(fd, objectId, objectType)};

    for (auto i = 0U; i < properties->count_props; ++i)
    {
        auto property{drmModeGetProperty(fd, properties->props[i])};

        if (name == property->name)
        {
            return properties->prop_values[i];
        }
    }

    return ~0;
}

//-------------------------------------------------------------------------

drm::drmVersion_ptr
drm::drmGetVersion(
    fd::FileDescriptor& fd) noexcept
{
    return drmVersion_ptr(::drmGetVersion(fd.fd()), &drmFreeVersion);
}

//-------------------------------------------------------------------------

drm::drmModeAtomicReq_ptr
drm::drmModeAtomicAlloc() noexcept
{
    return drmModeAtomicReq_ptr(::drmModeAtomicAlloc(),
                                &drmModeAtomicFree);
}

//-------------------------------------------------------------------------

drm::drmModeConnector_ptr
drm::drmModeGetConnector(
    fd::FileDescriptor& fd,
    uint32_t connId) noexcept
{
    return drmModeConnector_ptr(::drmModeGetConnector(fd.fd(), connId),
                                &drmModeFreeConnector);
}

//-------------------------------------------------------------------------

drm::drmModeCrtc_ptr
drm::drmModeGetCrtc(
    fd::FileDescriptor& fd,
    uint32_t crtcId) noexcept
{
    return drmModeCrtc_ptr(::drmModeGetCrtc(fd.fd(), crtcId),
                           &drmModeFreeCrtc);
}

//-------------------------------------------------------------------------

drm::drmModeEncoder_ptr
drm::drmModeGetEncoder(
    fd::FileDescriptor& fd,
    uint32_t encoderId) noexcept
{
    return drmModeEncoder_ptr(::drmModeGetEncoder(fd.fd(), encoderId),
                              &drmModeFreeEncoder);
}

//-------------------------------------------------------------------------

drm::drmModeObjectProperties_ptr
drm::drmModeObjectGetProperties(
    fd::FileDescriptor& fd,
    uint32_t objectId,
    uint32_t objectType) noexcept
{
    return drmModeObjectProperties_ptr(::drmModeObjectGetProperties(fd.fd(),
                                                                    objectId,
                                                                    objectType),
                                       &drmModeFreeObjectProperties);
}

//-------------------------------------------------------------------------

drm::drmModePlane_ptr
drm::drmModeGetPlane(
    fd::FileDescriptor& fd,
    uint32_t planeId) noexcept
{
    return drmModePlane_ptr(::drmModeGetPlane(fd.fd(), planeId),
                            &drmModeFreePlane);
}

//-------------------------------------------------------------------------

drm::drmModePlaneRes_ptr
drm::drmModeGetPlaneResources(
    fd::FileDescriptor& fd) noexcept
{
    return drmModePlaneRes_ptr(::drmModeGetPlaneResources(fd.fd()),
                               &drmModeFreePlaneResources);
}

//-------------------------------------------------------------------------

drm::drmModePropertyBlobRes_ptr
drm::drmModeGetPropertyBlob(
    fd::FileDescriptor& fd,
    uint32_t blobId) noexcept
{
    return drmModePropertyBlobRes_ptr(::drmModeGetPropertyBlob(fd.fd(), blobId),
                                      &drmModeFreePropertyBlob);
}

//-------------------------------------------------------------------------

drm::drmModePropertyRes_ptr
drm::drmModeGetProperty(
    fd::FileDescriptor& fd,
    uint32_t propertyId) noexcept
{
    return drmModePropertyRes_ptr(::drmModeGetProperty(fd.fd(), propertyId),
                                  &drmModeFreeProperty);
}

//-------------------------------------------------------------------------

drm::drmModeRes_ptr
drm::drmModeGetResources(
    fd::FileDescriptor& fd) noexcept
{
    return drmModeRes_ptr(::drmModeGetResources(fd.fd()),
                          &drmModeFreeResources);
}

//=========================================================================

int
drm::drmDropMaster(
    fd::FileDescriptor& fd) noexcept
{
    return ::drmDropMaster(fd.fd());
}

//-------------------------------------------------------------------------

int
drm::drmHandleEvent(
    fd::FileDescriptor& fd,
    drmEventContext* ev) noexcept
{
    return ::drmHandleEvent(fd.fd(), ev);
}

//-------------------------------------------------------------------------

int
drm::drmIoctl(
    fd::FileDescriptor& fd,
    unsigned long request,
    void *arg) noexcept
{
    return ::drmIoctl(fd.fd(), request, arg);
}

//-------------------------------------------------------------------------

int
drm::drmIsMaster(
    fd::FileDescriptor& fd) noexcept
{
    return ::drmIsMaster(fd.fd());
}

//-------------------------------------------------------------------------

int
drm::drmModeAddFB2(
    fd::FileDescriptor& fd,
    uint32_t width,
    uint32_t height,
	uint32_t pixel_format,
    const uint32_t bo_handles[4],
    const uint32_t pitches[4],
    const uint32_t offsets[4],
    uint32_t *buf_id,
    uint32_t flags) noexcept
{
    return ::drmModeAddFB2(fd.fd(),
                           width,
                           height,
						   pixel_format,
                           bo_handles,
                           pitches,
                           offsets,
                           buf_id,
                           flags);
}

//-------------------------------------------------------------------------

int
drm::drmModeAtomicCommit(
    fd::FileDescriptor& fd,
    drmModeAtomicReq_ptr& req,
    uint32_t flags,
    void* user_data)
{
    return ::drmModeAtomicCommit(fd.fd(), req.get(), flags, user_data);
}

//-------------------------------------------------------------------------

int
drm::drmModeCreatePropertyBlob(
    fd::FileDescriptor& fd,
    const void *data,
    size_t size,
    uint32_t *id) noexcept
{
    return ::drmModeCreatePropertyBlob(fd.fd(), data, size, id);
}

//-------------------------------------------------------------------------

int
drm::drmGetCap(
    fd::FileDescriptor& fd,
    uint64_t capability,
    uint64_t *value) noexcept
{
    return ::drmGetCap(fd.fd(), capability, value);
}

//-------------------------------------------------------------------------

int
drm::drmModeDestroyPropertyBlob(
    fd::FileDescriptor& fd,
    uint32_t id) noexcept
{
    return ::drmModeDestroyPropertyBlob(fd.fd(), id);
}

    //-------------------------------------------------------------------------

int
drm::drmModePageFlip(
    fd::FileDescriptor& fd,
    uint32_t crtc_id,
    uint32_t fb_id,
    uint32_t flags,
    void *user_data) noexcept
{
    return ::drmModePageFlip(fd.fd(), crtc_id, fb_id, flags, user_data);
}

//-------------------------------------------------------------------------

int
drm::drmModeRmFB(
    fd::FileDescriptor& fd,
    uint32_t bufferId) noexcept
{
    return ::drmModeRmFB(fd.fd(), bufferId);
}

//-------------------------------------------------------------------------

int
drm::drmModeSetCrtc(
    fd::FileDescriptor& fd,
    uint32_t crtcId,
    uint32_t bufferId,
    uint32_t x,
    uint32_t y,
    uint32_t *connectors,
    int count,
    drmModeModeInfoPtr mode) noexcept
{
    return ::drmModeSetCrtc(fd.fd(),
                            crtcId,
                            bufferId,
                            x,
                            y,
                            connectors,
                            count,
                            mode);
}

//-------------------------------------------------------------------------

int
drm::drmSetMaster(
    fd::FileDescriptor& fd) noexcept
{
    return ::drmSetMaster(fd.fd());
}

//-------------------------------------------------------------------------

bool
drm::addDrmPropertyToAtomicRequest(
    drmModeAtomicReq_ptr& atomicReq,
    fd::FileDescriptor& fd,
    uint32_t objectId,
    uint32_t objectType,
    const std::string& propertyName,
    uint64_t value) noexcept
{
    const auto propertyId{findDrmPropertyId(fd,
                                            objectId,
                                            objectType,
                                            propertyName)};

    if (propertyId)
    {
        drmModeAtomicAddProperty(atomicReq,
                                 objectId,
                                 propertyId,
                                 value);
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------

int
drm::drmModeAtomicAddProperty(
    drmModeAtomicReq_ptr& atomicReq,
    uint32_t object_id,
    uint32_t property_id,
    uint64_t value) noexcept
{
    return ::drmModeAtomicAddProperty(atomicReq.get(),
                                     object_id,
                                     property_id,
                                     value);
}

//-------------------------------------------------------------------------

std::string
drm::findDrmDevice() noexcept
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
            std::string card = device->nodes[DRM_NODE_PRIMARY];

            if (getModeCount(card) > 0)
            {
                return card;
            }
        }
    }

    return "";
}

//-------------------------------------------------------------------------

std::string
drm::findDrmDeviceWithConnector(
    uint32_t connectorId) noexcept
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
            auto fd = fd::FileDescriptor{::open(card, O_RDWR)};
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

//----------------------------------------------------------------------

std::string
drm::findDrmDevice(
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

//----------------------------------------------------------------------

uint32_t
drm::findDrmPrimaryPlaneId(
    fd::FileDescriptor& fd,
    uint32_t crtcMask) noexcept
{
    const auto planeResources{drm::drmModeGetPlaneResources(fd)};

    for (auto i = 0U; i < planeResources->count_planes; ++i)
    {
        const auto plane_id = planeResources->planes[i];
        const auto plane{drm::drmModeGetPlane(fd, plane_id)};

        if (plane and (plane->possible_crtcs & crtcMask))
        {
            const auto typeId{
                drm::drmGetPropertyValue(fd,
                                         plane_id,
                                         DRM_MODE_OBJECT_PLANE,
                                         "type")};

            if (typeId == DRM_PLANE_TYPE_PRIMARY)
            {
                return plane_id;
            }
        }
    }

    return 0;
}

//----------------------------------------------------------------------

uint32_t
drm::findDrmPropertyId(
    fd::FileDescriptor& fd,
    uint32_t objectId,
    uint32_t objectType,
    const std::string& name) noexcept
{
    auto properties{drmModeObjectGetProperties(fd, objectId, objectType)};

    if (!properties)
    {
        return 0;
    }

    for (auto i = 0U; i < properties->count_props; ++i)
    {
        auto property{drmModeGetProperty(fd, properties->props[i])};

        if (!property)
        {
            continue;
        }

        if (name == property->name)
        {
            return properties->props[i];
        }
    }

    return 0;
}

//----------------------------------------------------------------------

drm::FoundDrmResource
drm::findDrmResourcesForConnector(
    fd::FileDescriptor& fd,
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
                const uint32_t currentCrtc{1U << k};

                if (encoder->possible_crtcs & currentCrtc)
                {
                    const auto planeId{drm::findDrmPrimaryPlaneId(fd, currentCrtc)};
                    const auto currentCrtcId = resources->crtcs[k];
                    const auto crtc{drm::drmModeGetCrtc(fd, currentCrtcId)};

                    if ((crtc->mode.hdisplay > 0) and (crtc->mode.vdisplay > 0))
                    {
                        return FoundDrmResource{
                            .m_found = true,
                            .m_connectorId = connectorId,
                            .m_crtcId = currentCrtcId,
                            .m_planeId = planeId,
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

drm::FoundDrmResource
drm::findDrmResources(
    fd::FileDescriptor& fd,
    uint32_t connectorId) noexcept
{
    if (connectorId)
    {
        return findDrmResourcesForConnector(fd,
                                            connectorId,
                                            drm::drmModeGetResources(fd));
    }

    const auto resources = drm::drmModeGetResources(fd);

    for (int i = 0 ; i < resources->count_connectors ; ++i)
    {
        connectorId = resources->connectors[i];
        const auto resource{findDrmResourcesForConnector(fd,
                                                         connectorId,
                                                         resources)};

        if (resource.m_found)
        {
            return resource;
        }
    }

    return FoundDrmResource{ .m_found = false };
}

//-------------------------------------------------------------------------

int
drm::getModeCount(
    const std::string& card) noexcept
{
    fd::FileDescriptor fd{::open(card.c_str(), O_RDWR)};
    int modes{0};

    const auto resources = drm::drmModeGetResources(fd);

    for (auto i = 0 ; i < resources->count_connectors ; ++i)
    {
        const auto connectorId = resources->connectors[i];
        const auto connector = drm::drmModeGetConnector(fd, connectorId);
        const bool connected{connector->connection == DRM_MODE_CONNECTED};

        if (connected)
        {
            modes += connector->count_modes;
        }
    }

    return modes;
}

//-------------------------------------------------------------------------

bool
drm::setClientCap(
    fd::FileDescriptor& m_fd,
    uint64_t capability,
    uint64_t value) noexcept
{
    return (drmSetClientCap(m_fd.fd(), capability, value) == 0);
}

//-------------------------------------------------------------------------

bool
drm::setAtomicModeSetting(
    fd::FileDescriptor& m_fd) noexcept
{
    return setClientCap(m_fd, DRM_CLIENT_CAP_ATOMIC, 1);
}

//-------------------------------------------------------------------------

bool
drm::setUniversalPlanes(
    fd::FileDescriptor& m_fd) noexcept
{
    return setClientCap(m_fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);
}

