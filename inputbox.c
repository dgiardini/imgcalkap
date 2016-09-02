/* **************************************************************************
 * @version $Id: inputbox.c,v 1.1 2014/09/09 23:00:51 David Exp $
 *
 * File:		CInputBoxTest.c
 * Content:		main source code for CInputBox class
 * Notes:		compiles with C/C++ language rules depending on the
				Inputbox.h macro definitions
 *
 * Copyright(c) 2006 by legolas558
 *
 * https://sourceforge.net/projects/cinputbox
 *
 * This software is released under a BSD license. See LICENSE.TXT
 * You must accept the license before using this software.
 *
 * **************************************************************************/
/****************************** Original LICENSE.TXT ****************************
Copyright (c) 2006, legolas558 <legolas558@users.sourceforge.net>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission from the author.
    * Commercial use is allowed only by a specific written permission from the author.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*************************************************************************************/
/* This file has been heavily modified to get some working dialogs */

#include    <stdio.h>
#include	"inputbox.h"
#include	"proc.h"

#define		CIB_CLASS_NAME "CInputBox558"
#define		CIB_SPAN 10
#define		CIB_LEFT_OFFSET 6
#define		CIB_TOP_OFFSET 4
#define		CIB_WIDTH 250
//#define		CIB_HEIGHT 250
int CIB_HEIGHT;
#define		CIB_HEIGHT_CAL_POINT 160
#define		CIB_HEIGHT_MAP_INFO 240

#define		CIB_BTN_WIDTH 60
#define		CIB_BTN_HEIGHT 20
#define		CIB_ST_OFFSET 0

#define LAT_WIDTH 40
#define MIN_WIDTH 70
#define LBL_WIDTH 60
#define L_SPACE   2
#define LBL_WIDTH_MAP 92
#define EDIT_CNAME_WIDTH 150
#define EDIT_SCALE_WIDTH 50

#define ID_SAVE_AS 15 // hope is unique
OPENFILENAME ofn ;

int ShowSaveWindowWin(void)
{
	ZeroMemory( &ofn , sizeof( ofn));
	ofn.lStructSize = sizeof ( ofn );
	ofn.lpstrFile=data_dialog.filename;
	ofn.nMaxFile = sizeof(data_dialog.filename);
	ofn.lpstrFilter = "Kap file (*.kap)\0*.kap\0All (*.*)\0*.*\0";
	ofn.lpstrDefExt = "kap";
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;
	return GetSaveFileName( &ofn );
}

int ShowOpenFileWindowWin()
{
	ZeroMemory( &ofn , sizeof( ofn));
	ofn.lStructSize = sizeof ( ofn );
	ofn.lpstrFile=data_dialog.filename;
	ofn.nMaxFile = sizeof(data_dialog.filename);
	ofn.lpstrFilter = "Image files (*.bmp;*.tiff;*.png;*.jpg;... ) Kap files (*.kap)\0*.bmp;*.tiff;*.tif;*.png;*.jpg;*.pbm;*.pgm;*.ppm;*.sr;*.ras;*.kap\0All (*.*)\0*.*\0";
	ofn.nFilterIndex =1;
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR;
	return GetOpenFileName( &ofn );
}

LRESULT CALLBACK CIB_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CInputBox	*_this;
	_this = (CInputBox *)GetWindowLong(hWnd, GWL_USERDATA);

	switch (msg) {
		case WM_CREATE:
			_this = (CInputBox *) ((CREATESTRUCT *)lParam)->lpCreateParams;
			SetWindowLong(hWnd, GWL_USERDATA, (long)_this);
			CIB_DEREF(_this, create(DCIB_OBJ(_this) hWnd));
		break;
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDOK:
						if(verbose)
							printf("Ok\n");
						CIB_DEREF(_this, submit(DCIB_OBJ1(_this)));
				case IDCANCEL:
						if((LOWORD(wParam) != IDOK) && verbose)
							printf("Cancel\n");
						CIB_DEREF(_this, close(DCIB_OBJ1(_this)));
					break;
				case ID_SAVE_AS:
					if (ShowSaveWindowWin())
					{
						CIB_DEREF(_this, submit(DCIB_OBJ1(_this)));
						//CIB_DEREF(_this, close(DCIB_OBJ1(_this)));
						CreateKapFile(ofn.lpstrFile);
						//printf("data_dialog.filename:%s\n",data_dialog.filename);
					}
				break;
			}
			break;
		case WM_CLOSE:
			CIB_DEREF(_this, hide(DCIB_OBJ1(_this)));
			return 0;
		case WM_DESTROY:
			CIB_DEREF(_this, destroy(DCIB_OBJ1(_this)));
			break;
	}
	return(DefWindowProc (hWnd, msg, wParam, lParam));
}

