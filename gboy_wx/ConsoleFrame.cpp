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

#include "common.h"
#include <wx/wx.h>
#include "ConsoleFrame.h"

// ----------------------------------------------------------------------------
ConsoleFrame::ConsoleFrame(wxWindow *parent)
: ConsoleFrame_XRC(parent)
{
    m_text = new wxStyledTextCtrl(this, wxID_ANY);
    m_text->SetReadOnly(true);
    m_text->SetMarginWidth(0, 50);
    m_text->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_text->SetWrapMode(wxSTC_WRAP_WORD);

    wxFont monoFont(10,
            wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxColour bgColor(220, 220, 220);

    m_text->StyleSetBackground(wxSTC_STYLE_LINENUMBER, bgColor);
    m_text->StyleSetFont(wxSTC_STYLE_DEFAULT, monoFont);

    Bind(wxEVT_CLOSE_WINDOW, &ConsoleFrame::OnCloseWindow, this);

    WriteLn("gboy - a portable gameboy emulator");
    WriteLn("version " GBOY_ID_STR);
}

// ----------------------------------------------------------------------------
ConsoleFrame::~ConsoleFrame()
{
}

// ----------------------------------------------------------------------------
void ConsoleFrame::OnCloseWindow(wxCloseEvent &evt)
{
    if (evt.CanVeto()) {
        Show(false);
    }
    else {
        evt.Skip();
    }
}

// ----------------------------------------------------------------------------
void ConsoleFrame::Write(const wxString &msg)
{
    m_text->Freeze();
    m_text->SetReadOnly(false);
    m_text->AppendText(msg);
    m_text->ScrollToLine(m_text->GetLineCount());
    m_text->SetReadOnly(true);
    m_text->Thaw();
}

// ----------------------------------------------------------------------------
void ConsoleFrame::WriteLn(const wxString &msg)
{
    Write(msg + "\n");
}

