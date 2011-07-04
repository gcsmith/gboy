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

#ifndef GBOY_INPUTDIALOG__H
#define GBOY_INPUTDIALOG__H

#include <map>
#include "resource.h"

#define NUM_INPUTS 8

class InputDialog: public InputDialog_XRC
{
public:
    InputDialog(wxWindow *parent);

    void SetKeyMappings(std::map<int, int> &keymap);
    void GetKeyMappings(std::map<int, int> &keymap);

protected:
    void SetupField(wxTextCtrl *field, int key);
    wxString KeyToString(int keycode);

    void OnSetFocus(wxFocusEvent &event);
    void OnKillFocus(wxFocusEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void OnKeyUp(wxKeyEvent &event);

protected:
    struct TextCtrlData {
        int input_index;
        int keycode;
    };

    TextCtrlData m_fieldData[NUM_INPUTS];
    wxTextCtrl *m_fields[NUM_INPUTS];
};

#endif // GBOY_INPUTDIALOG__H

