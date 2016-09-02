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
#ifndef __POINTS_IMGCALKAP_H_INC
#define __POINTS_IMGCALKAP_H_INC
#ifdef __cplusplus
extern "C" {
#endif

void CleanCalPoint(int j, int type);
int CountValidCalPoints(int type);
void OrderCalPoints(int type);
int CheckNearPoint(int x,int y,int type);
void InsertPoint(int x,int y,int type);
void AddMarkAtCursor(int type);
void DeleteCalPoint(int which_one,int type);
int UnDrawCalPoint(int type);
void DeleteExtraCalPoints(int type);
double CalcAngle(int x, int y);
void PLYCoordXYtoLatLon(double factor);
void PLYCoordLatLonToXY(int w,int h);
void SaveCalData(void);
void RetrieveCalData(void);
char * strtoupper(char *str);
#ifdef __cplusplus
}
#endif

#endif
