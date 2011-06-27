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

#include "wx/wx.h"
#include "MainFrame.h"

// ----------------------------------------------------------------------------
MainFrame::MainFrame(wxWindow *parent, const wxChar *title)
: wxFrame(NULL, -1, title), m_config(NULL), m_recent(NULL), m_lastCycles(0)
{
    SetupStatusBar();
    SetupMainMenu();
    SetupEventHandlers();

    int attrib_list[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };
    m_render = new RenderWidget(this, NULL, attrib_list);

    CreateEmulatorContext();
    EmulatorEnabled(false);

    // create a timer firing at one second intervals to report cycles/second
    m_perftimer = new wxTimer(this);
    Connect(m_perftimer->GetId(), wxEVT_TIMER,
            wxTimerEventHandler(MainFrame::OnPerfTimerTick));
}

// ----------------------------------------------------------------------------
MainFrame::~MainFrame()
{
    if (m_perftimer) {
        m_perftimer->Stop();
        SAFE_DELETE(m_perftimer);
    }

    if (m_gbx) {
        m_gbx->Terminate();
        SAFE_DELETE(m_gbx);
    }

    if (m_config) {
        m_config->SetPath(wxT("/mru"));
        m_recent->Save(*m_config);

        SAFE_DELETE(m_recent);
        SAFE_DELETE(m_config);
    }
}

// ----------------------------------------------------------------------------
void MainFrame::SetConfig(wxConfig *config)
{
    const int MruListSize = 10;

    SAFE_DELETE(m_config);
    m_config = config;

    if (m_recent) {
        m_recent->RemoveMenu(m_recentMenu);
        SAFE_DELETE(m_recent);
    }

    m_recent = new wxFileHistory(MruListSize);
    m_recent->UseMenu(m_recentMenu);
    m_recent->AddFilesToMenu();

    m_config->SetPath(wxT("/mru"));
    m_recent->Load(*m_config);

    wxWindowID base = m_recent->GetBaseId();
    Connect(base, base + MruListSize, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnRecentOpen));
}

// ----------------------------------------------------------------------------
void MainFrame::SetupStatusBar()
{
    int widths[] = { -1, 120 };

    CreateStatusBar(2);
    SetStatusWidths(2, widths);
}

