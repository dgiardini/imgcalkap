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
#include <cv.h>	
#include <highgui.h>
#ifdef _OPENMP
#include <omp.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "imgcalkap.h"
#include "imgcalkap_png.h" // splash coded as base64
#include "base64.h"
#include "points.h"
#include "proc.h"
#include "keys.h"
#include "imgkap.h"
#ifdef COMPILE_LINUX
#endif

char * tmp_filename_header;
char * tmp_filename_out;

char *img_filename=NULL;

IplImage* img_map;
IplImage* img_map_orig;
IplImage* img_zoom;
IplImage* img_start=NULL;
CvSize    mapsize;
CvSize    mapsize_orig;

char datum[NDATUM][BUFF_LEN]={"WGS84","WGS72","Unknown"};
char projection[NPROJ][BUFF_LEN]={"Mercator","Transverse Mercator","Unknown"};
char depth[NDEPTH][BUFF_LEN]={"Meters","Feet","Fathoms","Unknown"};
char verbose=0;
float win_factor;
int gh,gw; // Size off map window
int global_scale_factor;

void StartWinMouseCallbak(int event, int x, int y, int flags, void* param )
{
	switch(event)
	{
		case CV_EVENT_LBUTTONDOWN: 
		case CV_EVENT_RBUTTONDOWN:
			if(CallOpenFileDialog())	
				img_filename=strdup(data_dialog.filename);
			break;
		default:   break;
	}
}

