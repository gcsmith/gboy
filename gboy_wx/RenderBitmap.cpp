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

#include "gbx.h"
#include <wx/bitmap.h>
#include <wx/brush.h>
#include <wx/dcbuffer.h>
#include <wx/rawbmp.h>
#include "RenderWidget.h"

class BitmapWidget: public wxPanel
{
public:
    BitmapWidget(wxWindow *parent, int width, int height);
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

    virtual void Create(wxWindow *parent, int width, int height) {
        m_panel = new BitmapWidget(parent, width, height);
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
BitmapWidget::BitmapWidget(wxWindow *parent, int width, int height)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS),
  m_filterEnable(false), m_filterType(0), m_scalingType(0)
{
    m_width  = width;
    m_height = height;
    m_aspect = (float)m_width / m_height;

    m_bmp = new wxBitmap(m_width, m_height, 24);
    ClearFramebuffer(0);

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    SetBackgroundColour(wxColour(0, 0, 0));

    Bind(wxEVT_SIZE,  &BitmapWidget::OnSize,  this);
    Bind(wxEVT_PAINT, &BitmapWidget::OnPaint, this);

    // log_info("Software renderer successfully initialized\n");
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

    for ( int y = 0; y < m_height; ++y ) {
        wxNativePixelData::Iterator rowStart = p;
        for ( int x = 0; x < m_width; ++x, ++p ) {
            uint32_t color = fb[y * m_width + x];
            p.Red()   = color & 0xFF;
            p.Green() = (color >> 8) & 0xFF;
            p.Blue()  = (color >> 16) & 0xFF;
        }

        p = rowStart;
        p.OffsetY(data, 1);
    }

    Refresh(true);
}

// ----------------------------------------------------------------------------
void BitmapWidget::ClearFramebuffer(uint8_t value)
{
    wxNativePixelData data(*m_bmp);
    wxNativePixelData::Iterator p(data);

    for (int y = 0; y < m_height; ++y) {
        wxNativePixelData::Iterator rowStart = p;
        for (int x = 0; x < m_width; ++x, ++p) {
            p.Red()   = value;
            p.Green() = value;
            p.Blue()  = value;
        }

        p = rowStart;
        p.OffsetY(data, 1);
    }

    Refresh(true);
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
    Refresh(true);
}

// ----------------------------------------------------------------------------
void BitmapWidget::OnPaint(wxPaintEvent &event)
{
    wxAutoBufferedPaintDC dc(this);

    dc.SetBackground(*wxBLACK_BRUSH);
    dc.Clear();

    wxImage img(m_bmp->ConvertToImage());
    dc.DrawBitmap(wxBitmap(img.Scale(m_w, m_h)), m_x, m_y, false);
}

// ----------------------------------------------------------------------------
RenderWidget *AllocateRenderBitmap()
{
    return new RenderBitmap();
}

