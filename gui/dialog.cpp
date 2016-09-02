#include "wx/wx.h"
#include "dialog.h"
#include "../parse_dms.h"

/****************************  Class for enter coordinates ***********************/
BEGIN_EVENT_TABLE(CoordinatesDlg, wxDialog)
	EVT_BUTTON(CDlg_Ok, CoordinatesDlg::OnOk)
	EVT_BUTTON(CDlg_Cancel, CoordinatesDlg::OnCancel)
	EVT_CLOSE(CoordinatesDlg::Quit2)
END_EVENT_TABLE()
CoordinatesDlg::CoordinatesDlg(const wxString& title, const wxPoint& pos, const wxSize& size)
		: wxDialog((wxDialog *)NULL, -1, title, pos, size)
{
	//wxString minihelp=wxT("Enter integer degrees and decimal minutes\nUse (-) sign for S/W in the degree field\n");
	const char * msg="Enter latitud and longitude\n"
				"Examples:\n"
				"32S 34.98 52W 30.00\n"
				"32S 35 30 52W 30 15\n"
				"32°35'30\"S 52°30'15\"W\n"
				"-33 35 30 -52 30 15\n";
	wxString minihelp= wxString::FromAscii(msg);
	

	wxStaticBox *statBox = new wxStaticBox(this, CDlg_StaticBox,wxT(" Lat/Long "), wxPoint(5,0), wxSize(CAL_DLG_WIDTH-15,CAL_DLG_HEIGHT-10));
	wxStaticText *MiniHelp = new wxStaticText(this, CDlg_MiniHelp,minihelp, wxPoint(L_MARGIN,15));
	/* Latitude/Longitude */	
	wxStaticText *LatLonDegText = new wxStaticText(this, CDlg_LatLonText, wxT("Lat Lon:"), wxPoint(M1,2*H_SEP+H_OFFSET));
	LatLonEdit = new wxTextCtrl(this, CDlg_LatLonEdit, wxT(""), wxPoint(M2,2*H_SEP+H_OFFSET), wxSize(E_WIDTH,CTRL_H));
	/* Ok-Cancel buttons */
	wxButton *btnOk        = new wxButton( this, CDlg_Ok, wxT("Ok"), wxPoint(3*L_MARGIN,CAL_DLG_HEIGHT-2*BTN_HEIGHT-CTRL_H), wxSize(BTN_WIDTH,BTN_HEIGHT));
	wxButton *btnCancel    = new wxButton( this, CDlg_Cancel, wxT("Cancel"), wxPoint(CAL_DLG_WIDTH-3*L_MARGIN-50,CAL_DLG_HEIGHT-2*BTN_HEIGHT-CTRL_H), wxSize(BTN_WIDTH,BTN_HEIGHT));
}

void CoordinatesDlg::SetData(data_dialog_t * p)
{
	data=p;
	result=0;
	if(p->dialog_type!=DIALOG_CAL_POINT)
	{
		Destroy();
		return;
	}
	LatLonEdit->SetValue(wxT(p->input_str));
}
void CoordinatesDlg::Quit2(wxCloseEvent& WXUNUSED(event))
{
	Destroy();
}
void CoordinatesDlg::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	EndModal(0);
    Destroy();
}

void CoordinatesDlg::OnOk(wxCommandEvent& WXUNUSED(event))
{
	wxString str_aux;
	wxString Str;
	int ret;
	str_aux=LatLonEdit->GetValue();
	//str_aux=LatDegEdit->GetValue()+LatMinEdit->GetValue()+" "+LonDegEdit->GetValue()+LonMinEdit->GetValue();;
	ret=parse_dms(str_aux.mb_str(wxConvUTF8),&data->input_lat,&data->input_lon);
	//printf("%s -> %f %f \n",str_aux.mb_str(wxConvUTF8),data->input_lat,data->input_lon);
	if(ret==0)
	{
		result=1;
	}
	else
	{
		data->input_lat=data->input_lon=0;
		result=-2;
	}
	
	//printf("Coordinates Dialog Ok. result:%d\n",result);
	fflush(stdout);
	EndModal(1);
	Destroy();
}
/****************************  Class for enter map data ***********************/

BEGIN_EVENT_TABLE(MapDlg, wxDialog)
	EVT_BUTTON(MDlg_Ok, MapDlg::OnOk)
	EVT_BUTTON(MDlg_Cancel, MapDlg::OnCancel)
	EVT_CLOSE(MapDlg::Quit2)