// ----------------------------------------------------------------------------
void MainFrame::SetupMainMenu()
{
    m_recentMenu = new wxMenu();
    m_recentMenu->Append(ID_RECENT_CLEAR,
            wxT("&Clear Recent List"), wxT("Clear recent file list"));
    m_recentMenu->AppendCheckItem(ID_RECENT_LOCK,
            wxT("&Lock Recent List"), wxT("Lock recent file list"));

    m_slotMenu = new wxMenu();
    for (int i = 0; i < 10; i++) {
        wxString item = wxString::Format(wxT("Slot %d"), i);
        wxString help = wxString::Format(wxT("Select quick state slot %d"), i);
        m_slotMenu->AppendRadioItem(ID_FILE_SAVESLOT + i, item, help);
    }

    m_fileMenu = new wxMenu();
    m_fileMenu->Append(wxID_OPEN,
            wxT("&Open...\tCtrl+O"), wxT("Select an image file to load"));
    m_fileMenu->AppendSubMenu(m_recentMenu,
            wxT("Open &Recent"), wxT("Select a recently loaded image file"));
    m_fileMenu->AppendSeparator();
    m_fileMenu->Append(ID_FILE_LOADSTATE,
            wxT("&Load State...\tF3"), wxT("Load save state from file"));
    m_fileMenu->Append(ID_FILE_SAVESTATE,
            wxT("&Save State...\tF4"), wxT("Save save state to file"));
    m_fileMenu->AppendSeparator();
    m_fileMenu->Append(ID_FILE_LOADQUICKSTATE,
            wxT("&Load Quick State\tF5"), wxT("Load from current state slot"));
    m_fileMenu->Append(ID_FILE_SAVEQUICKSTATE,
            wxT("&Save Quick State\tF6"), wxT("Save to current state slot"));
    m_fileMenu->AppendSubMenu(m_slotMenu,
            wxT("Select Quick State"), wxT("Select current quick state slot"));
    m_fileMenu->AppendSeparator();
    m_fileMenu->Append(wxID_EXIT,
            wxT("&Quit"), wxT("Quit the application"));

    m_machineMenu = new wxMenu();
    m_machineMenu->Append(ID_MACHINE_RESET,
            wxT("&Reset\tCtrl+R"), wxT("Reset emulator"));
    m_machineMenu->AppendCheckItem(ID_MACHINE_PAUSE,
            wxT("&Pause\tCtrl+P"), wxT("Pause emulator execution"));
    m_machineMenu->AppendCheckItem(ID_MACHINE_TURBO,
            wxT("&Turbo\tCtrl+T"), wxT("Disable CPU throttling"));
    m_machineMenu->Append(ID_MACHINE_STEP,
            wxT("Frame &Step\tCtrl+F"), wxT("Single step through frames"));

    m_settingsMenu = new wxMenu();
    m_settingsMenu->Append(ID_SETTINGS_INPUT,
            wxT("&Input..."), wxT("Display input settings dialog"));
    m_settingsMenu->Append(ID_SETTINGS_SOUND,
            wxT("&Sound..."), wxT("Display sound settings dialog"));
    m_settingsMenu->Append(ID_SETTINGS_VIDEO,
            wxT("&Video..."), wxT("Display video settings dialog"));
    m_settingsMenu->AppendSeparator();
    m_settingsMenu->AppendCheckItem(ID_SETTINGS_FS,
            wxT("Full Screen\tAlt+Return"), wxT("Toggle full screen mode"));
    m_settingsMenu->AppendCheckItem(ID_SETTINGS_VSYNC,
            wxT("Vertical Sync"), wxT("Toggle vertical sync enable"));

    m_viewMenu = new wxMenu();
    m_viewMenu->AppendCheckItem(ID_VIEW_STATUSBAR,
            wxT("&Statusbar"), wxT("Toggle statusbar display"));
    m_viewMenu->AppendCheckItem(ID_VIEW_TOOLBAR,
            wxT("&Toolbar"), wxT("Toggle toolbar display"));

    m_helpMenu = new wxMenu();
    m_helpMenu->Append(ID_HELP_REPORTBUG,
            wxT("&Report Bug"), wxT("Open browser to issue tracker"));
    m_helpMenu->Append(wxID_ABOUT,
            wxT("&About"), wxT("Display gboy about dialog"));

    // add each sub-menu to the main window menu bar
    m_menuBar = new wxMenuBar();
    m_menuBar->Append(m_fileMenu, wxT("&File"));
    m_menuBar->Append(m_machineMenu, wxT("&Machine"));
    m_menuBar->Append(m_settingsMenu, wxT("&Settings"));
    m_menuBar->Append(m_viewMenu, wxT("&View"));
    m_menuBar->Append(m_helpMenu, wxT("&Help"));

    // set default check state for each checkable menu item
    m_menuBar->Check(ID_SETTINGS_FS, false);
    m_menuBar->Check(ID_SETTINGS_VSYNC, false);
    m_menuBar->Check(ID_VIEW_STATUSBAR, true);
    m_menuBar->Check(ID_VIEW_TOOLBAR, false);

    SetMenuBar(m_menuBar);
}

// ----------------------------------------------------------------------------
void MainFrame::SetupEventHandlers()
{
    Connect(wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnOpen));
    Connect(ID_FILE_LOADSTATE, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnLoadState));
    Connect(ID_FILE_SAVESTATE, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnSaveState));
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnQuit));
    Connect(ID_RECENT_CLEAR, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnRecentClear));

    Connect(ID_MACHINE_RESET, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnMachineReset));
    Connect(ID_MACHINE_PAUSE, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnMachineTogglePause));
    Connect(ID_MACHINE_TURBO, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnMachineToggleTurbo));
    Connect(ID_MACHINE_STEP, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnMachineStep));

    Connect(ID_SETTINGS_FS, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnToggleFullscreen));
    Connect(ID_SETTINGS_VSYNC, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnToggleVsync));

    Connect(ID_VIEW_STATUSBAR, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnToggleStatusbar));
    Connect(ID_VIEW_TOOLBAR, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnToggleToolbar));

    Connect(ID_HELP_REPORTBUG, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnReportBug));
    Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MainFrame::OnAbout));

    Connect(wxID_ANY, wxEVT_GBX_SYNC,
            wxCommandEventHandler(MainFrame::OnGbxVideoSync));
    Connect(wxID_ANY, wxEVT_GBX_SPEED,
            wxCommandEventHandler(MainFrame::OnGbxSpeedChange));
    Connect(wxID_ANY, wxEVT_GBX_LCD,
            wxCommandEventHandler(MainFrame::OnGbxLcdEnabled));

    Connect(wxID_ANY, wxEVT_ERASE_BACKGROUND,
            wxEraseEventHandler(MainFrame::OnEraseBackground));
}

