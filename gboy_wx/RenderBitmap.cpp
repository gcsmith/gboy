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
#include "RenderWidget.h"

class BitmapWidget: public wxPanel
{
public:
    BitmapWidget(wxWindow *parent);
    virtual ~BitmapWidget();

    void UpdateFramebuffer(const uint32_t *fb);
    void ClearFramebuffer(uint8_t value);

    void SetStretchFilter(bool enable) { m_filterEnable = enable; }
    void SetFilterType(int index) { m_filterType = index; }
    void SetScalingType(int index) { m_scalingType = index; }

    bool StretchFilter() { return m_filterEnable; }
    int FilterType() { return m_filterType; }
    int ScalingType() { return m_scalingType; }

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

class RenderBitmap: public RenderWidget
{
public:
    RenderBitmap() : m_panel(NULL) { }
    virtual ~RenderBitmap() { m_panel->Destroy(); }

    virtual void Create(wxWindow *parent) {
        m_panel = new BitmapWidget(parent);
    }
    virtual void UpdateFramebuffer(const uint32_t *fb) {
        m_panel->UpdateFramebuffer(fb);
    }
    virtual void ClearFramebuffer(uint8_t value) {
        m_panel->ClearFramebuffer(value);
    }
    virtual void SetStretchFilter(bool enable) {
        m_panel->SetStretchFilter(enable);
    }
    virtual void SetFilterType(int index) {
        m_panel->SetFilterType(index);
    }
    virtual void SetScalingType(int index) {
        m_panel->SetScalingType(index);
    }
    virtual void SetSwapInterval(int interval) { }

    virtual bool StretchFilter() { return m_panel->StretchFilter(); }
    virtual int FilterType()     { return m_panel->FilterType(); }
    virtual int ScalingType()    { return m_panel->ScalingType(); }
    virtual int SwapInterval()   { return 0; }
    virtual wxWindow *Window()   { return m_panel; }

protected:
    BitmapWidget *m_panel;
};

// ----------------------------------------------------------------------------
BitmapWidget::BitmapWidget(wxWindow *parent)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize), m_xscale(1.0),
  m_yscale(1.0), m_filterEnable(false), m_filterType(0), m_scalingType(0)
{
    m_bmp = new wxBitmap(GBX_LCD_XRES, GBX_LCD_YRES);
    ClearFramebuffer(0);

    Connect(wxID_ANY, wxEVT_SIZE,
            wxSizeEventHandler(BitmapWidget::OnSize));
    Connect(wxID_ANY, wxEVT_PAINT,
            wxPaintEventHandler(BitmapWidget::OnPaint));
    Connect(wxID_ANY, wxEVT_ERASE_BACKGROUND,
            wxEraseEventHandler(BitmapWidget::OnEraseBackground));

    log_info("Software renderer successfully initialized\n");
}

// ----------------------------------------------------------------------------
BitmapWidget::~BitmapWidget()
{
}

// ----------------------------------------------------------------------------
void BitmapWidget::UpdateFramebuffer(const uint32_t *fb)
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
void BitmapWidget::ClearFramebuffer(uint8_t value)
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
void BitmapWidget::OnSize(wxSizeEvent &event)
{
    int cx, cy;
    GetClientSize(&cx, &cy);

    m_xscale = (double)cx / GBX_LCD_XRES;
    m_yscale = (double)cy / GBX_LCD_YRES;

    Refresh(false);
}

// ----------------------------------------------------------------------------
void BitmapWidget::OnPaint(wxPaintEvent &event)
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
void BitmapWidget::OnEraseBackground(wxEraseEvent &event)
{
    // override EraseBackground to avoid flickering on some platforms
}

// ----------------------------------------------------------------------------
RenderWidget *AllocateRenderBitmap()
{
    return new RenderBitmap();
}

