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

#ifndef GBOY_RENDERGL2__H
#define GBOY_RENDERGL2__H

#include <wx/glcanvas.h>
#include "RenderWidget.h"

class RenderGL2: public RenderWidget, public wxGLCanvas
{
public:
    RenderGL2(wxWindow *parent, wxGLContext *context, int *attrib);
    virtual ~RenderGL2();

    virtual void SetStretchFilter(bool enable);
    virtual void SetSwapInterval(int interval);
    virtual void UpdateFramebuffer(const uint32_t *fb);
    virtual void ClearFramebuffer(uint8_t value);
    virtual wxWindow *Window() { return this; }

protected:
    void InitGL();
    void UpdateDimensions();

    void OnSize(wxSizeEvent &event);
    void OnPaint(wxPaintEvent &event);
    void OnEraseBackground(wxEraseEvent &event);

protected:
    wxGLContext *m_context;
    bool m_init;
    bool m_stretch;
    bool m_filter;
    GLuint m_vbo;
    GLuint m_pbo;
    GLuint m_texture;
    int m_width, m_height;
    int m_pboSize;
    int m_textureDim;
    float m_tu, m_tv;
    float m_x0, m_y0, m_x1, m_y1;
};

#endif // GBOY_RENDERGL2__H

