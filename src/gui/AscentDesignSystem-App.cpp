/***************************************************************************
This file is part of the Shuttle Ascent Design System.

The Shuttle Ascent Design System is free software: you can redistribute it
and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

The Shuttle Ascent Design System is distributed in the hope that it will
be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public Licensealong with
the Shuttle Ascent Design System. If not, see <https://www.gnu.org/licenses/>.

**************************************************************************/

#include "AscentDesignSystem-App.h"
#include <wx/tokenzr.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>

enum
{
    wxID_combo_Launchpad = 1,
    ID_Button_Calculate,
    ID_Button_HA_Converge,
    ID_Button_HP_Converge,
    ID_Button_Export
};

bool AscentDesignSystemApp::OnInit()
{
	// Create the main application window
	AscentDesignSystemFrame* frame = new AscentDesignSystemFrame("Ascent Design System");
	// Show it
	frame->Show(true);
	// Start the event loop
	return true;
}

// Event table for AscentDesignSystemFrame
BEGIN_EVENT_TABLE(AscentDesignSystemFrame, wxFrame)
EVT_MENU(wxID_NEW, AscentDesignSystemFrame::OnNew)
EVT_MENU(wxID_OPEN, AscentDesignSystemFrame::OnOpen)
EVT_MENU(wxID_SAVE, AscentDesignSystemFrame::OnSave)
EVT_MENU(wxID_EXIT, AscentDesignSystemFrame::OnQuit)
EVT_MENU(wxID_ABOUT, AscentDesignSystemFrame::OnAbout)
EVT_CHOICE(wxID_combo_Launchpad, AscentDesignSystemFrame::OnCombo_Launchpad)
EVT_BUTTON(ID_Button_Calculate, AscentDesignSystemFrame::OnButtonCalculate)
EVT_BUTTON(ID_Button_HA_Converge, AscentDesignSystemFrame::OnButtonHAConverge)
EVT_BUTTON(ID_Button_HP_Converge, AscentDesignSystemFrame::OnButtonHPConverge)
EVT_BUTTON(ID_Button_Export, AscentDesignSystemFrame::OnButtonExport)
END_EVENT_TABLE()

