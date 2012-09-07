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

#ifndef GBOY_LOGGING__H
#define GBOY_LOGGING__H

#include <stdarg.h>
#include "common.h"

// log message severity levels

#define LOG_ERROR       0
#define LOG_INFO        1
#define LOG_WARNING     2
#define LOG_DEBUG       3
#define LOG_DEBUGSPEW   4
#define LOG_LAST        LOG_DEBUGSPEW

// logging routines that may be dynamically filtered at runtime

void log_vprintf(int level, const char *fmt, va_list arg);
void log_printf(int level, const char *fmt, ...);

// logging shortcuts that may be statically disabled at compile time

#ifdef ENABLE_LOG_ERROR
#define log_err(fmt, ...) log_printf(LOG_ERROR, fmt, ##__VA_ARGS__)
#else
#define log_err(fmt, ...)
#endif

#ifdef ENABLE_LOG_INFO
#define log_info(fmt, ...) log_printf(LOG_INFO, fmt, ##__VA_ARGS__)
#else
#define log_info(fmt, ...)
#endif

#ifdef ENABLE_LOG_WARNING
#define log_warn(fmt, ...) log_printf(LOG_WARNING, fmt, ##__VA_ARGS__)
#else
#define log_warn(fmt, ...)
#endif

#ifdef ENABLE_LOG_DEBUG
#define log_dbg(fmt, ...) log_printf(LOG_DEBUG, fmt, ##__VA_ARGS__)
#else
#define log_dbg(fmt, ...)
#endif

#ifdef ENABLE_LOG_DEBUGSPEW
#define log_spew(fmt, ...) log_printf(LOG_DEBUGSPEW, fmt, ##__VA_ARGS__)
#else
#define log_spew(fmt, ...)
#endif

#endif // GBOY_LOGGING__H

