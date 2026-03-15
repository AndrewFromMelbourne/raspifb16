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

#pragma once

//-------------------------------------------------------------------------

#include <stdint.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include <array>
#include <memory>
#include <span>
#include <string>

#include "fileDescriptor.h"

//-------------------------------------------------------------------------

namespace drm
{

//-------------------------------------------------------------------------

using drmModeAtomicReq_ptr = std::unique_ptr<drmModeAtomicReq, decltype(&drmModeAtomicFree)>;
using drmModeConnector_ptr = std::unique_ptr<drmModeConnector, decltype(&drmModeFreeConnector)>;
using drmModeCrtc_ptr = std::unique_ptr<drmModeCrtc, decltype(&drmModeFreeCrtc)>;
using drmModeEncoder_ptr = std::unique_ptr<drmModeEncoder, decltype(&drmModeFreeEncoder)>;
using drmModeObjectProperties_ptr = std::unique_ptr<drmModeObjectProperties, decltype(&drmModeFreeObjectProperties)>;
using drmModePlane_ptr = std::unique_ptr<drmModePlane, decltype(&drmModeFreePlane)>;
using drmModePlaneRes_ptr = std::unique_ptr<drmModePlaneRes, decltype(&drmModeFreePlaneResources)>;
using drmModePropertyBlobRes_ptr = std::unique_ptr<drmModePropertyBlobRes, decltype(&drmModeFreePropertyBlob)>;
using drmModePropertyRes_ptr = std::unique_ptr<drmModePropertyRes, decltype(&drmModeFreeProperty)>;
using drmModeRes_ptr = std::unique_ptr<drmModeRes, decltype(&drmModeFreeResources)>;
using drmVersion_ptr = std::unique_ptr<drmVersion, decltype(&drmFreeVersion)>;

 //-------------------------------------------------------------------------

bool drmDeviceHasDumbBuffer(const std::string& device);

//-------------------------------------------------------------------------

class DrmDevices
{
public:

    static constexpr int c_maxDrmNodes{256};

    DrmDevices();
    ~DrmDevices();

    DrmDevices(const DrmDevices&) = delete;
    DrmDevices(DrmDevices&&) = delete;
    DrmDevices& operator=(const DrmDevices&) = delete;
    DrmDevices& operator=(DrmDevices&&) = delete;

    [[nodiscard]] drmDevicePtr getDevice(int index) const;
    [[nodiscard]] int getDeviceCount() const noexcept { return m_deviceCount; }

private:

    int m_deviceCount{};
    std::array<drmDevicePtr, c_maxDrmNodes> m_devices{};
};

//-------------------------------------------------------------------------

struct FoundDrmResource
{
    bool m_found{false};
    uint32_t m_connectorId{};
    uint32_t m_crtcId{};
    uint32_t m_planeId{};
    drmModeModeInfo m_mode{};
};

//-------------------------------------------------------------------------

drmModeAtomicReq_ptr drmModeAtomicAlloc() noexcept;
uint64_t drmGetPropertyValue(const fd::FileDescriptor& fd, uint32_t objectId, uint32_t objectType, const std::string& name) noexcept;
drmVersion_ptr drmGetVersion(const fd::FileDescriptor& fd) noexcept;
drmModeConnector_ptr drmModeGetConnector(const fd::FileDescriptor& fd, uint32_t connId) noexcept;
drmModeCrtc_ptr drmModeGetCrtc(const fd::FileDescriptor& fd, uint32_t crtcId) noexcept;
drmModeEncoder_ptr drmModeGetEncoder(const fd::FileDescriptor& fd, uint32_t encoderId) noexcept;
drmModeObjectProperties_ptr drmModeObjectGetProperties(const fd::FileDescriptor& fd, uint32_t objectId, uint32_t objectType) noexcept;
drmModePlane_ptr drmModeGetPlane(const fd::FileDescriptor& fd, uint32_t planeId) noexcept;
drmModePlaneRes_ptr drmModeGetPlaneResources(const fd::FileDescriptor& fd) noexcept;
drmModePropertyRes_ptr drmModeGetProperty(const fd::FileDescriptor& fd, uint32_t propertyId) noexcept;
drmModePropertyBlobRes_ptr drmModeGetPropertyBlob(const fd::FileDescriptor& fd, uint32_t blobId) noexcept;
drmModeRes_ptr drmModeGetResources(const fd::FileDescriptor& fd) noexcept;

//-------------------------------------------------------------------------

bool
addDrmPropertyToAtomicRequest(
    drmModeAtomicReq_ptr& atomicReq,
    const fd::FileDescriptor& fd,
    uint32_t objectId,
    uint32_t objectType,
    const std::string& propertyName,
    uint64_t value) noexcept;

bool drmDropMaster(const fd::FileDescriptor& fd) noexcept;
bool drmHandleEvent(const fd::FileDescriptor& fd, drmEventContext* ev) noexcept;
int drmIoctl(const fd::FileDescriptor& fd, unsigned long request, void *arg) noexcept;
[[nodiscard]] bool drmIsMaster(const fd::FileDescriptor& fd) noexcept;

int
drmModeAddFB2(
    const fd::FileDescriptor& fd,
    uint32_t width,
    uint32_t height,
	uint32_t pixel_format,
    const uint32_t bo_handles[4],
    const uint32_t pitches[4],
    const uint32_t offsets[4],
    uint32_t *buf_id,
    uint32_t flags) noexcept;

int drmModeAtomicCommit(const fd::FileDescriptor& fd, drmModeAtomicReq_ptr& req, uint32_t flags, void* user_data);
bool drmModeAtomicAddProperty(drmModeAtomicReq_ptr& atomicReq, uint32_t object_id, uint32_t property_id, uint64_t value) noexcept;
int drmModeCreatePropertyBlob(const fd::FileDescriptor& fd, const void *data, size_t size, uint32_t *id) noexcept;
int drmGetCap(const fd::FileDescriptor& fd, uint64_t capability, uint64_t *value) noexcept;
int drmModeDestroyPropertyBlob(const fd::FileDescriptor& fd, uint32_t id) noexcept;
int drmModePageFlip(const fd::FileDescriptor& fd, uint32_t crtc_id, uint32_t fb_id, uint32_t flags, void *user_data) noexcept;
bool drmModeRmFB(const fd::FileDescriptor& fd, uint32_t bufferId) noexcept;

int
drmModeSetCrtc(
    const fd::FileDescriptor& fd,
    uint32_t crtcId,
    uint32_t bufferId,
    uint32_t x,
    uint32_t y,
    uint32_t *connectors,
    int count,
    drmModeModeInfoPtr mode) noexcept;

int drmModeSetCrtc(const fd::FileDescriptor& fd, drmModeCrtc_ptr& crtcPtr, std::span<uint32_t> connectors) noexcept;

bool drmSetMaster(const fd::FileDescriptor& fd) noexcept;

std::string findDrmDevice() noexcept;
std::string findDrmDeviceWithConnector(uint32_t connectorId) noexcept;
std::string findDrmDevice(uint32_t connectorId);
uint32_t findDrmPrimaryPlaneId(const fd::FileDescriptor& fd, uint32_t crtcMask) noexcept;
uint32_t findDrmPropertyId(const fd::FileDescriptor& fd, uint32_t objectId, uint32_t objectType, const std::string& name) noexcept;
FoundDrmResource findDrmResourcesForConnector(const fd::FileDescriptor& fd, uint32_t connectorId, const drm::drmModeRes_ptr& resources) noexcept;
FoundDrmResource findDrmResources(const fd::FileDescriptor& fd, uint32_t connectorId) noexcept;
int getModeCount(const std::string& card) noexcept;
bool setClientCap(const fd::FileDescriptor& m_fd, uint64_t capability, uint64_t value) noexcept;
bool setAtomicModeSetting(const fd::FileDescriptor& m_fd) noexcept;
bool setUniversalPlanes(const fd::FileDescriptor& m_fd) noexcept;

//-------------------------------------------------------------------------

}

