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

#ifndef GBOY_GBXTHREAD__H
#define GBOY_GBXTHREAD__H

#include <wx/wx.h>
#include "PrecisionTimer.h"
#include "gbx.h"

wxDECLARE_EVENT(wxEVT_GBX_LOG, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_GBX_SYNC, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_GBX_SPEED, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_GBX_LCD, wxThreadEvent);

class gbxThread: public wxThread
{
public:
    gbxThread(wxEvtHandler *parent, int system);
    virtual ~gbxThread();
    virtual ExitCode Entry();

    bool LoadFile(const wxString &path);
    void SetBiosDir(const wxString &path);
    bool SetPaused(bool paused);
    bool SetDebuggerEnabled(bool enabled);
    bool SetThrottleEnabled(bool throttle);
    void SetInputState(int key, int pressed);

    const uint32_t *Framebuffer() const;
    const wxString &BiosDir() const;
    bool Paused() const;
    bool BatteryBacked() const;
    bool DebuggerEnabled() const;
    long CycleCount() const;
    long ClockFrequency() const;

    void Reset();
    void Terminate();

protected:
    void SetThrottleFrequency(int hz);
    void DynamicSleep();

    void PostVideoSync();
    void PostSpeedChange(int speed);
    void PostLCDEnabled(int enabled);

    friend void ext_video_sync(void *data);
    friend void ext_speed_change(void *data, int speed);
    friend void ext_lcd_enabled(void *data, int enabled);

protected:
    wxEvtHandler *m_parent;
    wxMutex m_pauseLock;
    wxCondition m_pauseCond;
    mutable wxCriticalSection m_cs;
    gbx_context_t *m_ctx;
    uint32_t m_framebuffer[GBX_LCD_XRES * GBX_LCD_YRES];
    bool m_running;
    bool m_paused;
    bool m_throttle;
    bool m_debugger;
    bool m_lcdEnabled;
    long m_turboTicks;
    long m_lastCycles;
    long m_cyclesPerUpdate;
    PrecisionTimer m_timer;
    PrecisionTimer::Real m_clockHz;
    PrecisionTimer::Real m_error;
};

#endif // GBOY_GBXTHREAD__H

