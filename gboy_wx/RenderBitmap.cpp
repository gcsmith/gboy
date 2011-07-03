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

    void SetStretchFilter(bool enable);
    void SetFilterType(int index);
    void SetScalingType(int index);

    bool StretchFilter() { return m_filterEnable; }
    int FilterType() { return m_filterType; }
    int ScalingType() { return m_scalingType; }

protected:
    void UpdateDimensions();
    void ComputeAspectCorrectDimensions(int cx, int cy);
    void OnSize(wxSizeEvent &event);
    void OnPaint(wxPaintEvent &event);

protected:
    wxBitmap *m_bmp;
    double m_xscale;
    double m_yscale;
    bool m_filterEnable;
    int m_filterType;
    int m_scalingType;
    int m_x, m_y, m_w, m_h;
    int m_width, m_height;
    float m_aspect;
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
    m_bmp = new wxBitmap(GBX_LCD_XRES, GBX_LCD_YRES, 24);
    ClearFramebuffer(0);
    SetBackgroundColour(wxColour(0, 0, 0));

    Connect(wxID_ANY, wxEVT_SIZE,
            wxSizeEventHandler(BitmapWidget::OnSize));
    Connect(wxID_ANY, wxEVT_PAINT,
            wxPaintEventHandler(BitmapWidget::OnPaint));

    m_width  = GBX_LCD_XRES;
    m_height = GBX_LCD_YRES;
    m_aspect = (float)m_width / m_height;

    log_info("Software renderer successfully initialized\n");
}

// ----------------------------------------------------------------------------
BitmapWidget::~BitmapWidget()
{
}

// ----------------------------------------------------------------------------
void BitmapWidget::SetStretchFilter(bool enable)
{
    m_filterEnable = enable;
}

// ----------------------------------------------------------------------------
void BitmapWidget::SetFilterType(int index)
{
    m_filterType = index;
}

// ----------------------------------------------------------------------------
void BitmapWidget::SetScalingType(int index)
{
    m_scalingType = index;
    UpdateDimensions();
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
void BitmapWidget::UpdateDimensions()
{
    int cx, cy;
    GetClientSize(&cx, &cy);

    m_x = m_y = 0;
    m_w = cx;
    m_h = cy;

    if (m_scalingType == 1) {
        // fit the viewport while maintaining proper aspect ratio
        ComputeAspectCorrectDimensions(cx, cy);
    }
    else if (m_scalingType == 2) {
        // maintain aspect ratio and scale only by integer multiples
        float wnd_aspect = (float)cx / cy;
        int scale = wnd_aspect > m_aspect ? (cy / m_height) : (cx / m_width);
        if (scale > 0) {
            m_w = scale * m_width;
            m_h = scale * m_height;
            m_x = (cx - m_w) / 2;
            m_y = (cy - m_h) / 2;
        }
        else {
            // if viewport smaller than LCD resolution, just keep aspect
            ComputeAspectCorrectDimensions(cx, cy);
        }
    }

    m_xscale = (double)m_w / m_width;
    m_yscale = (double)m_h / m_height;
}

// ----------------------------------------------------------------------------
void BitmapWidget::ComputeAspectCorrectDimensions(int cx, int cy)
{
    float wnd_aspect = (float)cx / cy;
    if (wnd_aspect > m_aspect) {
        m_w = (int)(cy * m_aspect);
        m_h = cy;
        m_x = (cx - m_w) / 2;
    }
    else {
        m_w = cx;
        m_h = (int)(cx / m_aspect);
        m_y = (cy - m_h) / 2;
    }
}

// ----------------------------------------------------------------------------
void BitmapWidget::OnSize(wxSizeEvent &event)
{
    UpdateDimensions();
    Refresh(false);
}

// ----------------------------------------------------------------------------
void BitmapWidget::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc(this);

    if (m_filterEnable) {
        // this scaling method seems to apply filter under GTK but not windows
        dc.SetUserScale(m_xscale, m_yscale);
        int x = (int)(m_x / m_xscale);
        int y = (int)(m_y / m_yscale);
        dc.DrawBitmap(*m_bmp, x, y, false);
    }
    else {
        wxImage img(m_bmp->ConvertToImage());
        dc.DrawBitmap(wxBitmap(img.Scale(m_w, m_h)), m_x, m_y, false);
    }
}

// ----------------------------------------------------------------------------
RenderWidget *AllocateRenderBitmap()
{
    return new RenderBitmap();
}

