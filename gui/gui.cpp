#include <wx/wx.h>
#include <cv.h>	
#include <highgui.h>
#include "../imgcalkap.h"
#include "../proc.h"
#include "dialog.h"
#define STATUS_BAR_H -5

extern char verbose;
/* Class to display a the Open/Save dialog */
class COpenSaveFile
{
	public:
		wxString Open()
		{
			char * filename;
			wxString tmp;
			wxFrame * frame= new wxFrame();
			wxFileDialog openFileDialog(frame,  _("Open file"), wxT(""),wxT(""),
				wxT("Image files (*.bmp;*.tiff;*.png;*.jpg;... ) Kap files (*.kap)|*.bmp;*.tiff;*.tif;*.png;*.jpg;*.pbm;*.pgm;*.ppm;*.sr;*.ras;*.kap;*.KAP;*.BMP;*.TIFF;*.TIF;*.PNG"),
				wxFD_OPEN|wxFD_FILE_MUST_EXIST);
			if (openFileDialog.ShowModal() == wxID_CANCEL)
				return _(""); 
			tmp=openFileDialog.GetPath();		
			strcpy(data_dialog.filename,tmp.mb_str(wxConvUTF8));
			return tmp;
		}
		wxString Save()
		{
			char * filename;
			wxString tmp;
			wxFrame * frame= new wxFrame();
			wxFileDialog openFileDialog(frame, _("Save file"),wxT(""),wxT(""),
									wxT("Kap files (*.kap)|*.kap"),
									wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (openFileDialog.ShowModal() == wxID_CANCEL)
				return _("");
			tmp=openFileDialog.GetPath();	
			strcpy(data_dialog.filename,tmp.mb_str(wxConvUTF8));
			return tmp;
		}

};

/* Class to display simple messages that will be called from  C code */
class CShowMsg
{
	public:
		int Msg(wxString title, wxString msg, char type)
		{
			int flags=0;
			switch(type)
			{
				case T_OK:    flags=wxOK | wxICON_INFORMATION ; break;
				case T_YESNO: flags=wxYES_NO | wxICON_QUESTION ; break;
			}	
			int ret=wxMessageBox(msg,title, flags );
			if(ret==wxYES) 
				return 1;
			return  0;
		}
};
/* Class to display a dialof to enter coordinates */
class CShowCoordDlg
{
	public:
		CoordinatesDlg *dialog;
		int Msg(data_dialog_t * data)
		{
		
				dialog = new CoordinatesDlg(wxT("Edit coordinates"),  wxDefaultPosition, wxSize(CAL_DLG_WIDTH,CAL_DLG_HEIGHT));
				dialog->SetData(data);
				return dialog->ShowModal();
		}

	private:
};

/* Class to display a dialog for map info */
class CShowMapDlg
{
	public:
		MapDlg *dialog;
		int Msg(data_dialog_t * data)
		{
				dialog = new MapDlg(wxT("Chart info"),  wxDefaultPosition, wxSize(MAP_DLG_WIDTH,MAP_DLG_HEIGHT));
				dialog->SetData(data);				
				return dialog->ShowModal();
		}
	private:
};

/* C Wrapper for CShowMsg */
extern "C" int wxShowMsg(char * title, char * msg, char type)
{
	int flags=0;
	CShowMsg wxMsg;
	wxString tmp1(title,wxConvUTF8);
	wxString tmp2(msg,wxConvUTF8);
	if(verbose)
		printf("title:%s msg:%s\n",title,msg);
	return wxMsg.Msg(tmp1,tmp2,type);
}

/* C wrapper for coordinates dialog */
extern "C" int wxShowCoordDlg(data_dialog_t * data)
{
	if(data->dialog_type!=DIALOG_CAL_POINT)
	{
		if(verbose)
			printf("Se espera tipo dialogo\n");
		fflush(stdout);
		return 0;
	}
	CShowCoordDlg dlg;
	int ret=dlg.Msg(data);
	if(verbose)
		printf("wxShowCoordDlg() => %d\n",ret);
	fflush(stdout);
	return ret;
}

/* C wrapper for Coordinates dialog */
extern "C" int wxShowMapDlg(data_dialog_t * data)
{
	if(data->dialog_type==DIALOG_CAL_POINT)
		return 0;
	CShowMapDlg dlg;
	int ret=dlg.Msg(data);
	if(verbose)
		printf("wxShowMapDlg() => %d\n",ret);
	return ret;
}
/* C wrapper for File Open dialog */
extern "C" int wxShowOpenFileWindowWin(void)
{
	COpenSaveFile OFile;
	wxString tmp=OFile.Open();		
	if(tmp!=_(""))
		return 1;
	else
		return 0;
		
}
/* C wrapper for File Open dialog */
extern "C" int wxShowSaveWindowWin(void)
{
	COpenSaveFile OFile;
	wxString tmp=OFile.Save();		
	if(tmp!=_(""))
		return 1;
	else
		return 0;
		
}

/************** Application class **********************/
class ImgCalKapApp: public wxApp
{
	public:
	virtual bool OnInit();
};

/*************  PROGAM ENTRY POINT ********************/

IMPLEMENT_APP(ImgCalKapApp);
bool ImgCalKapApp::OnInit()
{
	int ret;
	if(wxGetApp().argc==2)
	{
		char p[200];
		wxString file=wxGetApp().argv[1];
		strcpy(p,file.mb_str(wxConvUTF8));
		ret=imgcalkap_main(p);
		/* If called with command line parameter, exit after calibrating the map */
		exit(0);
	}
	/*
	If I'm here, is because I'm not showing the map;
		so, start over again
	*/
	while(1)
	{
		ret=imgcalkap_main(NULL);
		if(verbose)
			printf("ret:%d",ret);
		if(ret < 0)
		{
			if(verbose)
				printf("EXIT\n");
			exit(0);
		}
	}
	return true;
}