CIB_DECL CIB_CONSTRUCTOR(CIB_OBJ HINSTANCE hInst)
{
	WNDCLASSEX			wndInputBox;
	RECT rect;

	memset(this, 0, sizeof(wndInputBox));

	this->hThisInstance = hInst;

	if(data_dialog.dialog_type==DIALOG_CAL_POINT)
		CIB_HEIGHT=CIB_HEIGHT_CAL_POINT;
	else
		CIB_HEIGHT=CIB_HEIGHT_MAP_INFO;
	
	wndInputBox.cbSize					= sizeof(wndInputBox);
	wndInputBox.lpszClassName			= CIB_CLASS_NAME;
	wndInputBox.style					= CS_HREDRAW | CS_VREDRAW;
	wndInputBox.lpfnWndProc				= CIB_WndProc;
	wndInputBox.lpszMenuName			= NULL;
	wndInputBox.hIconSm					= NULL;
	wndInputBox.cbClsExtra				= 0;
	wndInputBox.cbWndExtra				= 0;
	wndInputBox.hInstance				= hInst;
	wndInputBox.hIcon					= LoadIcon(NULL, IDI_WINLOGO);
	wndInputBox.hCursor					= LoadCursor(NULL, IDC_ARROW);
	wndInputBox.hbrBackground			= (HBRUSH)(COLOR_WINDOW);

	RegisterClassEx(&wndInputBox);

	if (this->hwndParent)
		GetWindowRect(this->hwndParent, &rect);
	else
		GetWindowRect(GetDesktopWindow(), &rect);

	this->hwndInputBox = CreateWindow(	CIB_CLASS_NAME, "",
					(WS_BORDER | WS_CAPTION), rect.left+(rect.right-rect.left-CIB_WIDTH)/2,
					rect.top+(rect.bottom-rect.top-CIB_HEIGHT)/2,
					CIB_WIDTH, CIB_HEIGHT, this->hwndParent, NULL,
					this->hThisInstance, this);
	return 0;
}

void	CIB_DECL destroy(CIB_OBJ1) {
	EnableWindow(this->hwndParent, true);
	SendMessage(this->hwndInputBox, WM_DESTROY, 0, 0);
}

CIB_DECL CIB_DESTRUCTOR(CIB_OBJ1)  {
	UnregisterClass(CIB_CLASS_NAME, this->hThisInstance);
	return 0;
}

void	CIB_DECL submit(CIB_OBJ1)
{
	char buff[100];
	if(data_dialog.dialog_type==DIALOG_CAL_POINT)
	{
		GetWindowText(this->hwndEditBoxLat,buff,sizeof(buff)-1);
		data_dialog.lat=atoi(buff);
		GetWindowText(this->hwndEditBoxLatMin,buff,sizeof(buff)-1);
		data_dialog.lat_min=atof(buff);
		GetWindowText(this->hwndEditBoxLong,buff,sizeof(buff)-1);
		data_dialog.lng=atoi(buff);
		GetWindowText(this->hwndEditBoxLongMin,buff,sizeof(buff)-1);
		data_dialog.lng_min=atof(buff);
	}
	else // Map Dialog
	{
		GetWindowText(this->hwndEditChartName,data_dialog.chart_name,sizeof(data_dialog.chart_name)-1);
		GetWindowText(this->hwndEditChartDesc,data_dialog.chart_desc,sizeof(data_dialog.chart_desc)-1);
		GetWindowText(this->hwndEditChartScale,buff,sizeof(buff)-1);
		data_dialog.scale=atol(buff);
		SendMessage(this->hwndEditCmbProj,CB_GETCURSEL,0,0);
		GetWindowText(this->hwndEditCmbProj,data_dialog.proj,sizeof(data_dialog.proj)-1);
		GetWindowText(this->hwndEditCmbDatum,data_dialog.datum,sizeof(data_dialog.datum)-1);
		GetWindowText(this->hwndEditCmbDepth,data_dialog.depth_units,sizeof(data_dialog.depth_units)-1);
		if(verbose)printf("Chart name:%s\n Desc:%s\n Scale:%ld\n Proj:%s\n Datum:%s\n Depth:%s\n",
		data_dialog.chart_name,
		data_dialog.chart_desc,
		data_dialog.scale,
		data_dialog.proj,
		data_dialog.datum,
		data_dialog.depth_units	);
	}
	this->bResult = true;
}

