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
#ifndef _IMGCALKAP_INC
#define _IMGCALKAP_INC
#include "imgcalkap.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UPDATE_ALL 			0
#define UPDATE_NO_SHADOW    1

void on_mouse(int event, int x, int y, int flags, void* param );
void init_globals(void);
void ProcessMapDrag(int startx, int starty,int endx ,int endy);	
void SetChartDataDefaults(void);
void ProcessKey(int c);
void UpdateMapWindow(char mode);
void DrawZoomWindow(int x, int y);
void CreateKapFile(char * filename);
void ShowCalPointDialog(int wich);
void ShowMapDialog(void);
void ProcessMapDrag(int startx, int starty,int endx ,int endy);
void ProcessMarkDrag(int startx, int starty,int endx ,int endy,int which);
void ProcessPlyMarkDrag(int startx, int starty,int endx ,int endy,int which);
void CheckBoundaries(CvRect * roi);
int ShowMsg(char * title, char * msg, char type);
int CallOpenFileDialog(void);
void ProcessPolyLineBounds(void);
void ShadowOutsideOfBounds(CvPoint * points,int npoints);
void DrawCalibMark(int x,int y,char has_data,CvRect * prev_mark_roi,char only_clean);
void disp_help(void);
char * strtoupper(char *str);
void CheckClosedWindow(char * name,IplImage * img);
void DisableXButton(char * name);
void ProcessMouseWheel(int direction);

#ifdef __cplusplus
}
#endif
#endif