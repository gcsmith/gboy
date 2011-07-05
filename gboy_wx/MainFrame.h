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

#include <map>
#include <wx/config.h>
#include <wx/docview.h>
#include <wx/notebook.h>
#include <wx/xrc/xmlres.h>
#include "gbxThread.h"
#include "InputDialog.h"
#include "PrecisionTimer.h"
#include "RenderWidget.h"
#include "resource.h"

class MainFrame: public MainFrame_XRC
{
    typedef std::map<int, int> KeyMap;

public:
    MainFrame(wxWindow *parent, wxConfig *config, const wxChar *title);
    virtual ~MainFrame();

    void LoadFile(const wxString &path);

protected:
    void SetupStatusBar();
    void SetupToolBar();
    void SetupRecentList();
    void SetupEventHandlers();

    void LoadConfiguration();
    void SaveConfiguration();

    void CreateRenderWidget(int type);
    void CreateEmulatorContext();

    void SetStatusBarEnabled(bool enable);
    void SetToolBarEnabled(bool enable);
    void SetEmulatorEnabled(bool enable);
    void SetFullscreenEnabled(bool enable);
    void SetVsyncEnabled(bool enable);

    bool StatusBarEnabled() const {
        return GetMenuBar()->IsChecked(XRCID("menu_view_statusbar"));
    }
    bool ToolBarEnabled() const {
        return GetMenuBar()->IsChecked(XRCID("menu_view_toolbar"));
    }
    bool FullscreenEnabled() const {
        return GetMenuBar()->IsChecked(XRCID("menu_settings_fs"));
    }
    bool VsyncEnabled() const {
        return GetMenuBar()->IsChecked(XRCID("menu_settings_vsync"));
    }

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

    void OnDisplayDialog(wxCommandEvent &event);
    void OnInputDialog(wxCommandEvent &event);
    void OnSoundDialog(wxCommandEvent &event);

    void OnToggleStatusbar(wxCommandEvent &event);
    void OnToggleToolbar(wxCommandEvent &event);
    void OnToggleFullscreen(wxCommandEvent &event);
    void OnToggleVsync(wxCommandEvent &event);

    void OnReportBug(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);

    void OnGbxVideoSync(wxCommandEvent &event);
    void OnGbxSpeedChange(wxCommandEvent &event);
    void OnGbxLcdEnabled(wxCommandEvent &event);

    void OnKeyDown(wxKeyEvent &event);
    void OnKeyUp(wxKeyEvent &event);
    void OnPerfTimerTick(wxTimerEvent &event);
    void OnEraseBackground(wxEraseEvent &event);

protected:
    wxConfig      *m_config;
    wxFileHistory *m_recent;
    wxTimer       *m_perftimer;
    gbxThread     *m_gbx;
    RenderWidget  *m_render;
    KeyMap         m_keymap;
    bool           m_vsync;
    bool           m_filterEnable;
    long           m_lastCycles;
    int            m_outputModule;
    int            m_filterType;
    int            m_scalingType;
    PrecisionTimer m_timer;
};

#endif // GBOY_MAINFRAME__H