void	CIB_DECL create(CIB_OBJ HWND hwndNew)
{
	HFONT	myFont;
	char buff[200];
	int k;
	this->hwndInputBox = hwndNew;
	myFont = (HFONT )GetStockObject(DEFAULT_GUI_FONT);
	//	SetWindowPos(hwndInputBox, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	// Construct a dialog for enter coordinates of a point
	if(data_dialog.dialog_type==DIALOG_CAL_POINT)
	{
		this->hwndQuery =	CreateWindow("Static", "", WS_CHILD | WS_VISIBLE,
							CIB_LEFT_OFFSET, CIB_TOP_OFFSET,
							CIB_WIDTH-CIB_LEFT_OFFSET*2, CIB_BTN_HEIGHT*2,
							this->hwndInputBox, NULL,
							this->hThisInstance, NULL);
		SendMessage(this->hwndQuery,WM_SETFONT,(WPARAM)myFont,FALSE);
		/* Latitude */
		CreateWindow("Static", "Lat Deg:", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, 
							CIB_LEFT_OFFSET,CIB_TOP_OFFSET + CIB_BTN_HEIGHT*2,
							LBL_WIDTH, CIB_BTN_HEIGHT,
							this->hwndInputBox,	NULL,
							this->hThisInstance, NULL);
		this->hwndEditBoxLat = CreateWindow("Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT| WS_TABSTOP, 
							LBL_WIDTH+L_SPACE,CIB_TOP_OFFSET + CIB_BTN_HEIGHT*2,
							LAT_WIDTH, CIB_BTN_HEIGHT,
							this->hwndInputBox,	NULL,
							this->hThisInstance, NULL);
		SendMessage(this->hwndEditBoxLat,WM_SETFONT,(WPARAM)myFont,FALSE);
		sprintf(buff,"%03d",data_dialog.lat);
		if(verbose > 1 )printf("%s\n",buff);
		SendMessage(this->hwndEditBoxLat,WM_SETTEXT,0,(LPARAM)buff);
		
		CreateWindow("Static", "Lat Min:", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, 
							LBL_WIDTH+2*L_SPACE+LAT_WIDTH, CIB_TOP_OFFSET + CIB_BTN_HEIGHT*2,
							LBL_WIDTH, CIB_BTN_HEIGHT,
							this->hwndInputBox,	NULL,
							this->hThisInstance, NULL);
		this->hwndEditBoxLatMin = CreateWindow("Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT| WS_TABSTOP,
							2*LBL_WIDTH+2*L_SPACE+LAT_WIDTH,CIB_TOP_OFFSET + CIB_BTN_HEIGHT*2,
							MIN_WIDTH, CIB_BTN_HEIGHT,
							this->hwndInputBox,	NULL,
							this->hThisInstance, NULL);
		SendMessage(this->hwndEditBoxLatMin,WM_SETFONT,(WPARAM)myFont,FALSE);
		sprintf(buff,"%f",data_dialog.lat_min);
		if(verbose > 1 ) printf("%s\n",buff);
		SendMessage(this->hwndEditBoxLatMin,WM_SETTEXT,0,(LPARAM)buff);

		/* Longitude */
		CreateWindow("Static", "Lon Deg:", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, 
							CIB_LEFT_OFFSET,CIB_TOP_OFFSET + CIB_BTN_HEIGHT*3+5,
							LBL_WIDTH, CIB_BTN_HEIGHT,
							this->hwndInputBox,	NULL,
							this->hThisInstance, NULL);
		this->hwndEditBoxLong = CreateWindow("Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT| WS_TABSTOP, 
							LBL_WIDTH+L_SPACE,CIB_TOP_OFFSET + CIB_BTN_HEIGHT*3+5,
							LAT_WIDTH, CIB_BTN_HEIGHT,
							this->hwndInputBox,	NULL,
							this->hThisInstance, NULL);
		SendMessage(this->hwndEditBoxLong,WM_SETFONT,(WPARAM)myFont,FALSE);
		sprintf(buff,"%03d",data_dialog.lng);
		if(verbose > 1 ) printf("%s\n",buff);
		SendMessage(this->hwndEditBoxLong,WM_SETTEXT,0,(LPARAM)buff);
		CreateWindow("Static", "Lon Min:", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, 
							LBL_WIDTH+2*L_SPACE+LAT_WIDTH, CIB_TOP_OFFSET + CIB_BTN_HEIGHT*3+5,
							LBL_WIDTH, CIB_BTN_HEIGHT,
							this->hwndInputBox,	NULL,
							this->hThisInstance, NULL);
		this->hwndEditBoxLongMin = CreateWindow("Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT| WS_TABSTOP,
							2*LBL_WIDTH+2*L_SPACE+LAT_WIDTH,CIB_TOP_OFFSET + CIB_BTN_HEIGHT*3+5,
							MIN_WIDTH, CIB_BTN_HEIGHT,
							this->hwndInputBox,	NULL,
							this->hThisInstance, NULL);
		SendMessage(this->hwndEditBoxLongMin,WM_SETFONT,(WPARAM)myFont,FALSE);
		sprintf(buff,"%f",data_dialog.lng_min);
		if(verbose > 1 ) printf("%s\n",buff);
		SendMessage(this->hwndEditBoxLongMin,WM_SETTEXT,0,(LPARAM)buff);
	}
	else // Chart info
	{
	/* Chart name */
	 CreateWindow("Static", "Chart Name:", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, 
	 						CIB_LEFT_OFFSET, CIB_TOP_OFFSET,
							LBL_WIDTH_MAP, CIB_BTN_HEIGHT,
							this->hwndInputBox, NULL,
							this->hThisInstance, NULL);
		this->hwndEditChartName = CreateWindow("Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT| WS_TABSTOP,
							LBL_WIDTH_MAP+L_SPACE,CIB_TOP_OFFSET,
							EDIT_CNAME_WIDTH, CIB_BTN_HEIGHT,
							this->hwndInputBox,	NULL,
							this->hThisInstance, NULL);
		SendMessage(this->hwndEditChartName,WM_SETFONT,(WPARAM)myFont,FALSE);
		strcpy(buff,data_dialog.chart_name);
		if(verbose > 1 ) printf("%s\n",buff);
		SendMessage(this->hwndEditChartName,WM_SETTEXT,0,(LPARAM)buff);
		/* Chart description */
		CreateWindow("Static", "Description:", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, 
	 						CIB_LEFT_OFFSET,CIB_TOP_OFFSET+CIB_BTN_HEIGHT,
							LBL_WIDTH_MAP,CIB_BTN_HEIGHT ,
							this->hwndInputBox, NULL,
							this->hThisInstance, NULL);
		this->hwndEditChartDesc = CreateWindow("Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT| WS_TABSTOP,
							LBL_WIDTH_MAP+L_SPACE,CIB_TOP_OFFSET+CIB_BTN_HEIGHT,
							EDIT_CNAME_WIDTH, CIB_BTN_HEIGHT,
							this->hwndInputBox,	NULL,
							this->hThisInstance, NULL);
		SendMessage(this->hwndEditChartDesc,WM_SETFONT,(WPARAM)myFont,FALSE);
		strcpy(buff,data_dialog.chart_desc);
		if(verbose > 1 ) printf("%s\n",buff);
		SendMessage(this->hwndEditChartDesc,WM_SETTEXT,0,(LPARAM)buff);
		/* Chart scale */
		CreateWindow("Static", "Scale: (1:x)", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, 
	 						CIB_LEFT_OFFSET,CIB_TOP_OFFSET+CIB_BTN_HEIGHT*2,
							LBL_WIDTH_MAP,CIB_BTN_HEIGHT ,
							this->hwndInputBox, NULL,
							this->hThisInstance, NULL);
		this->hwndEditChartScale = CreateWindow("Edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT| WS_TABSTOP,
							LBL_WIDTH_MAP+L_SPACE,CIB_TOP_OFFSET+CIB_BTN_HEIGHT*2,
							EDIT_SCALE_WIDTH, CIB_BTN_HEIGHT,
							this->hwndInputBox,	NULL,
							this->hThisInstance, NULL);
		SendMessage(this->hwndEditChartScale,WM_SETFONT,(WPARAM)myFont,FALSE);
		sprintf(buff,"%ld",data_dialog.scale);
		if(verbose > 1 ) printf("%s\n",buff);
		SendMessage(this->hwndEditChartScale,WM_SETTEXT,0,(LPARAM)buff);
		/* Chart Projection */
		CreateWindow("Static", "Projection:", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, 
	 						CIB_LEFT_OFFSET,CIB_TOP_OFFSET+CIB_BTN_HEIGHT*3,
							LBL_WIDTH_MAP,CIB_BTN_HEIGHT ,
							this->hwndInputBox, NULL,
							this->hThisInstance, NULL);
		this->hwndEditCmbProj = CreateWindow("COMBOBOX", "UNKNOWN", CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP, 
								LBL_WIDTH_MAP+L_SPACE,CIB_TOP_OFFSET + CIB_BTN_HEIGHT*3, 
								100, CIB_BTN_HEIGHT*10,
								this->hwndInputBox,	NULL,
								this->hThisInstance, NULL);
		for (k = 0; k < NPROJ; k ++)
		{
			strcpy(buff,projection[k]);
			// Add string to combobox.
			SendMessage(this->hwndEditCmbProj,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) buff); 
			printf("viendo %s %s\n",projection[k],data_dialog.proj);
			if(!stricmp(projection[k],data_dialog.proj))
			{
				if(verbose)printf("Setting Cursel:%s\n",data_dialog.proj);
				SendMessage(this->hwndEditCmbProj,(UINT) CB_SETCURSEL,(WPARAM) k,0); 
			}

		}						
		SendMessage(this->hwndEditCmbProj,WM_SETFONT,(WPARAM)myFont,FALSE);
		/* Datum */
		CreateWindow("Static", "Datum:", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, 
	 						CIB_LEFT_OFFSET,CIB_TOP_OFFSET+CIB_BTN_HEIGHT*4,
							LBL_WIDTH_MAP,CIB_BTN_HEIGHT ,
							this->hwndInputBox, NULL,
							this->hThisInstance, NULL);
		this->hwndEditCmbDatum = CreateWindow("COMBOBOX", "UNKNOWN", CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP, 
								LBL_WIDTH_MAP+L_SPACE,CIB_TOP_OFFSET + CIB_BTN_HEIGHT*4, 
								100, CIB_BTN_HEIGHT*10,
								this->hwndInputBox,	NULL,
								this->hThisInstance, NULL);
		for (k = 0; k < NDATUM; k ++)
		{
			strcpy(buff,datum[k]);
			// Add string to combobox.
			SendMessage(this->hwndEditCmbDatum,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) buff); 
			if(!stricmp(datum[k],data_dialog.datum))
			{
				if(verbose)printf("Setting Cursel:%s\n",data_dialog.datum);
				SendMessage(this->hwndEditCmbDatum,(UINT) CB_SETCURSEL,(WPARAM) k,0); 
			}
		}						
		SendMessage(this->hwndEditCmbDatum,WM_SETFONT,(WPARAM)myFont,FALSE);

		/* Depth */
		CreateWindow("Static", "Depth Units:", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, 
	 						CIB_LEFT_OFFSET,CIB_TOP_OFFSET+CIB_BTN_HEIGHT*5,
							LBL_WIDTH_MAP,CIB_BTN_HEIGHT ,
							this->hwndInputBox, NULL,
							this->hThisInstance, NULL);
		this->hwndEditCmbDepth = CreateWindow("COMBOBOX", "UNKNOWN", CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP, 
								LBL_WIDTH_MAP+L_SPACE,CIB_TOP_OFFSET + CIB_BTN_HEIGHT*5, 
								100, CIB_BTN_HEIGHT*10,
								this->hwndInputBox,	NULL,
								this->hThisInstance, NULL);
		for (k = 0; k < NDEPTH; k ++)
		{
			strcpy(buff,depth[k]);
			// Add string to combobox.
			SendMessage(this->hwndEditCmbDepth,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) buff); 
			if(!stricmp(depth[k],data_dialog.depth_units))
			{
				if(verbose)printf("Setting Cursel:%s\n",data_dialog.depth_units);
				SendMessage(this->hwndEditCmbDepth,(UINT) CB_SETCURSEL,(WPARAM) k,0); 
			}
		}						
		SendMessage(this->hwndEditCmbDepth,WM_SETFONT,(WPARAM)myFont,FALSE);

		this->hwndSave =	CreateWindow("Button", "Save as...", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					CIB_LEFT_OFFSET, CIB_TOP_OFFSET+CIB_BTN_HEIGHT*7,					
					CIB_BTN_WIDTH+10, 
					CIB_BTN_HEIGHT,
					this->hwndInputBox, (HMENU)ID_SAVE_AS,
					this->hThisInstance, NULL);


	}	
	/* Ok - Cancel */
	this->hwndOk =	CreateWindow("Button", "OK", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					CIB_WIDTH/2 - CIB_SPAN*2 - CIB_BTN_WIDTH, 
					CIB_HEIGHT - CIB_TOP_OFFSET*4 - CIB_BTN_HEIGHT*2,
					CIB_BTN_WIDTH, 
					CIB_BTN_HEIGHT,
					this->hwndInputBox, (HMENU)IDOK,
					this->hThisInstance, NULL);
	this->hwndCancel =	CreateWindow("Button", "Cancel",
					WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					CIB_WIDTH/2 + CIB_SPAN,
					CIB_HEIGHT - CIB_TOP_OFFSET*4 - CIB_BTN_HEIGHT*2,
					CIB_BTN_WIDTH, 
					CIB_BTN_HEIGHT,
					this->hwndInputBox, (HMENU)IDCANCEL,
							this->hThisInstance, NULL);
}

