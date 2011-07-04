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

#include <GL/glew.h>
#ifdef PLATFORM_WIN32
#include <GL/wglew.h>
#elif defined(PLATFORM_UNIX)
#include <GL/glxew.h>
#endif
#include <wx/glcanvas.h>
#include "RenderWidget.h"

typedef struct vertex {
    float x, y, z;
    float s, t;
    float _padding[3];
} vertex_t;

// ----------------------------------------------------------------------------
// Return the nearest square, pow2 texture dimension >= MAX(width, height).
unsigned int tex_pow2(unsigned int width, unsigned int height)
{
    unsigned int input = MAX(width, height);
    unsigned int value = 2;

    if (0 == (input & (input - 1)))
        return input;

    while (0 != (input >>= 1))
        value <<= 1;
    return value;
}

class GL2Widget: public wxGLCanvas
{
public:
    GL2Widget(wxWindow *parent, wxGLContext *context,
              int *attrib, int width, int height);
    virtual ~GL2Widget();

    void UpdateFramebuffer(const uint32_t *fb);
    void ClearFramebuffer(uint8_t value);

    void SetStretchFilter(bool enable);
    void SetFilterType(int index);
    void SetScalingType(int index);
    void SetSwapInterval(int interval);

    bool StretchFilter() { return m_filterEnable; }
    int FilterType() { return m_filterType; }
    int ScalingType() { return m_scalingType; }
    int SwapInterval() { return m_swapInterval; }

protected:
    void InitGL();
    void UpdateDimensions();
    void ComputeAspectCorrectDimensions(int cx, int cy);

    void OnSize(wxSizeEvent &event);
    void OnPaint(wxPaintEvent &event);
    void OnEraseBackground(wxEraseEvent &event);

protected:
    wxGLContext *m_context;
    bool m_filterEnable;
    GLuint m_vbo;
    GLuint m_pbo;
    GLuint m_texture;
    int m_width, m_height;
    int m_pboSize;
    int m_textureDim;
    int m_filterType;
    int m_scalingType;
    int m_swapInterval;
    float m_aspect;
    float m_tu, m_tv;
    float m_x0, m_y0, m_x1, m_y1;
};

class RenderGL2: public RenderWidget
{
public:
    RenderGL2() : m_panel(NULL) { }
    virtual ~RenderGL2() { m_panel->Destroy(); }

    virtual void Create(wxWindow *parent, int width, int height) {
        int attrib_list[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };
        parent->Show(true); // XXX: must be visible to access context
        m_panel = new GL2Widget(parent, NULL, attrib_list, width, height);
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
    virtual void SetSwapInterval(int interval) {
        m_panel->SetSwapInterval(interval);
    }

    virtual bool StretchFilter() { return m_panel->StretchFilter(); }
    virtual int FilterType()     { return m_panel->FilterType(); }
    virtual int ScalingType()    { return m_panel->ScalingType(); }
    virtual int SwapInterval()   { return m_panel->SwapInterval(); }
    virtual wxWindow *Window()   { return m_panel; }

protected:
    GL2Widget *m_panel;
};

// ----------------------------------------------------------------------------
GL2Widget::GL2Widget(wxWindow *parent, wxGLContext *context,
                     int *attrib, int width, int height)
: wxGLCanvas(parent, wxID_ANY, attrib, wxDefaultPosition, wxDefaultSize,
  wxWANTS_CHARS), m_context(context), m_filterEnable(false), m_filterType(0),
  m_scalingType(0)
{
    if (!m_context)
        m_context = new wxGLContext(this);

    Connect(wxID_ANY, wxEVT_SIZE,
            wxSizeEventHandler(GL2Widget::OnSize));
    Connect(wxID_ANY, wxEVT_PAINT,
            wxPaintEventHandler(GL2Widget::OnPaint));
    Connect(wxID_ANY, wxEVT_ERASE_BACKGROUND,
            wxEraseEventHandler(GL2Widget::OnEraseBackground));

    m_width  = width;
    m_height = height;
    m_aspect = (float)m_width / m_height;

    SetCurrent(*m_context);
    InitGL();
}

// ----------------------------------------------------------------------------
GL2Widget::~GL2Widget()
{
    SetCurrent(*m_context);

    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_pbo);
    glDeleteTextures(1, &m_texture);
}

// ----------------------------------------------------------------------------
void GL2Widget::SetStretchFilter(bool enable)
{
    m_filterEnable = enable;
    GLint filter = enable ? GL_LINEAR : GL_NEAREST;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
}

// ----------------------------------------------------------------------------
void GL2Widget::SetFilterType(int index)
{
    m_filterType = index;
}

// ----------------------------------------------------------------------------
void GL2Widget::SetScalingType(int index)
{
    m_scalingType = index;
    UpdateDimensions();
}

// ----------------------------------------------------------------------------
void GL2Widget::SetSwapInterval(int interval)
{
    SetCurrent(*m_context);
    m_swapInterval = interval;

#ifdef PLATFORM_WIN32
    if (WGLEW_EXT_swap_control)
        wglSwapIntervalEXT(interval);
    else
        log_err("WGL_EXT_swap_control is not available\n");
#elif defined(PLATFORM_UNIX)
    if (GLXEW_SGI_swap_control)
        glXSwapIntervalSGI(interval);
    else
        log_err("GLX_SGI_swap_control is not available\n");
#else
    log_err("vertical sync is not available on this platform\n");
#endif 
}

