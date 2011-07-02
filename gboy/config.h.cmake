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

#undef HAVE_INTRIN_H
#undef HAVE_STDINT_H

#cmakedefine HAVE_INTRIN_H
#cmakedefine HAVE_STDINT_H

#cmakedefine ENABLE_LOG_INFO
#cmakedefine ENABLE_LOG_ERR
#cmakedefine ENABLE_LOG_WARN
#cmakedefine ENABLE_LOG_DBG
#cmakedefine ENABLE_LOG_SPEW

#define GBOY_VER_MAJ  "@GBOY_VER_MAJ@"
#define GBOY_VER_MIN  "@GBOY_VER_MIN@"
#define GBOY_VER_REV  "@GBOY_VER_REV@"
#define GBOY_VER_HGID "@GBOY_VER_HGID@"

#define GBOY_VER_STR  GBOY_VER_MAJ "." GBOY_VER_MIN "." GBOY_VER_REV
#define GBOY_ID_STR   GBOY_VER_STR ":" GBOY_VER_HGID

