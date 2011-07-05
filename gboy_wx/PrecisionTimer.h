// gboy - a portable gameboy emulator
// Copyright (C) 2011  Garrett Smith.
// 
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
// 
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef GBOY_PRECISIONTIMER__H
#define GBOY_PRECISIONTIMER__H

#include "common.h"

#ifdef HAVE_CLOCK_GETTIME

// use clock_gettime for the highest resolution timing on UNIX platforms
#include <time.h>

class PrecisionTimer
{
public:
    typedef double Real;

    void Start() {
        clock_gettime(CLOCK_REALTIME, &m_last);
    }

    Real ElapsedNS() {
        timespec curr, delta;
        clock_gettime(CLOCK_REALTIME, &curr);

        if ((curr.tv_nsec - m_last.tv_nsec) < 0) {
            delta.tv_sec = curr.tv_sec - m_last.tv_sec - 1;
            delta.tv_nsec = 1000000000 + curr.tv_nsec - m_last.tv_nsec;
        }
        else {
            delta.tv_sec = curr.tv_sec - m_last.tv_sec;
            delta.tv_nsec = curr.tv_nsec - m_last.tv_nsec;
        }

        return (Real)(delta.tv_sec * 1000000000 + delta.tv_nsec);
    }

    Real ElapsedUS() {
        return ElapsedNS() / 1000;
    }

    Real ElapsedMS() {
        return ElapsedNS() / 1000000;
    }

    Real Elapsed() {
        return ElapsedNS() / 1000000000;
    }

protected:
    timespec m_last;
};

#elif defined(HAVE_WINDOWS_H)

// use QueryPerformanceCounter for the highest resolution timing on Windows
#include <windows.h>

class PrecisionTimer
{
public:
    typedef double Real;

    void Start()  {
        QueryPerformanceFrequency(&m_freq);
        QueryPerformanceCounter(&m_last);
    }

    Real ElapsedNS() {
        return Elapsed() * 1000000000;
    }

    Real ElapsedUS() {
        return Elapsed() * 1000000;
    }

    Real ElapsedMS() {
        return Elapsed() * 1000;
    }

    Real Elapsed() {
        LARGE_INTEGER curr;
        QueryPerformanceCounter(&curr);

        Real delta = curr.QuadPart - m_last.QuadPart;
        m_last = curr;

        return delta / m_freq.QuadPart;
    }

protected:
    LARGE_INTEGER m_freq;
    LARGE_INTEGER m_last;
};

#else

// if no high resolution timer is available, use generic wxWidgets timer
#include <wx/stopwatch.h>

class PrecisionTimer
{
public:
    typedef double Real;

    void Start()  {
        m_stopwatch.Start();
    }

    Real ElapsedNS() {
        return ElapsedMS() * 1000000;
    }

    Real ElapsedUS() {
        return ElapsedMS() * 1000;
    }

    Real ElapsedMS() {
        return (Real)m_stopwatch.Time();
    }

    Real Elapsed() {
        return ElapsedMS() / 1000;
    }

protected:
    wxStopWatch m_stopwatch;
};

#endif // HAVE_WINDOWS_H

#endif // GBOY_PRECISIONTIMER__H