int imgcalkap_main(char* filename)
{
	int c,typeheader,file_is_kap=0;
	char buff[100];
	char * tmpname;
	IplImage * img_tmp;
	tmpname=tmpnam(NULL);
	global_scale_factor=1;
	/* Create names for the temporary files */
	tmp_filename_header=malloc(strlen(tmpname)+4);
	tmp_filename_out=malloc(strlen(tmpname)+4);
	strcpy(tmp_filename_header,tmpname);
	strcpy(tmp_filename_out,tmpname);
	strcat(tmp_filename_header,"txt");
	strcat(tmp_filename_out,"png");
	if(verbose)
		printf("tmp header:%s tmp png:%s\n",tmp_filename_header,tmp_filename_out );

#ifdef COMPILE_LINUX
	gtk_disable_setlocale();
#endif	
	for(c=0;c<MAX_CAL_POINTS;c++)
	{
		CleanCalPoint(c,POINT_CAL);
		CleanCalPoint(c,POINT_PLY);
	}
	SetChartDataDefaults();
	if(filename==NULL)
	{
		if(img_filename!=NULL)
		{
			free(img_filename);
			img_filename=NULL;
		}
		img_start=CreateSplash();
		cvNamedWindow(START_WINDOW,CV_WINDOW_AUTOSIZE);
		cvShowImage(START_WINDOW,img_start);
		DisableXButton(START_WINDOW);

		// A callbak to get the mouse clicks and open the file open dialog 
		cvSetMouseCallback(START_WINDOW,StartWinMouseCallbak,NULL );
		while(img_filename==NULL)
		{
			c=cvWaitKey(100);
			#ifdef COMPILE_LINUX	
			c&=0xffff;
			#endif	
			if(c==KEY_ESC)
			{
				if(ShowMsg("Notice","Are you sure to exit ?", T_YESNO))
					return -1; // tell gui we want to exit
			}
			else if (c==KEY_F1)	
			{
				disp_help();
			}
		}
	}
	else
	{
		img_filename=filename;
	}
	/* Close splash */
	cvDestroyWindow(START_WINDOW);
	cvReleaseImage(&img_start);
	img_start=NULL;

	/* Look for the file type */
	typeheader = findfiletype(img_filename);
	if (typeheader ==  FIF_KAP)// Is a kap file
	{
		/* The retrieved  data will be stored in the data_dialog structure */
		int ret;
		if(verbose)
			printf( "KAP file detected\n");
//		ret = kaptoimg(typeheader,img_filename,FIF_TXT,TMP_FILE_HEADER,13,TMP_FILE_OUT,0); // 13 for png
		ret = kaptoimg(typeheader,img_filename,FIF_TXT,tmp_filename_header,13,tmp_filename_out,0); // 13 for png
		if(ret) // something wrong
		{
			return 2;
		}
		/* See if we can handle this file */
		for(c=0;c<NDATUM;c++)
		{
			if(!stricmp(data_dialog.datum,datum[c])) // Known datum
				break;
		}
		if(c==NDATUM)
		{
			sprintf(buff,"Can't handle datum %s. Switching to UNKNOWN",data_dialog.datum);
			strcpy(data_dialog.datum,"UNKNOWN");
			ShowMsg("Warning",buff,T_OK);
		}
		for(c=0;c<NPROJ;c++)
		{
			if(!stricmp(data_dialog.proj,projection[c])) // Known datum
				break;
			if(c==NPROJ)
			{
				sprintf(buff,"Can't handle projection %s. Switching to UNKNOWN",data_dialog.datum);
				strcpy(data_dialog.proj,"UNKNOWN");
				ShowMsg("Warning",buff,T_OK);
			}
		}
		for(c=0;c<NDEPTH;c++)
		{
			if(!stricmp(data_dialog.depth_units,depth[c])) // Known datum
				break;
		}
		if(c==NDEPTH)
		{
			sprintf(buff,"Can't handle dapth units %s. Switching to UNKNOWN",data_dialog.depth_units);
			strcpy(data_dialog.depth_units,"UNKNOWN");
			ShowMsg("Warning",buff,T_OK);
		}
		// Let's go
		img_filename=strdup(tmp_filename_out); // ready for reading the image
		file_is_kap=1;
	}
    img_tmp=cvLoadImage(img_filename, 1 );
    if(!img_tmp)
    {
	      return 1; // Error loading image
    }
	if(verbose)
		printf("%s loaded\n",img_filename);
	mapsize=cvGetSize(img_tmp);
	/* Save image size */
	mapsize_orig=mapsize;
	if(mapsize.width*mapsize.height > MAX_PICTURE_SIZE)
	{
		/* Search for a factor to resize the picture to something we can handle in memory */
		while((mapsize.width*mapsize.height/(global_scale_factor*global_scale_factor)) > MAX_PICTURE_SIZE)
		{
			global_scale_factor++;
			mapsize.width/=global_scale_factor;
			mapsize.height/=global_scale_factor;
		}
		if(verbose)
			printf("Image too big, resizing to %dx%d. factor:%d\n",mapsize.width,mapsize.height,global_scale_factor);
		/* Now, resize the image */
		img_map=cvCreateImage(mapsize,img_tmp->depth,img_tmp->nChannels );
		cvResize(img_tmp,img_map,CV_INTER_CUBIC);
		cvReleaseImage(&img_tmp);
	}
	else
	{
		// Just copy the image pointer
		img_map=img_tmp;
	}
    /* Set boundaries for image file. For a kap file we already
	have the boundaries in the file retieved by kaptoimg()   */
    if((!file_is_kap) || (!CountValidCalPoints(POINT_PLY)))
    {
	    if(verbose)
		    printf("No valid PLY points, creating defaults\n");
	    InsertPoint(0,0,POINT_PLY);
	    InsertPoint(mapsize.width,0,POINT_PLY);
	    InsertPoint(mapsize.width,mapsize.height,POINT_PLY);
	    InsertPoint(0,mapsize.height,POINT_PLY);
    }
    else
    {
		int j;
	    /* Calculate the position x,y of each boundary coordinate */
	    PLYCoordLatLonToXY(mapsize.width,mapsize.height);
		//PLYCoordXYtoLatLon();
		/* Move the calibration marks because the resizing */
		for(j=0;j<MAX_CAL_POINTS;j++)
		{
			if(cal_points[j].has_data)		
			{
				cal_points[j].x/=global_scale_factor;
				cal_points[j].y/=global_scale_factor;
			}
		}
	}
    /* Keep a clean copy of image */
	if(verbose)
		printf("about to copy %s...\n",img_filename);
    img_map_orig=cvCreateImage(mapsize,img_map->depth,img_map->nChannels );
    cvCopy(img_map,img_map_orig,NULL);
	if(verbose)
		printf("%s copied in memory\n",img_filename);
    win_factor=(float)mapsize.width/(float)mapsize.height;
    // Resize window to defaults
    // Resize to width
    gw=mapsize.width; 
    gh=mapsize.height; 
    if(mapsize.width > DEFAULT_WINDOW_WIDTH)
    {
	    gw=DEFAULT_WINDOW_WIDTH;
	    gh=gw/win_factor;
    }
    // Resize to height
    if( gh > DEFAULT_WINDOW_HEIGHT)
    {
	    gh=DEFAULT_WINDOW_HEIGHT;
	    gw=gh*win_factor;
    }
	if(verbose)
		printf("gw:%d gh:%d\n",gw,gh);
    /* Main window image */
    cvNamedWindow(MAIN_WINDOW,CV_WINDOW_NORMAL);
    cvShowImage(MAIN_WINDOW,img_map);
    cvResizeWindow(MAIN_WINDOW,gw,gh);
	DisableXButton(MAIN_WINDOW);

    init_globals();
    /* Zoom window */
    img_zoom=cvCreateImage(cvSize(ZOOM_WINDOW_WIDTH,ZOOM_WINDOW_HEIGHT) ,img_map->depth, img_map->nChannels );
    cvNamedWindow(ZOOM_WINDOW,CV_WINDOW_NORMAL|CV_GUI_NORMAL);
    cvShowImage(ZOOM_WINDOW,img_zoom);
    cvResizeWindow(ZOOM_WINDOW,100,100);
	DisableXButton(ZOOM_WINDOW);
    DrawZoomWindow(img_map->width/2,img_map->height/2);
    /* Now set the callbak */
    cvSetMouseCallback(MAIN_WINDOW,on_mouse,NULL );

    while(1)
    {
	    c=cvWaitKey(0);
	    if((c&0xff)==KEY_ESC)
	    {
		  if(ShowMsg("Notice","Stop calibrating this map ?", T_YESNO))
		  {
			  cvDestroyWindow(ZOOM_WINDOW);
			  cvDestroyWindow(MAIN_WINDOW);
			  cvReleaseImage(&img_map);
			  cvReleaseImage(&img_zoom);
			  cvReleaseImage(&img_map_orig);
			  img_map=NULL;
			  return 0;
		  }
	    }
	    ProcessKey(c);
    }
	remove(tmp_filename_header);
	remove(tmp_filename_out);
    return 0;
}
/* Create image from base64 stored image */
IplImage * CreateSplash(void)
{
	char *pout;
	int len;
	CvMat Mat;
	IplImage * splash;
	pout=(char *)malloc(sizeof(imgcalkap_png)); 
	len=decode_mem(imgcalkap_png,pout,sizeof(imgcalkap_png)); // Image decoded  from base64
	Mat=cvMat(1, len, CV_8UC1, pout);
	splash=cvDecodeImage(&Mat,1);
	free(pout);
	return splash;
}

 
