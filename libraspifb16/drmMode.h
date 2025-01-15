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
#include <string>

#include "fileDescriptor.h"

//-------------------------------------------------------------------------

namespace drm
{

//-------------------------------------------------------------------------

using drmModeConnector_ptr = std::unique_ptr<drmModeConnector, decltype(&drmModeFreeConnector)>;
using drmModeCrtc_ptr = std::unique_ptr<drmModeCrtc, decltype(&drmModeFreeCrtc)>;
using drmModeEncoder_ptr = std::unique_ptr<drmModeEncoder, decltype(&drmModeFreeEncoder)>;
using drmModeObjectProperties_ptr = std::unique_ptr<drmModeObjectProperties, decltype(&drmModeFreeObjectProperties)>;
using drmModePlane_ptr = std::unique_ptr<drmModePlane, decltype(&drmModeFreePlane)>;
using drmModePlaneRes_ptr = std::unique_ptr<drmModePlaneRes, decltype(&drmModeFreePlaneResources)>;
using drmModePropertyRes_ptr = std::unique_ptr<drmModePropertyRes, decltype(&drmModeFreeProperty)>;
using drmModeRes_ptr = std::unique_ptr<drmModeRes, decltype(&drmModeFreeResources)>;

 //-------------------------------------------------------------------------

bool drmDeviceHasDumbBuffer(const std::string& device);

//-------------------------------------------------------------------------

class DrmDevices
{
public:

    static constexpr int MaxDrmNodes{256};

    DrmDevices();
    ~DrmDevices();

    DrmDevices(const DrmDevices&) = delete;
    DrmDevices(DrmDevices&&) = delete;
    DrmDevices& operator=(const DrmDevices&) = delete;
    DrmDevices& operator=(DrmDevices&&) = delete;

    drmDevicePtr getDevice(int index) const;
    int getDeviceCount() const noexcept { return m_deviceCount; }

private:

    int m_deviceCount{};
    std::array<drmDevicePtr, MaxDrmNodes> m_devices{};
};

//-------------------------------------------------------------------------

drmModeConnector_ptr drmModeGetConnector(raspifb16::FileDescriptor& fd, uint32_t connId) noexcept;
drmModeCrtc_ptr drmModeGetCrtc(raspifb16::FileDescriptor& fd, uint32_t crtcId) noexcept;
drmModeEncoder_ptr drmModeGetEncoder(raspifb16::FileDescriptor& fd, uint32_t encoderId) noexcept;
drmModeObjectProperties_ptr drmModeObjectGetProperties(raspifb16::FileDescriptor& fd, uint32_t objectId, uint32_t objectType) noexcept;
drmModePlane_ptr drmModeGetPlane(raspifb16::FileDescriptor& fd, uint32_t planeId) noexcept;
drmModePlaneRes_ptr drmModeGetPlaneResources(raspifb16::FileDescriptor& fd) noexcept;
drmModePropertyRes_ptr drmModeGetProperty(raspifb16::FileDescriptor& fd, uint32_t propertyId) noexcept;
drmModeRes_ptr drmModeGetResources(raspifb16::FileDescriptor& fd) noexcept;
uint64_t drmGetPropertyValue(raspifb16::FileDescriptor& fd, uint32_t objectId, uint32_t objectType, const std::string& name) noexcept;

//-------------------------------------------------------------------------

}
