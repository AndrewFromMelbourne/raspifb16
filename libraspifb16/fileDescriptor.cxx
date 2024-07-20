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

#include <unistd.h>

#include "fileDescriptor.h"

//-------------------------------------------------------------------------

raspifb16::FileDescriptor::FileDescriptor(
    int fd,
    CloseIfFunction close_if)
:
    m_fd{fd},
    m_close_if{close_if}
{
}

//-------------------------------------------------------------------------

raspifb16::FileDescriptor::~FileDescriptor()
{
    closeFd();
}

//-------------------------------------------------------------------------

raspifb16::FileDescriptor::FileDescriptor(
    raspifb16::FileDescriptor&& rhs)
:
    m_fd{rhs.m_fd},
    m_close_if{std::move(rhs.m_close_if)}
{
    rhs.m_fd = -1;
    rhs.m_close_if = [](int) { return false; };
}

//-------------------------------------------------------------------------

raspifb16::FileDescriptor&
raspifb16::FileDescriptor::operator= (
    raspifb16::FileDescriptor&& rhs)
{
    if (this != &rhs)
    {
        closeFd();

        m_fd = rhs.m_fd;
        m_close_if = std::move(rhs.m_close_if);

        rhs.m_fd = -1;
        rhs.m_close_if = [](int) { return false; };
    }

    return *this;
}

//-------------------------------------------------------------------------

void
raspifb16::FileDescriptor::closeFd()
{
    if (m_close_if(m_fd))
    {
        ::close(m_fd);
    }
}