END_EVENT_TABLE()
MapDlg::MapDlg(const wxString& title, const wxPoint& pos, const wxSize& size)
		: wxDialog((wxDialog *)NULL, -1, title, pos, size)
{
	result=0;
	wxStaticBox *statBox = new wxStaticBox(this, MDlg_StaticBox,wxT(" Chart properties "), wxPoint(5,0), wxSize(MAP_DLG_WIDTH-15,MAP_DLG_HEIGHT-10));
	// Chart Name
	wxStaticText *ChartNameText = new wxStaticText(this, MDlg_ChartNameText, wxT("Chart Name:"), wxPoint(M_M1,H_SEP+H_OFFSET));
	ChartNameEdit = new wxTextCtrl(this, MDlg_ChartNameEdit, wxT(""), wxPoint(M_M2,H_SEP+H_OFFSET), wxSize(M_E_WIDTH,CTRL_H));
	// Description
	wxStaticText *ChartDescText = new wxStaticText(this, MDlg_ChartDescText, wxT("Description:"), wxPoint(M_M1,2*H_SEP+H_OFFSET));
	ChartDescEdit = new wxTextCtrl(this, MDlg_ChartDescEdit, wxT(""), wxPoint(M_M2,2*H_SEP+H_OFFSET), wxSize(M_E_WIDTH,CTRL_H));
	// Scale
	wxStaticText *ScaleText = new wxStaticText(this, MDlg_ScaleText, wxT("Scale:(1:x)"), wxPoint(M_M1,3*H_SEP+H_OFFSET));
	ChartScaleEdit = new wxTextCtrl(this, MDlg_ChartScaleEdit, wxT(""), wxPoint(M_M2,3*H_SEP+H_OFFSET), wxSize(M_E_WIDTH,CTRL_H));
	// Projection
	wxStaticText *CmbProjText = new wxStaticText(this, MDlg_CmbProjText, wxT("Projection:"), wxPoint(M_M1,4*H_SEP+H_OFFSET));
//	wxString cmbProjstr[NPROJ];
//	CmbProjEdit = new wxComboBox (this,MDlg_CmbProjText, wxString("Projection..."), wxPoint(M_M2,4*H_SEP+H_OFFSET), wxSize(M_E_WIDTH,CTRL_H),NPROJ,cmbProjstr,wxCB_DROPDOWN);
	CmbProjEdit = new wxComboBox (this,MDlg_CmbProjText, wxT(""), wxPoint(M_M2,4*H_SEP+H_OFFSET), wxSize(M_E_WIDTH,CTRL_H));
	// Datum
	wxStaticText *CmbDatumText = new wxStaticText(this, MDlg_CmbDatumText, wxT("Datum:"), wxPoint(M_M1,5*H_SEP+H_OFFSET));
	CmbDatumEdit = new wxComboBox(this, MDlg_CmbDatumEdit, wxT(""), wxPoint(M_M2,5*H_SEP+H_OFFSET), wxSize(M_E_WIDTH,CTRL_H));
	// Depth
	wxStaticText *CmbDepthText = new wxStaticText(this, MDlg_CmbDepthText, wxT("Depth units:"), wxPoint(M_M1,6*H_SEP+H_OFFSET));
	CmbDepthEdit = new wxComboBox(this, MDlg_CmbDepthEdit, wxT(""), wxPoint(M_M2,6*H_SEP+H_OFFSET), wxSize(M_E_WIDTH,CTRL_H));

	/* Ok-Cancel buttons */
	wxButton *btnOk        = new wxButton( this, MDlg_Ok, wxT("Ok"), wxPoint(3*L_MARGIN,MAP_DLG_HEIGHT-2*BTN_HEIGHT-CTRL_H), wxSize(BTN_WIDTH,BTN_HEIGHT));
	wxButton *btnCancel    = new wxButton( this, MDlg_Cancel, wxT("Cancel"), wxPoint(MAP_DLG_WIDTH-3*L_MARGIN-50,MAP_DLG_HEIGHT-2*BTN_HEIGHT-CTRL_H), wxSize(BTN_WIDTH,BTN_HEIGHT));
}
void MapDlg::SetData(data_dialog_t * p)
{
	data=p;
	wxString tmpstring;
	if(p->dialog_type==DIALOG_CAL_POINT)
	{
		Destroy();
		return;
	}
	wxString Str;
	/* Get data from strucure */
	ChartNameEdit->SetValue(tmpstring.FromUTF8(p->chart_name));
	ChartDescEdit->SetValue(tmpstring.FromUTF8(p->chart_desc));
	Str.Printf(wxT("%ld"),p->scale);
	//Str.mb_str(wxConvUTF8)
	ChartScaleEdit->SetValue(Str);
// Add string to combobox.
	for (int k = 0; k < NPROJ; k ++)
	{
		CmbProjEdit->Append(tmpstring.FromUTF8(projection[k]));
		if(!stricmp(projection[k],p->proj))
			CmbProjEdit->SetSelection(k);
	}						
	for (int k = 0; k < NDATUM; k ++)
	{
		CmbDatumEdit->Append(tmpstring.FromUTF8(datum[k]));
		if(!stricmp(datum[k],p->datum))
			CmbDatumEdit->SetSelection(k);
	}						
	for (int k = 0; k < NDEPTH; k ++)
	{
		CmbDepthEdit->Append(tmpstring.FromUTF8(depth[k]));
		if(!stricmp(depth[k],p->depth_units))
			CmbDepthEdit->SetSelection(k);
	}						
}
void MapDlg::Quit2(wxCloseEvent& WXUNUSED(event))
{
	EndModal(0);
	Destroy();
}
void MapDlg::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	EndModal(0);
    Destroy();
}

void MapDlg::OnOk(wxCommandEvent& WXUNUSED(event))
{
	wxString Str;
	Str=ChartNameEdit->GetValue();
	strcpy(data->chart_name,Str.mb_str(wxConvUTF8));
	Str=ChartDescEdit->GetValue();
	strcpy(data->chart_desc,Str.mb_str(wxConvUTF8));
	Str=ChartScaleEdit->GetValue();
	data->scale=atol(Str.mb_str(wxConvUTF8));
	Str=CmbProjEdit->GetValue();
	strcpy(data->proj,Str.mb_str(wxConvUTF8));
	Str=CmbDatumEdit->GetValue();
	strcpy(data->datum,Str.mb_str(wxConvUTF8));
	Str=CmbDepthEdit->GetValue();
	strcpy(data->depth_units,Str.mb_str(wxConvUTF8));
	if(verbose)printf("Chart name:%s\n Desc:%s\n Scale:%ld\n Proj:%s\n Datum:%s\n Depth:%s\n",
			data_dialog.chart_name,
			data_dialog.chart_desc,
			data_dialog.scale,
			data_dialog.proj,
			data_dialog.datum,
			data_dialog.depth_units	);
	result=1;
	EndModal(1);
	Destroy();
}