// ----------------------------------------------------------------------------
void MainFrame::LoadFile(const wxString &path)
{
    if (m_gbx->IsRunning())
        CreateEmulatorContext();

    m_gbx->LoadFile(path);
    m_gbx->Run();

    m_lastCycles = 0;
    m_perftimer->Start(1000, false);
    EmulatorEnabled(true);

    if (!GetMenuBar()->IsChecked(ID_RECENT_LOCK)) {
        // add the selected file to the MRU list (unless list is locked)
        m_recent->AddFileToHistory(path);
    }
}

// ----------------------------------------------------------------------------
void MainFrame::CreateEmulatorContext()
{
    if (m_gbx) {
        m_gbx->Terminate();
        delete m_gbx;
    }

    m_gbx = new gbxThread(this, SYSTEM_AUTO);
    if (wxTHREAD_NO_ERROR != m_gbx->Create())
        wxLogError(wxT("Failed to create gbx thread!"));

    m_render->SetEmulator(m_gbx);
}

// ----------------------------------------------------------------------------
void MainFrame::EmulatorEnabled(bool enable)
{
    GetMenuBar()->Enable(ID_MACHINE_RESET, enable);
    GetMenuBar()->Enable(ID_MACHINE_PAUSE, enable);
    GetMenuBar()->Enable(ID_MACHINE_TURBO, enable);
    GetMenuBar()->Enable(ID_MACHINE_STEP, enable);

    GetMenuBar()->Check(ID_MACHINE_PAUSE, false);
    GetMenuBar()->Check(ID_MACHINE_TURBO, false);
    
    SetStatusText(enable ? wxT("Running") : wxT("Stopped"), 0);
}

// ----------------------------------------------------------------------------
void MainFrame::OnOpen(wxCommandEvent &event)
{
    static const wxChar *SupportedFileTypes = wxT(
            "Game Boy Images|*.dmg;*.gb;*.gbc;*.cgb;*.sgb|"
            "DMG Game Boy Images|*.dmg;*.gb|"
            "Color Game Boy Images|*.gbc;*.cgb|"
            "All Files|*.*");

    wxFileDialog *fd = new wxFileDialog(this, wxT("Open"),
            wxT(""), wxT(""), SupportedFileTypes, wxOPEN);

    if (wxID_OK == fd->ShowModal()) {
        LoadFile(fd->GetPath());
    }
}

// ----------------------------------------------------------------------------
void MainFrame::OnRecentOpen(wxCommandEvent &event)
{
    int index = event.GetId() - m_recent->GetBaseId();
    LoadFile(m_recent->GetHistoryFile(index));
}

// ----------------------------------------------------------------------------
void MainFrame::OnRecentClear(wxCommandEvent &event)
{
    while (m_recent->GetCount())
        m_recent->RemoveFileFromHistory(0);
}

// ----------------------------------------------------------------------------
void MainFrame::OnLoadState(wxCommandEvent &event)
{
    wxMessageBox(wxT("TODO"), wxT("Load State"), wxOK | wxICON_ERROR, this);
}

// ----------------------------------------------------------------------------
void MainFrame::OnSaveState(wxCommandEvent &event)
{
    wxMessageBox(wxT("TODO"), wxT("Save State"), wxOK | wxICON_ERROR, this);
}

// ----------------------------------------------------------------------------
void MainFrame::OnQuit(wxCommandEvent &event)
{
    Close(true);
}

// ----------------------------------------------------------------------------
void MainFrame::OnMachineReset(wxCommandEvent &event)
{
    m_gbx->Reset();
}

