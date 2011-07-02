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

#ifndef GBOY_RENDERWIDGET__H
#define GBOY_RENDERWIDGET__H

#include <wx/wx.h>

class RenderWidget
{
public:
    virtual void UpdateFramebuffer(const uint32_t *fb) = 0;
    virtual void ClearFramebuffer(uint8_t value) = 0;

    virtual void SetStretchFilter(bool enable) = 0;
    virtual void SetFilterType(int index) = 0;
    virtual void SetScalingType(int index) = 0;
    virtual void SetSwapInterval(int interval) = 0;

    virtual bool StretchFilter() = 0;
    virtual int FilterType() = 0;
    virtual int ScalingType() = 0;
    virtual int SwapInterval() = 0;
    virtual wxWindow *Window() = 0;
};

#endif // GBOY_RENDERWIDGET__H

