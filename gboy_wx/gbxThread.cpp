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

#include <iostream>
#include <cassert>
#include "gbxThread.h"

DEFINE_EVENT_TYPE(wxEVT_GBX_SYNC)
DEFINE_EVENT_TYPE(wxEVT_GBX_SPEED)
DEFINE_EVENT_TYPE(wxEVT_GBX_LCD)

// ----------------------------------------------------------------------------
void ext_video_sync(void *data)
{
    ((gbxThread *)data)->PostVideoSync();
}

// ----------------------------------------------------------------------------
void ext_speed_change(void *data, int speed)
{
    ((gbxThread *)data)->PostSpeedChange(speed);
}

// ----------------------------------------------------------------------------
void ext_lcd_enabled(void *data, int enabled)
{
    ((gbxThread *)data)->PostLCDEnabled(enabled);
}

// ----------------------------------------------------------------------------
void ext_sound_write(void *data, uint16_t addr, uint8_t value)
{
}

// ----------------------------------------------------------------------------
void ext_sound_read(void *data, uint16_t addr, uint8_t *value)
{
}

// ----------------------------------------------------------------------------
gbxThread::gbxThread(wxEvtHandler *parent, int system)
: wxThread(wxTHREAD_JOINABLE), m_parent(parent), m_pauseCond(m_pauseLock),
  m_ctx(NULL), m_running(false), m_paused(false), m_throttle(true),
  m_debugger(false), m_error(0)
{
    if (gbx_create_context(&m_ctx, system))
        log_err("failed to create gbx context\n");

    gbx_set_userdata(m_ctx, this);
}

// ----------------------------------------------------------------------------
gbxThread::~gbxThread()
{
    assert(!m_running);

    if (m_ctx) {
        gbx_destroy_context(m_ctx);
        m_ctx = NULL;
    }
}

// ----------------------------------------------------------------------------
wxThread::ExitCode gbxThread::Entry()
{
    std::cerr << "Powering on system...\n";

    gbx_power_on(m_ctx);
    SetThrottleFrequency(CPU_FREQ_DMG);

    wxStopWatch ws;
    ws.Start();

    int turbo_ticks = 0;
    m_running = true;

    while (m_running) {
        // if paused, block until next pause toggle event is fired
        if (m_paused) {
            wxMutexLocker locker(m_pauseLock);
            m_pauseCond.Wait();
            continue;
        }

        m_cs.Enter();
        long startClock = ws.Time();
        gbx_execute_cycles(m_ctx, m_cyclesPerSec);
        float elapsed = (ws.Time() - startClock);
        m_cs.Leave();

        if (m_throttle)
            DynamicSleep(elapsed);
        else if (++turbo_ticks >= 50) {
            // give GUI thread a chance to access the CS every now and then
            turbo_ticks = 0;
            Sleep(1);
        }
    }

    return 0;
}

// ----------------------------------------------------------------------------
void gbxThread::SetThrottleFrequency(int hz)
{
    const float update_period_ms = 16.666667f;
    m_clockRate = (float)hz;
    m_cyclesPerSec = (int)(update_period_ms * m_clockRate / 1000.0f);
    m_realPeriod = m_cyclesPerSec * 1000.0f / m_clockRate;
}

// ----------------------------------------------------------------------------
void gbxThread::DynamicSleep(float elapsed)
{
    float delay_period = m_realPeriod - elapsed;
    if (delay_period <= 0) {
        // delay unnecessary if emulator can't perform at full speed
        m_error = 0.0f;
        return;
    }

    // compensate for millisecond precision (XXX: could be better)
    m_error += (delay_period - (int)delay_period);
    if (m_error >= 1.0f) {
        delay_period += (int)m_error;
        m_error -= (int)m_error;
    }

    Sleep((int)delay_period);
}

// ----------------------------------------------------------------------------
void gbxThread::PostVideoSync()
{
    gbx_get_framebuffer(m_ctx, m_framebuffer);
    wxCommandEvent event(wxEVT_GBX_SYNC, wxID_ANY);
    m_parent->AddPendingEvent(event);
}