AscentDesignSystemFrame::AscentDesignSystemFrame(const wxString& title)
	: wxFrame(NULL, wxID_ANY, title)
{

    // Create a menu bar
    wxMenu* fileMenu = new wxMenu;

    // The “About” item should be in the help menu
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, "&About...\tF1",
        "Show about dialog");

    //fileMenu->Append(wxID_NEW, wxT("New"), wxT(""));
    //fileMenu->AppendSeparator();
    fileMenu->Append(wxID_OPEN, "&Open...\tCtrl+O", "");
    fileMenu->Append(wxID_SAVE, wxT("&Save...\tCtrl+S"), wxT(""));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X",
        "Quit this program");

    // Now append the freshly created menu to the menu bar...
    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

	// Create a top-level panel to hold all the contents of the frame
	wxPanel* panel = new wxPanel(this, wxID_ANY);

    // Set up the sizer for the frame and resize the frame
    // according to its contents
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->SetMinSize(1280, 720);
    topSizer->Add(panel, 1, wxEXPAND);
    SetSizerAndFit(topSizer);

    // Create a status bar just for fun
    CreateStatusBar(1);
    SetStatusText(wxT("Welcome to the Ascent Design System!"));

    wxArrayString strings;
	int minX, minY, diffX, diffY, Y, difftext, diffunit;

    minX = 10;
    minY = 10;
    diffX = 70;
    diffY = 26;// 32;
    difftext = 4;
    diffunit = 200;

	Y = minY;

    new wxStaticText(panel, wxID_ANY, "LAUNCH SITE", wxPoint(minX, Y + difftext));
    strings.Add(wxT("LC-39A"));
    strings.Add(wxT("LC-39B"));
    strings.Add(wxT("SLC-6"));
    Add(combo_Launchpad = new wxChoice(panel, wxID_combo_Launchpad, wxPoint(minX + diffX + 40, Y), wxDefaultSize, strings), "/General/Launchpad");
    combo_Launchpad->SetToolTip(wxT("Launchpad"));
    combo_Launchpad->SetSelection(0);
    strings.clear();
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "LATLS", wxPoint(minX, Y + difftext));
    Add(text_LATLS = new wxTextCtrl(panel, wxID_ANY, "28:36:30", wxPoint(minX + diffX, Y)), "/General/LATLS");
    text_LATLS->SetToolTip(wxT("Geocentric latitude of launch site"));
    new wxStaticText(panel, wxID_ANY, "deg", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "LONGLS", wxPoint(minX, Y + difftext));
    Add(text_LONGLS = new wxTextCtrl(panel, wxID_ANY, "279:23:45", wxPoint(minX + diffX, Y)), "/General/LNGLS");
    text_LONGLS->SetToolTip(wxT("Geographic longitude of launch site"));
    new wxStaticText(panel, wxID_ANY, "deg", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "PFA", wxPoint(minX, Y + difftext));
    Add(text_PFA = new wxTextCtrl(panel, wxID_ANY, "14.4", wxPoint(minX + diffX, Y)), "/General/PFA");
    text_PFA->SetToolTip(wxT("Powered flight arc"));
    new wxStaticText(panel, wxID_ANY, "deg", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "PFT", wxPoint(minX, Y + difftext));
    Add(text_PFT = new wxTextCtrl(panel, wxID_ANY, "0:00:08:39.0", wxPoint(minX + diffX, Y)), "/General/PFT");
    text_PFT->SetToolTip(wxT("Powered flight time"));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "ALT", wxPoint(minX, Y + difftext));
    Add(text_ALT = new wxTextCtrl(panel, wxID_ANY, "52.0", wxPoint(minX + diffX, Y)), "/General/ALT");
    text_ALT->SetToolTip(wxT("Altitude of insertion"));
    new wxStaticText(panel, wxID_ANY, "NM", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "VEL", wxPoint(minX, Y + difftext));
    Add(text_VEL = new wxTextCtrl(panel, wxID_ANY, "25928.0", wxPoint(minX + diffX, Y)), "/General/VEL");
    text_VEL->SetToolTip(wxT("Velocity of insertion"));
    new wxStaticText(panel, wxID_ANY, "fps", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "FPA", wxPoint(minX, Y + difftext));
    Add(text_FPA = new wxTextCtrl(panel, wxID_ANY, "0.6", wxPoint(minX + diffX, Y)), "/General/FPA");
    text_FPA->SetToolTip(wxT("Flightpath angle of insertion"));
    new wxStaticText(panel, wxID_ANY, "deg", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "INC", wxPoint(minX, Y + difftext));
    Add(text_INC = new wxTextCtrl(panel, wxID_ANY, "51.6", wxPoint(minX + diffX, Y)), "/General/INC");
    text_INC->SetToolTip(wxT("Inclination of insertion"));
    new wxStaticText(panel, wxID_ANY, "deg", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "DIR", wxPoint(minX, Y + difftext));
    strings.Add(wxT("North"));
    strings.Add(wxT("South"));
    Add(combo_LaunchAzimuthDirection = new wxChoice(panel, wxID_ANY, wxPoint(minX + diffX + 40, Y), wxDefaultSize, strings), "/General/DIR");
    combo_LaunchAzimuthDirection->SetToolTip(wxT("Launch azimuth direction flag"));
    combo_LaunchAzimuthDirection->SetSelection(0);
    strings.clear();
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "DTOPT", wxPoint(minX, Y + difftext));
    Add(text_DTOPT = new wxTextCtrl(panel, wxID_ANY, "300.0", wxPoint(minX + diffX, Y)), "/General/DTOPT");
    text_DTOPT->SetToolTip(wxT("Optimum launch time bias"));
    new wxStaticText(panel, wxID_ANY, "sec", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "ET SEP PARAMETERS", wxPoint(minX + diffX, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "DVX", wxPoint(minX, Y + difftext));
    Add(text_ET_Sep_DVX = new wxTextCtrl(panel, wxID_ANY, "5.0", wxPoint(minX + diffX, Y)), "/General/ET_DVX");
    text_ET_Sep_DVX->SetToolTip("ET separation Delta V X in feet per second. Usually +5 due to +X translation or 0 if no translation/photo pitch maneuver.");
    new wxStaticText(panel, wxID_ANY, "fps", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "DVY", wxPoint(minX, Y + difftext));
    Add(text_ET_Sep_DVY = new wxTextCtrl(panel, wxID_ANY, "0.0", wxPoint(minX + diffX, Y)), "/General/ET_DVY");
    text_ET_Sep_DVY->SetToolTip("ET separation Delta V Y in feet per second");
    new wxStaticText(panel, wxID_ANY, "fps", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "DVZ", wxPoint(minX, Y + difftext));
    Add(text_ET_Sep_DVZ = new wxTextCtrl(panel, wxID_ANY, "-4.0", wxPoint(minX + diffX, Y)), "/General/ET_DVZ");
    text_ET_Sep_DVZ->SetToolTip("ET separation Delta V Z in feet per second. Usually +4 if insertion is heads down or -4 if heads up.");
    new wxStaticText(panel, wxID_ANY, "fps", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "DTIG", wxPoint(minX, Y + difftext));
    Add(text_ET_Sep_DTIG = new wxTextCtrl(panel, wxID_ANY, "0:00:00:12.0", wxPoint(minX + diffX, Y)), "/General/ET_DTIG");
    text_ET_Sep_DTIG->SetToolTip("Time from MECO to ET separation");
    new wxStaticText(panel, wxID_ANY, "sec", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "INSERTION", wxPoint(minX, Y + difftext));
    strings.Add(wxT("Standard"));
    strings.Add(wxT("Direct"));
    Add(combo_InsertionMode = new wxChoice(panel, wxID_ANY, wxPoint(minX + diffX + 40, Y), wxDefaultSize, strings), "/General/INSERTION_MODE");
    combo_InsertionMode->SetToolTip(wxT("Insertion mode"));
    combo_InsertionMode->SetSelection(1);
    strings.clear();
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "MPS DUMP", wxPoint(minX + diffX, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "DVX", wxPoint(minX, Y + difftext));
    Add(text_MPS_Dump_DVX = new wxTextCtrl(panel, wxID_ANY, "8.1", wxPoint(minX + diffX, Y)), "/General/MPS_DVX");
    text_MPS_Dump_DVX->SetToolTip("MPS dump Delta V X in feet per second");
    new wxStaticText(panel, wxID_ANY, "fps", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "DVY", wxPoint(minX, Y + difftext));
    Add(text_MPS_Dump_DVY = new wxTextCtrl(panel, wxID_ANY, "0.5", wxPoint(minX + diffX, Y)), "/General/MPS_DVY");
    text_MPS_Dump_DVY->SetToolTip("MPS dump Delta V Y in feet per second");
    new wxStaticText(panel, wxID_ANY, "fps", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "DVZ", wxPoint(minX, Y + difftext));
    Add(text_MPS_Dump_DVZ = new wxTextCtrl(panel, wxID_ANY, "1.6", wxPoint(minX + diffX, Y)), "/General/MPS_DVZ");
    text_MPS_Dump_DVZ->SetToolTip("MPS dump Delta V Z in feet per second");
    new wxStaticText(panel, wxID_ANY, "fps", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "DTIG", wxPoint(minX, Y + difftext));
    Add(text_MPS_Dump_DTIG = new wxTextCtrl(panel, wxID_ANY, "0:00:01:54.0", wxPoint(minX + diffX, Y)), "/General/MPS_DTIG");
    text_MPS_Dump_DTIG->SetToolTip("Time from MECO to MPS dump");
    new wxStaticText(panel, wxID_ANY, "sec", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    Y += diffY;
    new wxStaticText(panel, wxID_ANY, "COMMENT", wxPoint(minX, Y + difftext));
    Add(text_Comment = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(minX + diffX, Y), wxSize(256, -1)), "/General/COMMENT");
    text_Comment->SetToolTip("Description of ascent plan");

    minX += 256;
    Y = minY;

    new wxStaticText(panel, wxID_ANY, "EXTERNAL TANK DATA", wxPoint(minX, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "CD", wxPoint(minX, Y + difftext));
    Add(text_ET_CD = new wxTextCtrl(panel, wxID_ANY, "2.0", wxPoint(minX + diffX, Y)), "/General/ET_CD");
    text_ET_CD->SetToolTip("Drag coefficient");
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "AREA", wxPoint(minX, Y + difftext));
    Add(text_ET_Area = new wxTextCtrl(panel, wxID_ANY, "598.0", wxPoint(minX + diffX, Y)), "/General/ET_AREA");
    text_ET_Area->SetToolTip("Vehicle area");
    new wxStaticText(panel, wxID_ANY, "ft2", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "WEIGHT", wxPoint(minX, Y + difftext));
    Add(text_ET_Weight = new wxTextCtrl(panel, wxID_ANY, "75000.0", wxPoint(minX + diffX, Y)), "/General/ET_WEIGHT");
    text_ET_Weight->SetToolTip("Vehicle weight");
    new wxStaticText(panel, wxID_ANY, "lbm", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "SHUTTLE ORBITER DATA", wxPoint(minX, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "CD", wxPoint(minX, Y + difftext));
    Add(text_Shuttle_CD = new wxTextCtrl(panel, wxID_ANY, "2.0", wxPoint(minX + diffX, Y)), "/General/CD");
    text_Shuttle_CD->SetToolTip("Drag coefficient");
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "AREA", wxPoint(minX, Y + difftext));
    Add(text_Shuttle_Area = new wxTextCtrl(panel, wxID_ANY, "2500.0", wxPoint(minX + diffX, Y)), "/General/AREA");
    text_Shuttle_Area->SetToolTip("Vehicle area");
    new wxStaticText(panel, wxID_ANY, "ft2", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "WEIGHT", wxPoint(minX, Y + difftext));
    Add(text_Shuttle_Weight = new wxTextCtrl(panel, wxID_ANY, "251679", wxPoint(minX + diffX, Y)), "/General/WEIGHT");
    text_Shuttle_Weight->SetToolTip("Vehicle weight");
    new wxStaticText(panel, wxID_ANY, "lbm", wxPoint(minX + diffunit, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "OMS-1 TGTS", wxPoint(minX, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "DTIG", wxPoint(minX, Y + difftext));
    Add(text_OMS1_DTIG = new wxTextCtrl(panel, wxID_ANY, "0:00:02:00.000", wxPoint(minX + diffX, Y)), "/General/OMS1_DTIG");
    text_OMS1_DTIG->SetToolTip("Time from MECO to OMS-1");
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "C1", wxPoint(minX, Y + difftext));
    Add(text_OMS1_C1 = new wxTextCtrl(panel, wxID_ANY, "0.0", wxPoint(minX + diffX, Y)), "/General/OMS1_C1");
    text_OMS1_C1->SetToolTip("C1 for OMS-1");
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "C2", wxPoint(minX, Y + difftext));
    Add(text_OMS1_C2 = new wxTextCtrl(panel, wxID_ANY, "0.0", wxPoint(minX + diffX, Y)), "/General/OMS1_C2");
    text_OMS1_C2->SetToolTip("C2 for OMS-1");
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "HT", wxPoint(minX, Y + difftext));
    Add(text_OMS1_HT = new wxTextCtrl(panel, wxID_ANY, "158.0", wxPoint(minX + diffX, Y)), "/General/OMS1_HT");
    text_OMS1_HT->SetToolTip("Height for OMS-1");
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "THETA-T", wxPoint(minX, Y + difftext));
    Add(text_OMS1_THETAT = new wxTextCtrl(panel, wxID_ANY, "154.0", wxPoint(minX + diffX, Y)), "/General/OMS1_THETA");
    text_OMS1_THETAT->SetToolTip("Theta-T for OMS-1");
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "OMS-2 TGTS", wxPoint(minX, Y + difftext));
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "DTIG", wxPoint(minX, Y + difftext));
    Add(text_OMS2_DTIG = new wxTextCtrl(panel, wxID_ANY, "0:00:35:01.000", wxPoint(minX + diffX, Y)), "/General/OMS2_DTIG");
    text_OMS2_DTIG->SetToolTip("Time from MECO to OMS-2");
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "C1", wxPoint(minX, Y + difftext));
    Add(text_OMS2_C1 = new wxTextCtrl(panel, wxID_ANY, "0.0", wxPoint(minX + diffX, Y)), "/General/OMS2_C1");
    text_OMS2_C1->SetToolTip("C1 for OMS-2");
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "C2", wxPoint(minX, Y + difftext));
    Add(text_OMS2_C2 = new wxTextCtrl(panel, wxID_ANY, "0.0", wxPoint(minX + diffX, Y)), "/General/OMS2_C2");
    text_OMS2_C2->SetToolTip("C2 for OMS-2");
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "HT", wxPoint(minX, Y + difftext));
    Add(text_OMS2_HT = new wxTextCtrl(panel, wxID_ANY, "110.8", wxPoint(minX + diffX, Y)), "/General/OMS2_HT");
    text_OMS2_HT->SetToolTip("Height for OMS-2");
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "THETA-T", wxPoint(minX, Y + difftext));
    Add(text_OMS2_THETAT = new wxTextCtrl(panel, wxID_ANY, "338.0", wxPoint(minX + diffX, Y)), "/General/OMS2_THETA");
    text_OMS2_THETAT->SetToolTip("Theta-T for OMS-2");
    Y += diffY;

    Y += diffY;
    new wxButton(panel, ID_Button_Calculate, wxT("Calculate"), wxPoint(minX + diffX, Y), wxDefaultSize);
    Y += diffY;

    minX += 256;
    Y = minY;

    text_Output = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(minX, Y), wxSize(600, 500), wxTE_READONLY | wxTE_MULTILINE);

    Y = 564;
    new wxStaticText(panel, wxID_ANY, "CONVERGE HA", wxPoint(minX, Y + difftext));
    Add(text_HA_DESIRED = new wxTextCtrl(panel, wxID_ANY, "125.0", wxPoint(minX + diffX*2, Y)), "/General/HA_DESIRED");
    text_HA_DESIRED->SetToolTip("Direct Insertion: Use insertion velocity to achieve desired apogee height after MPS Dump");
    new wxButton(panel, ID_Button_HA_Converge, wxT("Converge"), wxPoint(minX + diffX*4, Y), wxDefaultSize);
    Y += diffY;

    new wxStaticText(panel, wxID_ANY, "CONVERGE HP", wxPoint(minX, Y + difftext));
    Add(text_HP_DESIRED = new wxTextCtrl(panel, wxID_ANY, "85.0", wxPoint(minX + diffX * 2, Y)), "/General/HP_DESIRED");
    text_HP_DESIRED->SetToolTip("Desired perigee height after OMS-2");
    new wxButton(panel, ID_Button_HP_Converge, wxT("Converge"), wxPoint(minX + diffX * 4, Y), wxDefaultSize);
    Y += diffY;

    Y += diffY * 2;
    new wxButton(panel, ID_Button_Export, wxT("Export"), wxPoint(minX + diffX * 4, Y), wxDefaultSize);
    Y += diffY;

    ads = new AscentDesignSystem();
}

