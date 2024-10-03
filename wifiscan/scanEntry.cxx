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

#include <cstdio>
#include <string>

#include "scanEntry.h"

//-------------------------------------------------------------------------

ScanEntry:: ScanEntry()
:
    m_mac48("<mac48 not set>"),
    m_essid("<essid not set>"),
    m_frequency(0.0),
    m_channel(0),
    m_signalQuality(0.0),
    m_signalLevel(0.0)
{
}

//-------------------------------------------------------------------------

void
ScanEntry:: setMac48(
	const std::string& mac48)
{
	m_mac48 = mac48;
}

//-------------------------------------------------------------------------

void
ScanEntry:: setEssid(
	const std::string& essid)
{
	m_essid = essid;
}

//-------------------------------------------------------------------------

void
ScanEntry:: setFrequency(
	double frequency)
{
	m_frequency = frequency;
}

//-------------------------------------------------------------------------

void
ScanEntry:: setChannel(
	int channel)
{
	m_channel = channel;
}

//-------------------------------------------------------------------------

void
ScanEntry:: setSignalQuality(
	double signalQuality)
{
	m_signalQuality = signalQuality;
}

//-------------------------------------------------------------------------

void
ScanEntry:: setSignalLevel(
	double signalLevel)
{
	m_signalLevel = signalLevel;
}

//-------------------------------------------------------------------------

std::string
ScanEntry:: asString() const
{
	char buffer[100];

	sprintf(buffer,
			"%3.0f%% ch%03d %s %s",
			m_signalQuality,
			m_channel,
			m_mac48.c_str(),
	        m_essid.c_str());

	return buffer;
}

//-------------------------------------------------------------------------

void
ScanEntry:: clear()
{
    m_mac48 = "<mac48 not set>";
    m_essid = "<essid not set>";
    m_frequency = 0.0;
    m_channel = 0;
    m_signalQuality = 0.0;
    m_signalLevel = 0.0;
}

//-------------------------------------------------------------------------

bool
ScanEntry:: operator>(
	const ScanEntry& rhs) const
{
	return m_signalLevel > rhs.m_signalLevel;
}