void	CIB_DECL close(CIB_OBJ1) {
	PostMessage(this->hwndInputBox, WM_CLOSE, 0, 0);
}

void	CIB_DECL hide(CIB_OBJ1) {
	ShowWindow(this->hwndInputBox, SW_HIDE);
}

void	CIB_DECL show(CIB_OBJ LPCSTR lpszTitle, LPCSTR	lpszQuery) {
	SetWindowText(this->hwndInputBox, lpszTitle);
	SetWindowText(this->hwndQuery, lpszQuery);
	SendMessage(this->hwndEditBoxLat, EM_LIMITTEXT, this->wInputMaxLength, 0);
	SendMessage(this->hwndEditBoxLat, EM_SETSEL, 0, -1);
	SendMessage(this->hwndEditBoxLong, EM_LIMITTEXT, this->wInputMaxLength, 0);
	SendMessage(this->hwndEditBoxLong, EM_SETSEL, 0, -1);
	SetFocus(this->hwndEditBoxLat);
	ShowWindow(this->hwndInputBox, SW_NORMAL);
}

int CIB_DECL ShowInputBox(CIB_OBJ HWND hwndParentWindow, LPCTSTR lpszTitle, LPCTSTR lpszQuery, LPSTR szResult, WORD wMax)
{
	MSG	msg;
	BOOL	bRet;
	this->hwndParent = hwndParentWindow;
	this->szInputText = szResult;
	this->wInputMaxLength = wMax;
	this->bResult = false;
//	EnableWindow(hwndParent, false);
	CIB_DEREF(this, show(DCIB_OBJ(this) lpszTitle, lpszQuery));
	
	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
	{

		if (msg.message==WM_KEYDOWN) {
			switch (msg.wParam) {
			//case VK_RETURN:
			//	CIB_DEREF(this, submit(DCIB_OBJ1(this)));
			case VK_ESCAPE:
				CIB_DEREF(this, close(DCIB_OBJ1(this)));
				break;
			default:
				if (IsDialogMessage(this->hwndInputBox, &msg)) 
					break;
				else
					TranslateMessage(&msg);
				break;
			}
		} else
//		if (!IsDialogMessage(this->hwndInputBox, &msg)) {
			TranslateMessage(&msg);
//		}
		DispatchMessage(&msg);	
		if (msg.message == WM_CLOSE)
			break;
	}
//	EnableWindow(hwndParent, true);
    return this->bResult;
}