// ----------------------------------------------------------------------------
void MainFrame::OnMachineTogglePause(wxCommandEvent &event)
{
    bool paused = GetMenuBar()->IsChecked(ID_MACHINE_PAUSE);
    m_gbx->SetPaused(paused);
    m_lastCycles = 0;

    if (paused) {
        log_info("Emulator paused.\n");
        SetStatusText(wxT("Paused"), 0);
        m_perftimer->Stop();
    }
    else {
        log_info("Emulator unpaused.\n");
        SetStatusText(wxT("Running"), 0);
        m_perftimer->Start();
    }
}

// ----------------------------------------------------------------------------
void MainFrame::OnMachineToggleTurbo(wxCommandEvent &event)
{
    bool turbo = GetMenuBar()->IsChecked(ID_MACHINE_TURBO);
    m_gbx->SetThrottleEnabled(!turbo);
}

// ----------------------------------------------------------------------------
void MainFrame::OnMachineStep(wxCommandEvent &event)
{
    wxMessageBox(wxT("TODO"), wxT("Frame Step"), wxOK | wxICON_ERROR, this);
}

// ----------------------------------------------------------------------------
void MainFrame::OnToggleStatusbar(wxCommandEvent &event)
{
    GetStatusBar()->Show(GetMenuBar()->IsChecked(ID_VIEW_STATUSBAR));
}

// ----------------------------------------------------------------------------
void MainFrame::OnToggleToolbar(wxCommandEvent &event)
{
}

// ----------------------------------------------------------------------------
void MainFrame::OnToggleFullscreen(wxCommandEvent &event)
{
    bool fullscreen = GetMenuBar()->IsChecked(ID_SETTINGS_FS);

    log_info("Full screen %s.\n", fullscreen ? "enabled" : "disabled");
    ShowFullScreen(fullscreen);
}

// ----------------------------------------------------------------------------
void MainFrame::OnToggleVsync(wxCommandEvent &event)
{
    bool vsync = GetMenuBar()->IsChecked(ID_SETTINGS_VSYNC);

    log_info("Vertical sync %s.\n", vsync ? "enabled" : "disabled");
    m_render->SetSwapInterval(vsync ? 1 : 0);
}

// ----------------------------------------------------------------------------
void MainFrame::OnReportBug(wxCommandEvent &event)
{
    wxMessageBox(wxT("TODO"), wxT("Report Bug"), wxOK | wxICON_ERROR, this);
}

// ----------------------------------------------------------------------------
void MainFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox(wxT("gboy - a simple, cross platform game boy emulator\n"
                     "v0.1-dev\n\n"
                     "Garrett Smith 2011"),
                 wxT("About gboy"), wxOK | wxICON_INFORMATION, this);
}

// ----------------------------------------------------------------------------
void MainFrame::OnGbxVideoSync(wxCommandEvent &event)
{
    m_render->UpdateFramebuffer(m_gbx->Framebuffer());
}

// ----------------------------------------------------------------------------
void MainFrame::OnGbxSpeedChange(wxCommandEvent &event)
{
    log_err("TODO: OnGbxSpeedChange\n");
}

// ----------------------------------------------------------------------------
void MainFrame::OnGbxLcdEnabled(wxCommandEvent &event)
{
    if (event.GetInt() == 0)
        m_render->ClearFramebuffer(0xFF);
}

// ----------------------------------------------------------------------------
void MainFrame::OnPerfTimerTick(wxTimerEvent &event)
{
    if (m_lastCycles <= 0) {
        m_lastCycles = m_gbx->CycleCount();
        return;
    }

    long cycles = m_gbx->CycleCount();
    float cps = cycles - m_lastCycles;
    m_lastCycles = cycles;

    float mhz = cps / 1000000.0f;
    int percent = (int)(0.5f + 100.0f * cps / CPU_FREQ_DMG);
    wxString label = wxString::Format(wxT("%.2f MHz / %d%%"), mhz, percent);
    SetStatusText(label, 1);
}

// ----------------------------------------------------------------------------
void MainFrame::OnEraseBackground(wxEraseEvent &event)
{
    // override EraseBackground to avoid flickering on some platforms
}

