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

#pragma once

#include <wx/wx.h>
#include "../base/AscentDesignSystem.h"

// Declare the application class
class AscentDesignSystemApp : public wxApp
{
public:
	// Called on application startup
	bool OnInit() override;
};

// This defines the equivalent of main() for the current platform.
wxIMPLEMENT_APP(AscentDesignSystemApp);

// Declare our main frame class
class AscentDesignSystemFrame : public wxFrame
{
public:
	// Constructor
	AscentDesignSystemFrame(const wxString& title);
	~AscentDesignSystemFrame();

	// Event handlers
	void OnNew(wxCommandEvent& event);
	void OnOpen(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnCombo_Launchpad(wxCommandEvent& event);
	void OnButtonCalculate(wxCommandEvent& event);
	void OnButtonHAConverge(wxCommandEvent& event);
	void OnButtonHPConverge(wxCommandEvent& event);
	void OnButtonExport(wxCommandEvent& event);
private:

	// Text parsing
	int GetInteger(wxTextCtrl* text, const wxString& name, int* val);
	int GetDouble(wxTextCtrl* text, const wxString& name, double* val);
	int GetDDDHHMMSS(wxTextCtrl* text, const wxString& name, double* val);
	int GetHHMMSS(wxTextCtrl* text, const wxString& name, double* val);
	int ParseTime(wxTextCtrl* text, size_t size, double &sgn, int *vals, double* secs);

	int GetDouble(wxTextCtrl* text, const wxString& name, std::vector<ADSArray>& arr);
	int GetDDDHHMMSS(wxTextCtrl* text, const wxString& name, std::vector<ADSArray>& arr);
	int GetHHMMSS(wxTextCtrl* text, const wxString& name, std::vector<ADSArray>& arr);

	// Utilities
	int CollectInputs(std::vector<ADSArray> &arr);
	void WriteOutput(const std::vector<std::string>& out);

	// Save/Load Handling
	struct wxTextCtrlData
	{
		wxTextCtrl* text;
		wxString name;
	};

	struct wxChoiceData
	{
		wxChoice* choice;
		wxString name;
	};

	struct wxCheckBoxData
	{
		wxCheckBox* checkBox;
		wxString name;
	};

	void Add(wxTextCtrl* text, const wxString& config);
	void Add(wxChoice* choice, const wxString& config);
	void Add(wxCheckBox* choice, const wxString& config);

	std::vector<wxTextCtrlData> textCtrlData;
	std::vector<wxChoiceData> choiceData;
	std::vector<wxCheckBoxData> checkBoxData;

	wxChoice* combo_Launchpad;
	wxTextCtrl* text_LATLS;
	wxTextCtrl* text_LONGLS;
	wxTextCtrl* text_GMTLO;
	wxTextCtrl* text_PFA;
	wxTextCtrl* text_PFT;
	wxTextCtrl* text_ALT;
	wxTextCtrl* text_VEL;
	wxTextCtrl* text_FPA;
	wxTextCtrl* text_INC;
	wxChoice* combo_LaunchAzimuthDirection;
	wxTextCtrl* text_DTOPT;
	wxTextCtrl* text_ET_Sep_DVX;
	wxTextCtrl* text_ET_Sep_DVY;
	wxTextCtrl* text_ET_Sep_DVZ;
	wxTextCtrl* text_ET_Sep_DTIG;
	wxTextCtrl* text_Output;
	wxChoice* combo_InsertionMode;
	wxTextCtrl* text_MPS_Dump_DVX;
	wxTextCtrl* text_MPS_Dump_DVY;
	wxTextCtrl* text_MPS_Dump_DVZ;
	wxTextCtrl* text_MPS_Dump_DTIG;
	wxTextCtrl* text_ET_CD;
	wxTextCtrl* text_ET_Area;
	wxTextCtrl* text_ET_Weight;
	wxTextCtrl* text_Shuttle_CD;
	wxTextCtrl* text_Shuttle_Area;
	wxTextCtrl* text_Shuttle_Weight;
	wxTextCtrl* text_OMS1_DTIG;
	wxTextCtrl* text_OMS1_C1;
	wxTextCtrl* text_OMS1_C2;
	wxTextCtrl* text_OMS1_HT;
	wxTextCtrl* text_OMS1_THETAT;
	wxTextCtrl* text_OMS2_DTIG;
	wxTextCtrl* text_OMS2_C1;
	wxTextCtrl* text_OMS2_C2;
	wxTextCtrl* text_OMS2_HT;
	wxTextCtrl* text_OMS2_THETAT;
	wxTextCtrl* text_HA_DESIRED;
	wxTextCtrl* text_HP_DESIRED;
	wxTextCtrl* text_Comment;

	// This class handles events
	DECLARE_EVENT_TABLE()

	// ADS pointer
	AscentDesignSystem* ads;

	// Current save file name
	wxString FileName;
};