// ----------------------------------------------------------------------------
void gbxThread::PostSpeedChange(int speed)
{
    wxCommandEvent event(wxEVT_GBX_SPEED, wxID_ANY);
    event.SetInt(speed);
    m_parent->AddPendingEvent(event);
    SetThrottleFrequency(speed ? CPU_FREQ_CGB : CPU_FREQ_DMG);
}

// ----------------------------------------------------------------------------
void gbxThread::PostLCDEnabled(int enabled)
{
    wxCommandEvent event(wxEVT_GBX_LCD, wxID_ANY);
    event.SetInt(enabled);
    m_parent->AddPendingEvent(event);
}

// ----------------------------------------------------------------------------
bool gbxThread::LoadFile(const wxString &path)
{
    wxCriticalSectionLocker locker(m_cs);
    return gbx_load_file(m_ctx, path.ToAscii()) == 0;
}

// ----------------------------------------------------------------------------
void gbxThread::SetBiosDir(const wxString &path)
{
    wxCriticalSectionLocker locker(m_cs);
    gbx_set_bios_dir(m_ctx, path.ToAscii());
}

// ----------------------------------------------------------------------------
bool gbxThread::SetPaused(bool paused)
{
    wxCriticalSectionLocker locker(m_cs);
    bool old_value = m_paused;
    m_paused = paused;
    
    // if the thread is running and blocked on pause, signal it to wake up
    wxMutexLocker pauseLocker(m_pauseLock);
    m_pauseCond.Broadcast();
    return old_value;
}

// ----------------------------------------------------------------------------
bool gbxThread::Paused() const
{
    wxCriticalSectionLocker locker(m_cs);
    return m_paused;
}

// ----------------------------------------------------------------------------
bool gbxThread::SetThrottleEnabled(bool throttle)
{
    wxCriticalSectionLocker locker(m_cs);
    bool old_value = m_throttle;

    m_throttle = throttle;
    return old_value;
}

// ----------------------------------------------------------------------------
bool gbxThread::SetDebuggerEnabled(bool enabled)
{
    wxCriticalSectionLocker locker(m_cs);
    bool old_value = m_debugger;

    m_debugger = enabled;
    gbx_set_debugger(m_ctx, enabled ? 1 : 0);
    return old_value;
}

// ----------------------------------------------------------------------------
void gbxThread::SetInputState(int key, int pressed)
{
    wxCriticalSectionLocker locker(m_cs);
    gbx_set_input_state(m_ctx, key, pressed ? 1 : 0);
}

// ----------------------------------------------------------------------------
const uint32_t *gbxThread::Framebuffer() const
{
    return m_framebuffer;
}

// ----------------------------------------------------------------------------
bool gbxThread::BatteryBacked() const
{
    wxCriticalSectionLocker locker(m_cs);
    return (gbx_get_cart_features(m_ctx) & CART_BATTERY) ? true : false;
}

// ----------------------------------------------------------------------------
bool gbxThread::DebuggerEnabled() const
{
    wxCriticalSectionLocker locker(m_cs);
    return m_debugger;
}

// ----------------------------------------------------------------------------
long gbxThread::CycleCount() const
{
    wxCriticalSectionLocker locker(m_cs);
    return gbx_get_cycle_count(m_ctx);
}

// ----------------------------------------------------------------------------
long gbxThread::ClockFrequency() const
{
    wxCriticalSectionLocker locker(m_cs);
    return gbx_get_clock_frequency(m_ctx);
}

// ----------------------------------------------------------------------------
void gbxThread::Reset()
{
    wxCriticalSectionLocker locker(m_cs);
    gbx_power_on(m_ctx);
}

// ----------------------------------------------------------------------------
void gbxThread::Terminate()
{
    // if the thread is blocked at shutdown, make sure it is unblocked here
    if (m_paused) {
        wxMutexLocker locker(m_pauseLock);
        m_pauseCond.Broadcast();
    }

    if (m_running) {
        m_running = false;
        Wait();
    }
}

