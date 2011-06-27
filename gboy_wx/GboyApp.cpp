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
#include <wx/cmdline.h>
#include "gbx.h"
#include "GboyApp.h"
#include "MainFrame.h"
#include "gbxThread.h"

IMPLEMENT_APP(GboyApp);

static const wxCmdLineEntryDesc g_cmdLineDesc [] = {
    { wxCMD_LINE_SWITCH, wxT("b"), wxT("bios-dir"),
        wxT("specify where bios files are located"),
        wxCMD_LINE_VAL_NONE },
    { wxCMD_LINE_SWITCH, wxT("d"), wxT("debugger"),
        wxT("enable debugging interface"),
        wxCMD_LINE_VAL_NONE },
    { wxCMD_LINE_SWITCH, wxT("f"), wxT("fullscreen"),
        wxT("run in fullscreen mode"),
        wxCMD_LINE_VAL_NONE },
    { wxCMD_LINE_SWITCH, wxT(""), wxT("log-serial"),
        wxT("log serial output to the specified file"),
        wxCMD_LINE_VAL_NONE },
    { wxCMD_LINE_SWITCH, wxT(""), wxT("no-sound"),
        wxT("disable sound playback"),
        wxCMD_LINE_VAL_NONE },
    { wxCMD_LINE_SWITCH, wxT("h"), wxT("help"),
        wxT("display this usage message"),
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_SWITCH, wxT(""), wxT("version"),
        wxT("display program version") },
    { wxCMD_LINE_NONE }
};

// ----------------------------------------------------------------------------
bool GboyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    wxConfig *config = new wxConfig(wxT("gboy_wx"));

    MainFrame *frame = new MainFrame(NULL, wxT("gboy"));
    frame->SetClientSize(wxSize(GBX_LCD_XRES << 1, GBX_LCD_YRES << 1));
    frame->SetConfig(config);
    frame->Show(true);

    SetTopWindow(frame);
    return true;
}

// ----------------------------------------------------------------------------
int GboyApp::OnExit()
{
    return 0;
}

// ----------------------------------------------------------------------------
void GboyApp::OnInitCmdLine(wxCmdLineParser &parser)
{
    parser.SetDesc(g_cmdLineDesc);
    parser.SetSwitchChars(wxT("-"));
}

// ----------------------------------------------------------------------------
bool GboyApp::OnCmdLineParsed(wxCmdLineParser &parser)
{
    return true;
}