// ----------------------------------------------------------------------------
void GL2Widget::UpdateFramebuffer(const uint32_t *fb)
{
    SetCurrent(*m_context);

    // lock the pbo for write only access and copy over the framebuffer
    glBufferData(GL_PIXEL_UNPACK_BUFFER, m_pboSize, NULL, GL_DYNAMIC_DRAW);
    void *pbuf = (void *)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

    memcpy(pbuf, (const void *)fb, m_pboSize);

    // unlock the pbo
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
            m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // force the widget to update its contents
    Refresh(false);
}

// ----------------------------------------------------------------------------
void GL2Widget::ClearFramebuffer(uint8_t value)
{
    SetCurrent(*m_context);

    // lock the pbo for write only access and copy over the framebuffer
    glBufferData(GL_PIXEL_UNPACK_BUFFER, m_pboSize, NULL, GL_DYNAMIC_DRAW);
    void *pbuf = (void *)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

    memset(pbuf, value, m_pboSize);

    // unlock the pbo
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
            m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // force the widget to update its contents
    Refresh(false);
}

// ----------------------------------------------------------------------------
void GL2Widget::InitGL()
{
    GLint rc = glewInit();
    if (GLEW_OK != rc)
        log_err("failed to initialize GLEW (%s)\n", glewGetErrorString(rc));

    m_pboSize = m_width * m_height * 4;
    m_textureDim = tex_pow2(m_width, m_height);

    m_tu = m_width  / (float)(m_textureDim + 1);
    m_tv = m_height / (float)(m_textureDim + 1);

    m_x0 = m_y0 = -1.0f;
    m_x1 = m_y1 = 1.0f;

    // set some reasonable defaults for rendering a 2d scene
    glClearColor(0, 0, 0, 0);

    glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // create the render texture and pbo
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            m_textureDim, m_textureDim, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glGenBuffers(1, &m_pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);

    // generate the vertex buffer
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * 4, NULL, GL_STREAM_DRAW);

    log_info("OpenGL2 renderer successfully initialized\n");

    SetSwapInterval(0);
    ClearFramebuffer(0);
    UpdateDimensions();
}

// ----------------------------------------------------------------------------
void GL2Widget::UpdateDimensions()
{
    int cx, cy;
    GetClientSize(&cx, &cy);

    SetCurrent(*m_context);
    glViewport(0, 0, (GLint)cx, (GLint)cy);

    m_x0 = m_y0 = -1.0f;
    m_x1 = m_y1 = 1.0f;

    if (m_scalingType == 1) {
        // fit the viewport while maintaining proper aspect ratio
        ComputeAspectCorrectDimensions(cx, cy);
    }
    else if (m_scalingType == 2) {
        // maintain aspect ratio and scale only by integer multiples
        float wnd_aspect = (float)cx / cy;
        int scale = wnd_aspect > m_aspect ? (cy / m_height) : (cx / m_width);
        if (scale > 0) {
            float sx = (scale * m_width) / (float)cx;
            float sy = (scale * m_height) / (float)cy;
            m_x0 = -sx; m_x1 = sx;
            m_y0 = -sy; m_y1 = sy;
        }
        else {
            // if viewport smaller than LCD resolution, just keep aspect
            ComputeAspectCorrectDimensions(cx, cy);
        }
    }
}

// ----------------------------------------------------------------------------
void GL2Widget::ComputeAspectCorrectDimensions(int cx, int cy)
{
    float wnd_aspect = (float)cx / cy;
    if (wnd_aspect > m_aspect) {
        float ratio = m_aspect / wnd_aspect;
        m_x0 = -ratio;
        m_x1 = ratio;
    }
    else {
        float ratio = wnd_aspect / m_aspect;
        m_y0 = -ratio;
        m_y1 = ratio;
    }
}

// ----------------------------------------------------------------------------
void GL2Widget::OnSize(wxSizeEvent &event)
{
    wxGLCanvas::OnSize(event);
    UpdateDimensions();
    Refresh(false);
}

// ----------------------------------------------------------------------------
void GL2Widget::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc(this);

    SetCurrent(*m_context);
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, m_tv); glVertex3f(m_x0, m_y0, 0.0f); 
    glTexCoord2f(m_tu, m_tv); glVertex3f(m_x1, m_y0, 0.0f); 
    glTexCoord2f(m_tu, 0.0f); glVertex3f(m_x1, m_y1, 0.0f); 
    glTexCoord2f(0.0f, 0.0f); glVertex3f(m_x0, m_y1, 0.0f);
    glEnd();

    SwapBuffers();
}

// ----------------------------------------------------------------------------
void GL2Widget::OnEraseBackground(wxEraseEvent &event)
{
    // override EraseBackground to avoid flickering on some platforms
}

// ----------------------------------------------------------------------------
RenderWidget *AllocateRenderGL2()
{
    return new RenderGL2();
}

