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

#include <wx/wx.h>
#include "InputDialog.h"
#include "gbx.h"

// ----------------------------------------------------------------------------
InputDialog::InputDialog(wxWindow *parent)
: InputDialog_XRC(parent)
{
    SetupField(m_upEdit,     INPUT_UP);
    SetupField(m_downEdit,   INPUT_DOWN);
    SetupField(m_leftEdit,   INPUT_LEFT);
    SetupField(m_rightEdit,  INPUT_RIGHT);
    SetupField(m_AEdit,      INPUT_A);
    SetupField(m_BEdit,      INPUT_B);
    SetupField(m_selectEdit, INPUT_SELECT);
    SetupField(m_startEdit,  INPUT_START);

    m_joypadPage->SetFocus();
}

// ----------------------------------------------------------------------------
void InputDialog::SetKeyMappings(std::map<int, int> &keymap)
{
    std::map<int, int>::const_iterator i;
    for (i = keymap.begin(); i != keymap.end(); ++i) {
        if (i->second >= NUM_INPUTS) {
            assert(!"invalid input index in InputDialog::SetKeyMappings");
            continue;
        }

        m_fieldData[i->second].keycode = i->first;
        m_fields[i->second]->SetValue(KeyToString(i->first));
    }
}

// ----------------------------------------------------------------------------
void InputDialog::GetKeyMappings(std::map<int, int> &keymap)
{
    keymap.clear();
    for (int i = 0; i < NUM_INPUTS; i++) {
        // skip input indices that were cleared / left unmapped
        if (-1 == m_fieldData[i].keycode)
            continue;

        if (keymap.end() != keymap.find(m_fieldData[i].keycode)) {
            assert(!"duplicate keycode in InputDialog::GetKeyMappings");
            continue;
        }

        keymap[m_fieldData[i].keycode] = m_fieldData[i].input_index;
    }
}

// ----------------------------------------------------------------------------
void InputDialog::SetupField(wxTextCtrl *field, int input_index)
{
    field->Connect(wxID_ANY, wxEVT_KEY_DOWN,
            wxKeyEventHandler(InputDialog::OnKeyDown), NULL, this);
    field->Connect(wxID_ANY, wxEVT_KEY_UP,
            wxKeyEventHandler(InputDialog::OnKeyUp), NULL, this);
    field->Connect(wxID_ANY, wxEVT_SET_FOCUS,
            wxFocusEventHandler(InputDialog::OnSetFocus), NULL, this);
    field->Connect(wxID_ANY, wxEVT_KILL_FOCUS,
            wxFocusEventHandler(InputDialog::OnKillFocus), NULL, this);

    m_fieldData[input_index].input_index = input_index;
    m_fieldData[input_index].keycode = -1;

    field->SetClientData((void *)&m_fieldData[input_index]);
    m_fields[input_index] = field;
}

// ----------------------------------------------------------------------------
wxString InputDialog::KeyToString(int keycode)
{
    if (keycode == WXK_ALT)
        return "ALT";
    else if (keycode == WXK_CONTROL)
        return "CONTROL";
    else if (keycode == WXK_SHIFT)
        return "SHIFT";
    else {
        wxAcceleratorEntry accel(0, keycode);
        return accel.ToString();
    }
}

// ----------------------------------------------------------------------------
void InputDialog::OnSetFocus(wxFocusEvent &event)
{
    wxTextCtrl *child = (wxTextCtrl *)event.GetEventObject();
    child->SetValue("<press key>");
}

// ----------------------------------------------------------------------------
void InputDialog::OnKillFocus(wxFocusEvent &event)
{
    wxTextCtrl *child = (wxTextCtrl *)event.GetEventObject();
    TextCtrlData *data = (TextCtrlData *)child->GetClientData();

    if (-1 != data->keycode)
        child->SetValue(KeyToString(data->keycode));
    else
        child->SetValue("");
}

// ----------------------------------------------------------------------------
void InputDialog::OnKeyDown(wxKeyEvent &event)
{
    wxTextCtrl *child = (wxTextCtrl *)event.GetEventObject();
    TextCtrlData *data = (TextCtrlData *)child->GetClientData();

    int keycode = event.GetKeyCode();
    if (keycode != WXK_ESCAPE) {
        // escape key map not be mapped, use to skip to next field
        child->SetValue(KeyToString(keycode));
        data->keycode = keycode;

        // scan fields for duplicate assignments and clear them
        for (int i = 0; i < NUM_INPUTS; i++) {
            if (i != data->input_index && m_fieldData[i].keycode == keycode) {
                m_fields[i]->SetValue("");
                m_fieldData[i].keycode = -1;
            }
        }
    }

    child->Navigate();
}

// ----------------------------------------------------------------------------
void InputDialog::OnKeyUp(wxKeyEvent &event)
{
}

