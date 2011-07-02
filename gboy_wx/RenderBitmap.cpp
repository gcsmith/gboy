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

#include <wx/bitmap.h>
#include <wx/rawbmp.h>
#include "gbx.h"
#include "RenderBitmap.h"

// ----------------------------------------------------------------------------
RenderBitmap::RenderBitmap(wxWindow *parent)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize), m_xscale(1.0),
  m_yscale(1.0), m_filterEnable(false), m_filterType(0), m_scalingType(0)
{
    m_bmp = new wxBitmap(GBX_LCD_XRES, GBX_LCD_YRES);
    ClearFramebuffer(0);

    Connect(wxID_ANY, wxEVT_SIZE,
            wxSizeEventHandler(RenderBitmap::OnSize));
    Connect(wxID_ANY, wxEVT_PAINT,
            wxPaintEventHandler(RenderBitmap::OnPaint));
    Connect(wxID_ANY, wxEVT_ERASE_BACKGROUND,
            wxEraseEventHandler(RenderBitmap::OnEraseBackground));

    log_info("Software renderer successfully initialized\n");
}

// ----------------------------------------------------------------------------
RenderBitmap::~RenderBitmap()
{
}

// ----------------------------------------------------------------------------
void RenderBitmap::SetStretchFilter(bool enable)
{
    m_filterEnable = enable;
}

// ----------------------------------------------------------------------------
void RenderBitmap::SetFilterType(int index)
{
    m_filterType = index;
}

// ----------------------------------------------------------------------------
void RenderBitmap::SetScalingType(int index)
{
    m_scalingType = index;
}

// ----------------------------------------------------------------------------
void RenderBitmap::SetSwapInterval(int interval)
{
    log_info("RenderBitmap::SetSwapInterval - not supported\n");
}

// ----------------------------------------------------------------------------
void RenderBitmap::UpdateFramebuffer(const uint32_t *fb)
{
    wxNativePixelData data(*m_bmp);
    wxNativePixelData::Iterator p(data);

    for ( int y = 0; y < GBX_LCD_YRES; ++y ) {
        wxNativePixelData::Iterator rowStart = p;
        for ( int x = 0; x < GBX_LCD_XRES; ++x, ++p ) {
            uint32_t color = fb[y * GBX_LCD_XRES + x];
            p.Red()   = color & 0xFF;
            p.Green() = (color >> 8) & 0xFF;
            p.Blue()  = (color >> 16) & 0xFF;
        }

        p = rowStart;
        p.OffsetY(data, 1);
    }

    Refresh(false);
}

// ----------------------------------------------------------------------------
void RenderBitmap::ClearFramebuffer(uint8_t value)
{
    wxNativePixelData data(*m_bmp);
    wxNativePixelData::Iterator p(data);

    for (int y = 0; y < GBX_LCD_YRES; ++y) {
        wxNativePixelData::Iterator rowStart = p;
        for (int x = 0; x < GBX_LCD_XRES; ++x, ++p) {
            p.Red()   = value;
            p.Green() = value;
            p.Blue()  = value;
        }

        p = rowStart;
        p.OffsetY(data, 1);
    }

    Refresh(false);
}

// ----------------------------------------------------------------------------
void RenderBitmap::OnSize(wxSizeEvent &event)
{
    int cx, cy;
    GetClientSize(&cx, &cy);

    m_xscale = (double)cx / GBX_LCD_XRES;
    m_yscale = (double)cy / GBX_LCD_YRES;

    Refresh(false);
}

// ----------------------------------------------------------------------------
void RenderBitmap::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc(this);

    if (m_filterEnable) {
        dc.SetUserScale(m_xscale, m_yscale);
        dc.DrawBitmap(*m_bmp, 0, 0, false);
    }
    else {
        int cx, cy;
        GetClientSize(&cx, &cy);

        wxImage img(m_bmp->ConvertToImage());
        dc.DrawBitmap(wxBitmap(img.Scale(cx, cy)), 0, 0, false);
    }
}

// ----------------------------------------------------------------------------
void RenderBitmap::OnEraseBackground(wxEraseEvent &event)
{
    // override EraseBackground to avoid flickering on some platforms
}

