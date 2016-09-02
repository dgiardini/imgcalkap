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
#include <stdio.h>
#include "proc.h"

/***** Windows handlers **************/
#ifndef COMPILE_LINUX

#include <windows.h>
HHOOK MouseHook;
LRESULT CALLBACK LowLevelMouseProc (int code, WPARAM wParam, LPARAM lParam);
/* Function called when init the main application */
void SetMouseWheelCallback(void)
{
//	HANDLE Proc= GetCurrentProcess();
	MouseHook = SetWindowsHookEx(WH_MOUSE_LL, &LowLevelMouseProc,GetModuleHandle(0),0);

}
/* Windows procedure to hook mouse events ans filter mouse wheel events for zooming */
LRESULT CALLBACK LowLevelMouseProc (int code, WPARAM wParam, LPARAM lParam)
{
	MSLLHOOKSTRUCT *mouseInfo;
	int mouse_dir=0;
	HWND focus=GetFocus();
	/* Don't process mouse wheel events from other windows */
	if(focus==NULL)
		return 0;

    if (code == HC_ACTION)
    {
		/* Filter only mouse wheel events */
        switch (wParam)
        {
            case WM_MOUSEWHEEL: 
					    mouseInfo = (MSLLHOOKSTRUCT*)lParam;
						if((long int)(mouseInfo->mouseData) > 0)
						{
							mouse_dir=1;
						}
						else
						{
							mouse_dir=0;
						}
						break;
			default: 
				/* Go to default hook */
				return CallNextHookEx(MouseHook, code, wParam, lParam); 
				break;
        }
		if(verbose)
			printf("MouseWheel event. Mouse direction:%d\n",mouse_dir);
		ProcessMouseWheel(mouse_dir); // in proc.c
    }
	return 0;
}
#else
/***************** LINUX WORLD ***********************/
#include "gtk/gtk.h"
#include "gdk/gdkkeysyms.h"
#include <stdio.h>
static void gd_scroll_event(GtkWidget *widget, GdkEventScroll *scroll, void * s);
void SetMouseWheelCallback(void)
{
	GtkWidget * widget;
	widget=(GtkWidget * )cvGetWindowHandle(MAIN_WINDOW);
	g_signal_connect(widget, "scroll-event", G_CALLBACK(gd_scroll_event),NULL);
}

static void gd_scroll_event(GtkWidget *widget, GdkEventScroll *scroll, void * s)
{
	s=s;
    int mouse_dir;
    if (scroll->direction == GDK_SCROLL_UP) 
	{
        mouse_dir=1;
    }
	else if (scroll->direction == GDK_SCROLL_DOWN) 
	{
        mouse_dir=0;
    } 
	else
	{
       return ; // No scroll
	}
	ProcessMouseWheel(mouse_dir); // in proc.c
	return;
}
#endif
