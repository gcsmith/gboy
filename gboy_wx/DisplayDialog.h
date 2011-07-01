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

#ifndef GBOY_DISPLAYDIALOG__H
#define GBOY_DISPLAYDIALOG__H

#include "resource.h"

class DisplayDialog: public DisplayDialog_XRC
{
public:
    DisplayDialog(wxWindow *parent);

    void SetOutpoutModule(int index);
    void SetFilterType(int index);
    void SetScalingType(int index);
    void SetFilterEnabled(bool enabled);

    int OutputModule() const;
    int FilterType() const;
    int ScalingType() const;
    bool FilterEnabled() const;
};

#endif // GBOY_DISPLAYDIALOG__H
