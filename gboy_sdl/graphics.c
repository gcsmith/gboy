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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "graphics.h"

// -----------------------------------------------------------------------------
graphics_t *graphics_init(int width, int height, int stretch)
{
    graphics_t *gfx = (graphics_t *)calloc(1, sizeof(graphics_t));
    gfx->width = width;
    gfx->height = height;
    gfx->stretch = stretch;

    gfx->pbo_sz = width * height * 4;
    gfx->tex_dim = tex_pow2(width, height);

    gfx->tex_u = gfx->width / (float)gfx->tex_dim;
    gfx->tex_v = gfx->height / (float)gfx->tex_dim;

    gfx->tex_x0 = gfx->tex_y0 = -1.0f;
    gfx->tex_x1 = gfx->tex_y1 = 1.0f;

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
    glGenTextures(1, &gfx->texture);
    glBindTexture(GL_TEXTURE_2D, gfx->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            gfx->tex_dim, gfx->tex_dim, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glGenBuffers(1, &gfx->pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, gfx->pbo);

    return gfx;
}

// -----------------------------------------------------------------------------
void graphics_resize(graphics_t *gfx, int width, int height)
{
    glViewport(0, 0, width, height);

    if (gfx->stretch) {
        // stretch the image to fit the entire viewport
        gfx->tex_x0 = gfx->tex_y0 = -1.0f;
        gfx->tex_x1 = gfx->tex_y1 = 1.0f;
    }
    else {
        // fit the viewport while maintaining proper aspect ratio
        float wnd_aspect = (float)width / height;
        float gbx_aspect = (float)gfx->width / gfx->height;

        if (wnd_aspect > gbx_aspect) {
            float ratio = gbx_aspect / wnd_aspect;
            gfx->tex_y0 = -1.0f;
            gfx->tex_y1 = 1.0f;
            gfx->tex_x0 = -ratio;
            gfx->tex_x1 = ratio;
        }
        else {
            float ratio = wnd_aspect / gbx_aspect;
            gfx->tex_y0 = -ratio;
            gfx->tex_y1 = ratio;
            gfx->tex_x0 = -1.0f;
            gfx->tex_x1 = 1.0f;
        }
    }
}

// -----------------------------------------------------------------------------
void graphics_update(graphics_t *gfx, uint32_t *src)
{
    uint32_t *pbuf;

    // lock the pbo for write only access and copy over the framebuffer
    glBufferData(GL_PIXEL_UNPACK_BUFFER, gfx->pbo_sz, NULL, GL_DYNAMIC_DRAW);
    pbuf = (uint32_t *)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

    memcpy(pbuf, src, gfx->pbo_sz);

    // unlock the pbo
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
            gfx->width, gfx->height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

// -----------------------------------------------------------------------------
void graphics_render(graphics_t *gfx)
{
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: replace this with a vbo
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, gfx->tex_v);
    glVertex3f(gfx->tex_x0, gfx->tex_y0, 0.0f); 
    glTexCoord2f(gfx->tex_u, gfx->tex_v);
    glVertex3f(gfx->tex_x1, gfx->tex_y0, 0.0f); 
    glTexCoord2f(gfx->tex_u, 0.0f);
    glVertex3f(gfx->tex_x1, gfx->tex_y1, 0.0f); 
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(gfx->tex_x0, gfx->tex_y1, 0.0f);
    glEnd();
}

// -----------------------------------------------------------------------------
void graphics_shutdown(graphics_t *gfx)
{
    glDeleteBuffers(1, &gfx->pbo);
    glDeleteTextures(1, &gfx->texture);
    SAFE_FREE(gfx);
}

