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

#ifndef GBOY_GBOYAPP__H
#define GBOY_GBOYAPP__H

// some helper macros to make the command line entry table more readable

#define cmd_help(s, l, d) { wxCMD_LINE_SWITCH, s, l, d,     \
                            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP }
#define cmd_optb(s, l, d) { wxCMD_LINE_SWITCH, s, l, d,     \
                            wxCMD_LINE_VAL_NONE }
#define cmd_opti(s, l, d) { wxCMD_LINE_OPTION, s, l, d,     \
                            wxCMD_LINE_VAL_NUMBER }
#define cmd_opts(s, l, d) { wxCMD_LINE_OPTION, s, l, d,     \
                            wxCMD_LINE_VAL_STRING }
#define cmd_parm(d)       { wxCMD_LINE_PARAM, 0, 0, d,      \
                            wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL }
#define cmd_term          { wxCMD_LINE_NONE }

class GboyApp: public wxApp
{
public:
    GboyApp();

    virtual bool OnInit();
    virtual int  OnExit();
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

    void LogMessage(int level, const wxString &msg);

protected:
    wxFrame *m_frame;
    wxString m_romfile;
    bool m_fullscreen;
    bool m_vsync;
    bool m_turbo;
    long m_scale;
    int m_system;
};

#endif // GBOY_GBOYAPP__H

