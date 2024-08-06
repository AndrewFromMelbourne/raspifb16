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

#include <cstdlib>
#include <string>
#include <system_error>

#include "interface565Factory.h"

#ifdef LIBDRM_INSTALLED
#include "dumbbuffer565.h"
#endif

#include "framebuffer565.h"

//-------------------------------------------------------------------------

namespace
{
const std::string defaultFrameBufferDevice{"/dev/fb1"};
}

//-------------------------------------------------------------------------

namespace raspifb16
{

//-------------------------------------------------------------------------

std::unique_ptr<Interface565>
createInterface565(
    InterfaceType565 type,
    const std::string& device)
{
    std::string interfaceDevice = device;

    switch (type)
    {
    case InterfaceType565::FRAME_BUFFER_565:

        if (interfaceDevice.empty())
        {
            interfaceDevice = defaultFrameBufferDevice;
        }

        return std::make_unique<FrameBuffer565>(interfaceDevice);

        break;

    case InterfaceType565::KMSDRM_DUMB_BUFFER_565:

#ifdef LIBDRM_INSTALLED
    {
        uint32_t connectorId{0};

        const auto connectorString = std::getenv("RASPIFB16_DRM_CONNECTOR");

        if (connectorString)
        {
            try
            {
                connectorId = std::stol(connectorString);
            }
            catch(...)
            {
                // do nothing
            }
        }

        return std::make_unique<DumbBuffer565>(interfaceDevice, connectorId);
    }
#else
        throw std::invalid_argument("There is no KMSDRM library installed");
#endif

        break;
    };

    return nullptr;
}

//-------------------------------------------------------------------------

} // namespace raspifb16
