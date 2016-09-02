/**********************************************************************
  imgcalkap - Simple map calibration tool for imgcap
    Copyright (C) 2014  -  David Giardini - dgiardini@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/
#ifndef __IMGCALKAP_DLG_H_INC
#define __IMGCALKAP_DLG_H_INC

#include "../imgcalkap.h"

/* Coordinates dialog dimensions */
#ifndef COMPILE_LINUX
  #define L_MARGIN   20
  #define T_WIDTH    40
  #define E_WIDTH    (40+100)
  #define E2_WIDTH   70
  #define CTRL_SEP   5
#else
  #define L_MARGIN   20
  #define T_WIDTH    60
  #define E_WIDTH    (50+100)
  #define E2_WIDTH   90
  #define CTRL_SEP   5
#endif


#define M1   L_MARGIN + 150
#define M2   M1+T_WIDTH+CTRL_SEP
#define M3   M2+E_WIDTH+2*CTRL_SEP
#define M4   M3+T_WIDTH+CTRL_SEP

#define CTRL_H     20
#define H_SEP      25
#define H_OFFSET   5

#define BTN_WIDTH 60
#define	BTN_HEIGHT 25

#define H_MARGIN 	CTRL_H+2*H_SEP
#define CAL_DLG_HEIGHT  H_MARGIN*3
#define CAL_DLG_WIDTH   M4+E2_WIDTH+2*L_MARGIN


/* Map dialog dimensions */

/* Map dialog dimensions */
#define M_L_MARGIN   20
#define M_T_WIDTH    60
#define M_CTRL_SEP   5
#define M_E_WIDTH    100

#define M_M1   M_L_MARGIN
#define M_M2   M_M1+M_T_WIDTH+CTRL_SEP
#define M_M3   M_M2+M_E_WIDTH+2*CTRL_SEP
//#define M_M4   M_M3+M_T_WIDTH+CTRL_SEP

#define MAP_DLG_HEIGHT  H_MARGIN*5
#define MAP_DLG_WIDTH   M_M3+2*M_L_MARGIN

/****************************  Class for enter coordinates ***********************/
class CoordinatesDlg : public wxDialog
{
public:
    // Constructor(s)
    CoordinatesDlg(const wxString& title, const wxPoint& pos, const wxSize& size);
	~CoordinatesDlg(){Destroy();}
     // Event handlers (these functions should _not_ be virtual)
    void OnCancel(wxCommandEvent& event);
    void Quit2(wxCloseEvent& event);
    void OnOk(wxCommandEvent& event);
    void SetData(data_dialog_t * p);
    	
private:
	// Edit boxes
	wxTextCtrl *LatLonEdit;
	/*
	wxTextCtrl *LatDegEdit;
	wxTextCtrl *LatMinEdit;
	wxTextCtrl *LonDegEdit;
	wxTextCtrl *LonMinEdit;
	*/
	//Cordinates data
	data_dialog_t * data;
	int result;
    // Any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};
// IDs for the controls in the dialog
enum
{
    // command buttons
	CDlg_Ok=1,	
	CDlg_Cancel,
/*	
    CDlg_LatDegText=1000,
	CDlg_LatDegEdit,
	CDlg_LatMinText,
	CDlg_LatMinEdit,
	CDlg_LonDegText,
	CDlg_LonDegEdit,
	CDlg_LonMinText,
	CDlg_LonMinEdit,
*/	
	CDlg_LatLonEdit=1000,
	CDlg_LatLonText,
	CDlg_MiniHelp,
	CDlg_StaticBox	
};


/****************************  Class for enter map data ***********************/
class MapDlg : public wxDialog
{
public:
    // Constructor(s)
    MapDlg(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MapDlg(){Destroy();}
    // Event handlers (these functions should _not_ be virtual)
    void OnCancel(wxCommandEvent& event);
    void Quit2(wxCloseEvent& event);
    void OnOk(wxCommandEvent& event);
    void SetData(data_dialog_t * p);
    
private:
	// Edit boxes
	wxTextCtrl *ChartNameEdit;
	wxTextCtrl *ChartDescEdit;
	wxTextCtrl *ChartScaleEdit;
	wxComboBox *CmbProjEdit;
	wxComboBox *CmbDatumEdit;
	wxComboBox *CmbDepthEdit;
	wxTextCtrl *StaticBox;	

	//Cordinates data
	data_dialog_t * data;
	int result;
    // Any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};
// IDs for the controls in the dialog
enum
{
    // command buttons
    MDlg_Ok=1,
	MDlg_Cancel,
    // Edit and combo boxes
	MDlg_ChartNameEdit=1000,
	MDlg_ChartDescEdit,
	MDlg_ChartScaleEdit,
	MDlg_CmbProjEdit,
	MDlg_CmbDatumEdit,
	MDlg_CmbDepthEdit,
	MDlg_StaticBox,	
	MDlg_ChartNameText,
	MDlg_ChartDescText,
	MDlg_ScaleText,
	MDlg_CmbProjText,
	MDlg_CmbDatumText,
	MDlg_CmbDepthText
	
};

#endif
