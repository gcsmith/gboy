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

#ifndef GBOY_RENDERBITMAP__H
#define GBOY_RENDERBITMAP__H

#include <wx/panel.h>
#include "RenderWidget.h"

class RenderBitmap: public RenderWidget, public wxPanel
{
public:
    RenderBitmap(wxWindow *parent);
    virtual ~RenderBitmap();

    virtual void UpdateFramebuffer(const uint32_t *fb);
    virtual void ClearFramebuffer(uint8_t value);

    virtual void SetStretchFilter(bool enable);
    virtual void SetFilterType(int index);
    virtual void SetScalingType(int index);
    virtual void SetSwapInterval(int interval);

    virtual bool StretchFilter() { return m_filterEnable; }
    virtual int FilterType() { return m_filterType; }
    virtual int ScalingType() { return m_scalingType; }
    virtual int SwapInterval() { return 0; }
    virtual wxWindow *Window() { return this; }

protected:
    void OnSize(wxSizeEvent &event);
    void OnPaint(wxPaintEvent &event);
    void OnEraseBackground(wxEraseEvent &event);

protected:
    wxBitmap *m_bmp;
    double m_xscale;
    double m_yscale;
    bool m_filterEnable;
    int m_filterType;
    int m_scalingType;
};

#endif // GBOY_RENDERBITMAP__H