AscentDesignSystemFrame::~AscentDesignSystemFrame()
{
    delete ads;
}

void AscentDesignSystemFrame::OnNew(wxCommandEvent& event)
{
    // Set everything to default values
}

void SaveTextCtrl(wxFileConfig* config, wxTextCtrl* ctrl, wxString str)
{
    config->Write(str, ctrl->GetLineText(0));
}

void LoadTextCtrl(wxFileConfig* config, wxTextCtrl* ctrl, wxString str)
{
    wxString strtemp;
    if (config->Read(str, &strtemp)) ctrl->ChangeValue(strtemp);
}

void SaveChoiceBox(wxFileConfig* config, wxChoice* ctrl, wxString str)
{
    config->Write(str, ctrl->GetSelection());
}

void LoadChoiceBox(wxFileConfig* config, wxChoice* ctrl, wxString str)
{
    int inttemp;
    if (config->Read(str, &inttemp)) ctrl->SetSelection(inttemp);
}

void SaveCheckBox(wxFileConfig* config, wxCheckBox* ctrl, wxString str)
{
    config->Write(str, ctrl->GetValue());
}

void LoadCheckBox(wxFileConfig* config, wxCheckBox* ctrl, wxString str)
{
    bool bTemp;
    if (config->Read(str, &bTemp)) ctrl->SetValue(bTemp);
}

