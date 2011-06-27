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

#include <map>
#include "wx/wx.h"
#include "wx/glcanvas.h"
#include "gbxThread.h"

class RenderWidget: public wxGLCanvas
{
    typedef std::map<int, int> KeyMap;

public:
    RenderWidget(wxWindow *parent, wxGLContext *context, int *attrib);
    virtual ~RenderWidget();

    void SetEmulator(gbxThread *gbx);
    void SetSwapInterval(int interval);
    void UpdateFramebuffer(const uint32_t *fb);
    void ClearFramebuffer(uint8_t value);

protected:
    void InitGL();
    void OnSize(wxSizeEvent &event);
    void OnPaint(wxPaintEvent &event);
    void OnEraseBackground(wxEraseEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void OnKeyUp(wxKeyEvent &event);

protected:
    wxWindow *m_parent;
    wxGLContext *m_context;
    gbxThread *m_gbx;
    bool m_init;
    bool m_stretch;
    GLuint m_pbo;
    GLuint m_texture;
    int m_width, m_height;
    int m_pboSize;
    int m_textureDim;
    float m_tu, m_tv;
    float m_x0, m_y0, m_x1, m_y1;
    KeyMap m_keymap;
};

#endif // GBOY_RENDERWIDGET__H

