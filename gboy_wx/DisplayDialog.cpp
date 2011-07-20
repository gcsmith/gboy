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
#include "DisplayDialog.h"

// ----------------------------------------------------------------------------
DisplayDialog::DisplayDialog(wxWindow *parent)
: DisplayDialog_XRC(parent)
{
    m_outputModule->Append("Software");
    m_outputModule->Append("OpenGL");
    m_outputModule->SetSelection(0);

    m_filterType->Append("None");
    m_filterType->SetSelection(0);

    m_videoScaling->SetSelection(0);

    // give initial focus to the output module combo box
    m_outputModule->SetFocus();
}

// ----------------------------------------------------------------------------
void DisplayDialog::SetOutputModule(int index)
{
    m_outputModule->SetSelection(index);
}

// ----------------------------------------------------------------------------
void DisplayDialog::SetFilterType(int index)
{
    m_filterType->SetSelection(index);
}

// ----------------------------------------------------------------------------
void DisplayDialog::SetScalingType(int index)
{
    m_videoScaling->SetSelection(index);
}

// ----------------------------------------------------------------------------
void DisplayDialog::SetFilterEnabled(bool enabled)
{
    m_enableFilter->SetValue(enabled);
}

// ----------------------------------------------------------------------------
int DisplayDialog::OutputModule() const
{
    return m_outputModule->GetSelection();
}

// ----------------------------------------------------------------------------
int DisplayDialog::FilterType() const
{
    return m_filterType->GetSelection();
}

// ----------------------------------------------------------------------------
int DisplayDialog::ScalingType() const
{
    return m_videoScaling->GetSelection();
}

// ----------------------------------------------------------------------------
bool DisplayDialog::FilterEnabled() const
{
    return m_enableFilter->IsChecked();
}