void AscentDesignSystemFrame::OnOpen(wxCommandEvent& event)
{
    wxFileName fn(wxGetCwd());
    wxString defaultDir = fn.GetPath();

    wxFileDialog dlg(this, "Open file", defaultDir, wxEmptyString, "Ini files (*.ini)|*.ini", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dlg.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    wxFileConfig* config = new wxFileConfig("ADS", wxEmptyString, dlg.GetPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_RELATIVE_PATH);

    for (unsigned i = 0; i < textCtrlData.size(); i++)
    {
        LoadTextCtrl(config, textCtrlData[i].text, textCtrlData[i].name);
    }
    for (unsigned i = 0; i < choiceData.size(); i++)
    {
        LoadChoiceBox(config, choiceData[i].choice, choiceData[i].name);
    }
    for (unsigned i = 0; i < checkBoxData.size(); i++)
    {
        LoadCheckBox(config, checkBoxData[i].checkBox, checkBoxData[i].name);
    }

    // Clear the output text box
    text_Output->Clear();
    // Include file name in title
    SetTitle(dlg.GetPath() + " - Ascent Design System");
    
    delete config;
}

void AscentDesignSystemFrame::OnSave(wxCommandEvent& event)
{
    wxFileName fn(wxGetCwd());
    wxString defaultDir = fn.GetPath();

    wxFileDialog dlg(this, "Save file", defaultDir, wxEmptyString, "Ini files (*.ini)|*.ini", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dlg.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    wxFileConfig* config = new wxFileConfig("ADS", wxEmptyString, dlg.GetPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_RELATIVE_PATH);

    for (unsigned i = 0; i < textCtrlData.size(); i++)
    {
        SaveTextCtrl(config, textCtrlData[i].text, textCtrlData[i].name);
    }
    for (unsigned i = 0; i < choiceData.size(); i++)
    {
        SaveChoiceBox(config, choiceData[i].choice, choiceData[i].name);
    }
    for (unsigned i = 0; i < checkBoxData.size(); i++)
    {
        SaveCheckBox(config, checkBoxData[i].checkBox, checkBoxData[i].name);
    }

    delete config;

    SetStatusText("Save successful!");

    // Include file name in title
    SetTitle(dlg.GetPath() + " - Ascent Design System");
}

void AscentDesignSystemFrame::OnQuit(wxCommandEvent& event)
{
    // Destroy the frame
    Close();
}

void AscentDesignSystemFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg;
    msg.Printf("Hello and welcome to %s",
        wxVERSION_STRING);
    wxMessageBox(msg, "About Minimal",
        wxOK | wxICON_INFORMATION, this);
}

void AscentDesignSystemFrame::OnCombo_Launchpad(wxCommandEvent& event)
{
    text_LATLS->Clear();
    text_LONGLS->Clear();

    switch (combo_Launchpad->GetSelection())
    {
    case 0: // LC-39A
        text_LATLS->AppendText("28:36:30");
        text_LONGLS->AppendText("279:23:45");
        break;
    case 1: // LC-39B
        text_LATLS->AppendText("28:37:38");
        text_LONGLS->AppendText("279:22:45");
        break;
    case 2: // SLC-6
        text_LATLS->AppendText("34:34:53");
        text_LONGLS->AppendText("239:22:23");
        break;
    }
}

void AscentDesignSystemFrame::OnButtonCalculate(wxCommandEvent& event)
{
    std::vector<ADSArray> arr;
    if (CollectInputs(arr)) return;

    std::vector<std::string> out;

    ads->PerformAscentSimulation(arr, out);

    WriteOutput(out);
}

void AscentDesignSystemFrame::OnButtonHAConverge(wxCommandEvent& event)
{
    std::vector<ADSArray> arr;
    double HA;
    int Error;

    if (CollectInputs(arr)) return;
    if (GetDouble(text_HA_DESIRED, "HA DESIRED", &HA)) return;

    std::vector<std::string> out;

    Error = ads->ConvergeHA(arr, HA, out);
    if (Error)
    {
        out.clear();
        out.push_back("Failed to converge on HA!");
    }
    else

    WriteOutput(out);
}

void AscentDesignSystemFrame::OnButtonHPConverge(wxCommandEvent& event)
{
    std::vector<ADSArray> arr;
    double HP;
    int Error;

    if (CollectInputs(arr)) return;
    if (GetDouble(text_HP_DESIRED, "HP DESIRED", &HP)) return;

    std::vector<std::string> out;

    Error = ads->ConvergeHP(arr, HP, out);
    if (Error)
    {
        out.clear();
        out.push_back("Failed to converge on HP!");
    }

    WriteOutput(out);
}

void AscentDesignSystemFrame::OnButtonExport(wxCommandEvent& event)
{
    wxFileName fn(wxGetCwd());
    wxString defaultDir = fn.GetPath();

    wxFileDialog dlg(this, "Export I-loads:", defaultDir, wxEmptyString, "Text files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dlg.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    wxString filename = dlg.GetPath();

    wxTextFile file(filename);

    if (!file.Exists())
    {
        file.Create();
    }

    if (!file.Open())
        return;

    file.Clear();

    // Collect inputs
    std::vector<ADSArray> arr;
    if (CollectInputs(arr)) return;

    std::vector<std::string> out;
    ads->Export(arr, out);

    for (unsigned i = 0; i < out.size(); i++)
    {
        file.AddLine(out[i]);
    }

    file.Write();
    file.Close();
}

int AscentDesignSystemFrame::GetInteger(wxTextCtrl* text, const wxString& name, int* val)
{
    if (text->GetValue().ToInt(val) == false)
    {
        SetStatusText(name + " has invalid format!");
        return 1;
    }
    return 0;
}

int AscentDesignSystemFrame::GetDouble(wxTextCtrl* text, const wxString& name, double* val)
{
    if (text->GetValue().ToDouble(val) == false)
    {
        SetStatusText(name + " has invalid format!");
        return 1;
    }
    return 0;
}

int AscentDesignSystemFrame::GetDDDHHMMSS(wxTextCtrl* text, const wxString& name, double* val)
{
    double sgn, secs;
    int vals[3];

    if (ParseTime(text, 4, sgn, vals, &secs))
    {
        SetStatusText(name + " has invalid format!");
        return 1;
    }

    *val = sgn * (secs + (double)(60 * (vals[2] + 60 * vals[1])));
    return 0;
}

int AscentDesignSystemFrame::GetHHMMSS(wxTextCtrl* text, const wxString& name, double* val)
{
    double sgn, secs;
    int vals[2];

    if (ParseTime(text, 3, sgn, vals, &secs))
    {
        SetStatusText(name + " has invalid format!");
        return 1;
    }

    *val = sgn * ((double)(vals[0]) + (double)(vals[1]) / 60.0 + secs / 3600.0);
    return 0;
}

int AscentDesignSystemFrame::ParseTime(wxTextCtrl* text, size_t size, double& sgn, int* vals, double* secs)
{
    wxStringTokenizer tkz(text->GetValue(), ":");

    if (tkz.CountTokens() != size) return 1;

    wxArrayString strings;

    strings.resize(size);

    for (size_t i = 0; i < size; i++)
    {
        strings[i] = tkz.GetNextToken();
    }

    // Convert to int and double
    for (size_t i = 0; i < size; i++)
    {
        if ((i + 1) < size)
        {
            // Not last
            if (strings[i].ToInt(&vals[i]) == false) return 1;
        }
        else
        {
            // Last
            if (strings[i].ToDouble(secs) == false) return 1;
        }

        if (i == 0)
        {
            // First
            if (strings[0][0] == '-')
            {
                sgn = -1.0;
                vals[0] = abs(vals[0]);
            }
            else
            {
                sgn = 1.0;
            }
        }
    }

    return 0;
}

int AscentDesignSystemFrame::GetDouble(wxTextCtrl* text, const wxString& name, std::vector<ADSArray>& arr)
{
    double val;
    int err;

    err = (GetDouble(text, name, &val));
    if (err) return err;

    ADSArray temp;

    temp.dVal = val;
    arr.push_back(temp);

    return 0;
}

int AscentDesignSystemFrame::GetDDDHHMMSS(wxTextCtrl* text, const wxString& name, std::vector<ADSArray>& arr)
{
    double val;
    int err;

    err = (GetDDDHHMMSS(text, name, &val));
    if (err) return err;

    ADSArray temp;

    temp.dVal = val;
    arr.push_back(temp);

    return 0;
}

int AscentDesignSystemFrame::GetHHMMSS(wxTextCtrl* text, const wxString& name, std::vector<ADSArray>& arr)
{
    double val;
    int err;

    err = (GetHHMMSS(text, name, &val));
    if (err) return err;

    ADSArray temp;

    temp.dVal = val;
    arr.push_back(temp);

    return 0;
}

int AscentDesignSystemFrame::CollectInputs(std::vector<ADSArray>& arr)
{
    ADSArray temp;

    if (GetDouble(text_INC, "INC", arr)) return 1;
    if (GetHHMMSS(text_LATLS, "LATLS", arr)) return 1;
    if (GetHHMMSS(text_LONGLS, "LONGLS", arr)) return 1;
    temp.iVal = combo_LaunchAzimuthDirection->GetSelection(); arr.push_back(temp);
    if (GetDouble(text_DTOPT, "DTOPT", arr)) return 1;
    if (GetDouble(text_PFA, "PFA", arr)) return 1;
    if (GetDDDHHMMSS(text_PFT, "PFT", arr)) return 1;
    if (GetDouble(text_ALT, "ALT", arr)) return 1;
    if (GetDouble(text_VEL, "VEL", arr)) return 1;
    if (GetDouble(text_FPA, "FPA", arr)) return 1;
    if (GetDouble(text_ET_Sep_DVX, "ET Sep DVX", arr)) return 1;
    if (GetDouble(text_ET_Sep_DVY, "ET Sep DVY", arr)) return 1;
    if (GetDouble(text_ET_Sep_DVZ, "ET Sep DVZ", arr)) return 1;
    if (GetDDDHHMMSS(text_ET_Sep_DTIG, "ET Sep DTIG", arr)) return 1;
    temp.iVal = combo_InsertionMode->GetSelection(); arr.push_back(temp);
    if (GetDouble(text_MPS_Dump_DVX, "MPS Dump DVX", arr)) return 1;
    if (GetDouble(text_MPS_Dump_DVY, "MPS Dump DVY", arr)) return 1;
    if (GetDouble(text_MPS_Dump_DVZ, "MPS Dump DVZ", arr)) return 1;
    if (GetDDDHHMMSS(text_MPS_Dump_DTIG, "MPS Dump DTIG", arr)) return 1;
    if (GetDDDHHMMSS(text_OMS1_DTIG, "OMS-1 DTIG", arr)) return 1;
    if (GetDouble(text_OMS1_C1, "OMS-1 C1", arr)) return 1;
    if (GetDouble(text_OMS1_C2, "OMS-1 C2", arr)) return 1;
    if (GetDouble(text_OMS1_HT, "OMS-1 HT", arr)) return 1;
    if (GetDouble(text_OMS1_THETAT, "OMS-1 THETAT", arr)) return 1;
    if (GetDDDHHMMSS(text_OMS2_DTIG, "OMS-2 DTIG", arr)) return 1;
    if (GetDouble(text_OMS2_C1, "OMS-2 C1", arr)) return 1;
    if (GetDouble(text_OMS2_C2, "OMS-2 C2", arr)) return 1;
    if (GetDouble(text_OMS2_HT, "OMS-2 HT", arr)) return 1;
    if (GetDouble(text_OMS2_THETAT, "OMS-2 THETAT", arr)) return 1;
    if (GetDouble(text_Shuttle_CD, "Chaser CD", arr)) return 1;
    if (GetDouble(text_Shuttle_Area, "Chaser Area", arr)) return 1;
    if (GetDouble(text_Shuttle_Weight, "Chaser Weight", arr)) return 1;
    if (GetDouble(text_ET_CD, "ET CD", arr)) return 1;
    if (GetDouble(text_ET_Area, "ET Area", arr)) return 1;
    if (GetDouble(text_ET_Weight, "ET Weight", arr)) return 1;

    return 0;
}

void AscentDesignSystemFrame::WriteOutput(const std::vector<std::string>& out)
{
    text_Output->Clear();

    for (unsigned i = 0; i < out.size(); i++)
    {
        text_Output->AppendText(out[i]);
        text_Output->AppendText("\n");
    }
}

void AscentDesignSystemFrame::Add(wxTextCtrl* text, const wxString& config)
{
    wxTextCtrlData data;

    data.text = text;
    data.name = config;

    textCtrlData.push_back(data);
}

void AscentDesignSystemFrame::Add(wxChoice* choice, const wxString& config)
{
    wxChoiceData data;

    data.choice = choice;
    data.name = config;

    choiceData.push_back(data);
}

void AscentDesignSystemFrame::Add(wxCheckBox* choice, const wxString& config)
{
    wxCheckBoxData data;

    data.checkBox = choice;
    data.name = config;

    checkBoxData.push_back(data);
}