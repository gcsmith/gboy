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

#ifndef GBOY_MAINFRAME__H
#define GBOY_MAINFRAME__H

#include <wx/config.h>
#include <wx/docview.h>
#include "gbxThread.h"
#include "RenderWidget.h"

enum
{
    ID_FILE_RECENT = wxID_HIGHEST + 1, 
    ID_FILE_LOADSTATE,
    ID_FILE_SAVESTATE,
    ID_FILE_LOADQUICKSTATE,
    ID_FILE_SAVEQUICKSTATE,

    ID_FILE_SAVESLOT,
    ID_FILE_SAVESLOT_LAST = ID_FILE_SAVESLOT + 9,

    ID_RECENT_CLEAR,
    ID_RECENT_LOCK,

    ID_MACHINE_RESET,
    ID_MACHINE_PAUSE,
    ID_MACHINE_TURBO,
    ID_MACHINE_STEP,

    ID_SETTINGS_INPUT,
    ID_SETTINGS_SOUND,
    ID_SETTINGS_VIDEO,
    ID_SETTINGS_FS,
    ID_SETTINGS_VSYNC,

    ID_VIEW_STATUSBAR,
    ID_VIEW_TOOLBAR,
    
    ID_HELP_REPORTBUG,
};

class MainFrame: public wxFrame
{
public:
    MainFrame(wxWindow *parent, const wxChar *title);
    virtual ~MainFrame();

    void SetConfig(wxConfig *config);
    void SetEmulatorThread(gbxThread *gbx);

protected:
    void SetupStatusBar();
    void SetupMainMenu();
    void SetupEventHandlers();

    void LoadFile(const wxString &path);
    void CreateEmulatorContext();
    void EmulatorEnabled(bool enable);

    void OnOpen(wxCommandEvent &event);
    void OnLoadState(wxCommandEvent &event);
    void OnSaveState(wxCommandEvent &event);
    void OnQuit(wxCommandEvent &event);

    void OnRecentOpen(wxCommandEvent &event);
    void OnRecentClear(wxCommandEvent &event);

    void OnMachineReset(wxCommandEvent &event);
    void OnMachineTogglePause(wxCommandEvent &event);
    void OnMachineToggleTurbo(wxCommandEvent &event);
    void OnMachineStep(wxCommandEvent &event);

    void OnToggleStatusbar(wxCommandEvent &event);
    void OnToggleToolbar(wxCommandEvent &event);
    void OnToggleFullscreen(wxCommandEvent &event);
    void OnToggleVsync(wxCommandEvent &event);

    void OnReportBug(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);

    void OnGbxVideoSync(wxCommandEvent &event);
    void OnGbxSpeedChange(wxCommandEvent &event);
    void OnGbxLcdEnabled(wxCommandEvent &event);

    void OnPerfTimerTick(wxTimerEvent &event);
    void OnEraseBackground(wxEraseEvent &event);

protected:
    wxMenuBar     *m_menuBar;
    wxMenu        *m_fileMenu;
    wxMenu        *m_recentMenu;
    wxMenu        *m_slotMenu;
    wxMenu        *m_machineMenu;
    wxMenu        *m_settingsMenu;
    wxMenu        *m_viewMenu;
    wxMenu        *m_helpMenu;
    wxConfig      *m_config;
    wxFileHistory *m_recent;
    wxTimer       *m_perftimer;
    gbxThread     *m_gbx;
    RenderWidget  *m_render;
    long           m_lastCycles;
};

#endif // GBOY_MAINFRAME__H

