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
#ifndef __IMGCALKAP_H_INC
#define __IMGCALKAP_H_INC
#include <cv.h>

#ifdef COMPILE_LINUX
	#define stricmp strcasecmp
#endif

#define IMGCALKAPVERSION "v0.2 - 09/2014"

#ifdef __cplusplus
extern "C" {
#endif

//#define MAIN_WINDOW "Map Calibration - Press F1 for help "
//#define ZOOM_WINDOW "Zoom Area"
#define MAIN_WINDOW "Map Calibration Window - Press F1 for help"
#define ZOOM_WINDOW "Zoom"
#define START_WINDOW "ImgCalKap - Press F1 for help"
#define START_SCREEN_IMG "imgcalkap.pngx"

// Actual size of little zoom window
#define ZOOM_WINDOW_WIDTH  50
#define ZOOM_WINDOW_HEIGHT 50

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600

#define MAP_ZOOM_FACTOR 2
#define NEAR_POINT_DISTANCE 50 		// pixels for to no overlap points
#define MAX_CAL_POINTS 200

#define DIALOG_CAL_POINT 1
#define DIALOG_CHART     2
#define T_OK    1
#define T_YESNO 2

#define NDATUM   3
#define NPROJ    3
#define NDEPTH   4
#define BUFF_LEN 50

#define POINT_CAL 0
#define POINT_PLY 1

#ifdef IMGCALKAP_AS_LIB
int imgcalkap_main(char * filename);
#endif

#define MAX_PICTURE_SIZE (8192*6144) // Reference picture size
//#define MAX_PICTURE_SIZE (800*600) // Reference picture size
extern int global_scale_factor; // To resize images to seomething we can handle
extern char datum[NDATUM][BUFF_LEN];
extern char projection[NPROJ][BUFF_LEN];
extern char depth[NDEPTH][BUFF_LEN];
extern char verbose;
extern char verbose;
extern float win_factor;

typedef struct
{
	int x;
	int y;
	CvRect prev_mark_roi; 
	int    lat;
	double lat_min;
	int    lng;
	double lng_min;
	double lat_dec;
	double lng_dec;
	double phi;
	char has_data;
	char is_extra;
	char selected;
	char  type; // callibration point or ply (boundary)
}cal_point_t;

typedef struct
{
	char	title[100];
	char	comment[200];
//	int		lat;
//	double	lat_min;
//	int    	lng;
//	double 	lng_min;
	double  input_lat;
	double  input_lon;
	char 	input_str[100];
	long 	scale;
	int     chart_number;
	char 	chart_name[100];
	char 	chart_desc[100];
	char 	depth_units[20];
	char 	datum[100];
	char 	proj[100];
	char 	dialog_type;
	char    filename[250];
}data_dialog_t;

extern char * img_filename;
extern IplImage* img_map;
extern IplImage* img_zoom;
extern IplImage* img_map_orig;
extern IplImage* img_start;

extern CvSize mapsize;
extern CvSize mapsize_orig;

extern int gx,gy;
extern CvSize zsize;
extern CvRect roi; // actual region of interest of map image
extern cal_point_t cal_points[];
extern cal_point_t ply_points[];
extern CvPoint *PlyMarks;
extern int nPly;

extern int calib_mark_dim;
extern data_dialog_t data_dialog;
extern int gh,gw;
IplImage * CreateSplash(void);
void StartWinMouseCallbak(int event, int x, int y, int flags, void* param );

#ifdef __cplusplus
}
#endif

#endif