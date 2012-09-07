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

wxDEFINE_EVENT(wxEVT_GBX_LOG, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_GBX_SYNC, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_GBX_SPEED, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_GBX_LCD, wxThreadEvent);

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
  m_debugger(false), m_lcdEnabled(true), m_turboTicks(0), m_error(0)
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
    log_info("Powering on system...\n");
    gbx_power_on(m_ctx);

    static const int execute_cycles = 100000;
    SetThrottleFrequency(CPU_FREQ_DMG);

    m_running = true;
    while (m_running) {

        if (m_paused) {
            // if paused, block until next pause toggle event is fired
            wxMutexLocker locker(m_pauseLock);
            m_pauseCond.Wait();

            // reset the timer so there's no lurching when unpaused
            m_timer.Start();
            continue;
        }

        wxCriticalSectionLocker locker(m_cs);
        gbx_execute_cycles(m_ctx, execute_cycles);

        if (!m_lcdEnabled) {
            // sleep here when the LCD is disabled, as there will be no vblank
            DynamicSleep();
        }
    }

    return 0;
}

// ----------------------------------------------------------------------------
void gbxThread::SetThrottleFrequency(int hz)
{
    m_clockHz = (PrecisionTimer::Real)hz;
    m_lastCycles = gbx_get_cycle_count(m_ctx);
    m_timer.Start();
}

// ----------------------------------------------------------------------------
void gbxThread::DynamicSleep()
{
    long curr_cycles = gbx_get_cycle_count(m_ctx);

    m_cs.Leave();
    if (m_throttle) {
        long cycle_delta = curr_cycles - m_lastCycles;

        PrecisionTimer::Real elapsed_ms = m_timer.ElapsedMS();
        PrecisionTimer::Real target_ms = 1000 * cycle_delta / m_clockHz;
        PrecisionTimer::Real delay_ms = target_ms - elapsed_ms + m_error;

        if (delay_ms > 0) {
            m_timer.Start();
            wxMilliSleep((int)(delay_ms + 0.5f));
            m_error = delay_ms - m_timer.ElapsedMS();
        }
        else
            m_error = 0;
    }
    else if (++m_turboTicks > 50) {
        // sleep every now and then even if cpu throttling is disabled so
        // that the GUI thread isn't starved
        wxMilliSleep(1);
        m_turboTicks = 0;
    }
    m_cs.Enter();

    // reset the timer for the next iteration
    m_lastCycles = curr_cycles;
    m_timer.Start();
}

// ----------------------------------------------------------------------------
void gbxThread::PostVideoSync()
{
    gbx_get_framebuffer(m_ctx, m_framebuffer);
    wxThreadEvent evt(wxEVT_GBX_SYNC, wxID_ANY);
    wxQueueEvent(m_parent, evt.Clone());

    DynamicSleep();
}

// ----------------------------------------------------------------------------
void gbxThread::PostSpeedChange(int speed)
{
    wxThreadEvent evt(wxEVT_GBX_SPEED, wxID_ANY);
    evt.SetInt(speed);
    wxQueueEvent(m_parent, evt.Clone());

    SetThrottleFrequency(speed ? CPU_FREQ_CGB : CPU_FREQ_DMG);
}

// ----------------------------------------------------------------------------
void gbxThread::PostLCDEnabled(int enabled)
{
    wxThreadEvent evt(wxEVT_GBX_LCD, wxID_ANY);
    evt.SetInt(enabled);
    wxQueueEvent(m_parent, evt.Clone());

    m_lcdEnabled = (bool)enabled;
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
    // wxCriticalSectionLocker locker(m_cs);
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

    // XXX: is it really wise to do this here?
    m_parent->DeletePendingEvents();

    log_info("gbx thread terminated\n");
}

