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
#include <stdlib.h>
#include <highgui.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "imgcalkap.h"
#include "points.h"
#include "proc.h"
#include "keys.h"
#include "imgkap.h"
#include "base64.h"
#include "gui/gui.h"
#include "mousewheel.h"

#ifdef COMPILE_LINUX
	#include "gtk/gtk.h"
	#include "gdk/gdkkeysyms.h"
#endif
double millis(void)
{
  static struct timeval  tv;  
  gettimeofday(&tv, NULL);
  return ((tv.tv_sec) * 1000 + (tv.tv_usec) / 1000); 
}
//#define TIME_DEBUG
void millis_dbg(char * msg)
{
#ifndef TIME_DEBUG	
	return ;
#else	
  static int pvez=1;
  static double ptime;
  double ahora;
  double ret;
  if(pvez)
  {
      ptime=millis();
      pvez=0;
  }
  ahora=millis();
  ret=(ahora-ptime);
  ptime=ahora;
  if(msg!=NULL)
  {
      if(msg[0]==0)	return; 
      printf("------%s:",msg);
      printf(" %f ms ----- \n",ret);
  }
  else
      printf("............\n");
  fflush(stdout);
  return;
  #endif
}

/* Draw the little window with a zoomed area and a cross in it */
int gx,gy;
int actual_zoom=0;
CvRect roi;
data_dialog_t data_dialog;
int calib_mark_dim;
int ctrl_pressed=0;
double update_time_map=0;
#define COLOR_TEXTO CV_RGB(0xFF,0x00,0X00)
CvFont font;
#define CMD_IMGKAP "imgkap.exe"
#define CALIB_MARK_WIDTH_FACTOR .05

void init_globals(void)
{
	/* Set a handler for mouse wheel events */
	SetMouseWheelCallback();
	roi.x=0;roi.y=0;
	roi.width=mapsize.width;
	roi.height=mapsize.height;
	calib_mark_dim= roi.width*CALIB_MARK_WIDTH_FACTOR/win_factor;
	//RetrieveCalData();	
	if(verbose > 1)	
	   cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX ,5,5,0,3,CV_AA);
	UpdateMapWindow(UPDATE_ALL);
}
void SetChartDataDefaults(void)
{
	memset(&data_dialog,0,sizeof(data_dialog));
	data_dialog.scale=50000;
	data_dialog.chart_number=1;
	strcpy(data_dialog.datum,datum[0]); // some default
	strcpy(data_dialog.proj,projection[0]); // some default
	strcpy(data_dialog.depth_units,depth[0]); // some default
}
/* 
	This function must return the data_dialog structure 
    filled with the propper fields 
*/
int CallInputDialog(void)
{
	int res;
	if(data_dialog.dialog_type==DIALOG_CAL_POINT)
		res=wxShowCoordDlg(&data_dialog);
	else
		res=wxShowMapDlg(&data_dialog);
	return res;
}
void CallSaveFileDialog(void)
{
	int res;
	if(CountValidCalPoints(POINT_CAL)<2)
	{
		ShowMsg("Error","Introduce at least two calibration points",T_OK);
		return;
	}
	res=wxShowSaveWindowWin();
	if(res)
		CreateKapFile(data_dialog.filename);
}

int CallOpenFileDialog(void)
{
	int res;
	res=wxShowOpenFileWindowWin();
	return res;
}

int ShowMsg(char * title, char * msg, char type)
{
	return wxShowMsg(title,msg,type);
}

/* Lat/Long where the chart is valid */
double CalcProjectionParameter(char * proj)
{
	double maxl=-999, minl=999;
	int j;
	
	if(!stricmp(proj,"MERCATOR"))
	{
		for(j=0;j<MAX_CAL_POINTS;j++)
			if(cal_points[j].has_data)
			{
				if(cal_points[j].lat_dec < minl)
					minl=cal_points[j].lat_dec;
				if(cal_points[j].lat_dec > maxl)
					maxl=cal_points[j].lat_dec;
			}
	}
	else if(!stricmp(proj,"TRANSVERSE MERCATOR"))
	{
		for(j=0;j<MAX_CAL_POINTS;j++)
			if(cal_points[j].has_data)
			{
				if(cal_points[j].lng_dec < minl)
					minl=cal_points[j].lng_dec;
				if(cal_points[j].lng_dec > maxl)
					maxl=cal_points[j].lng_dec;
			}
	}
	else
	{
	 return 0.0;  // Unknown
	}
	if(verbose)
		printf("PP: proj %s->%f minl=%f maxl=%f \n",proj,(maxl+minl)/2.0,minl,maxl);
	return (maxl+minl)/2.0;
}
void GetChartBounds(void)
{
	int j,k=0;
	if(PlyMarks==NULL)
		PlyMarks=(CvPoint *)malloc(MAX_CAL_POINTS * sizeof(CvPoint));
	for(j=0;j<MAX_CAL_POINTS;j++)
	{
		if(ply_points[j].has_data)
		{
			/* x,y coordinates of the point for the given mark */
			PlyMarks[k]=cvPoint(ply_points[j].x,ply_points[j].y);
			k++;
			if(verbose > 2)
				printf("PolyLinePoint:%d,%d\n",ply_points[j].x,ply_points[j].y);
		}
	}
	nPly=k;
}

void DrawAllMarks(void)
{
	int j;
	for(j=0;j<MAX_CAL_POINTS;j++)
		if(cal_points[j].x!=-1)
		{
			char np[5];
			sprintf(np,"%d",j);
			//cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX ,font_scale,font_scale,0,thickness,ftype);
			DrawCalibMark(cal_points[j].x,cal_points[j].y,cal_points[j].has_data,&cal_points[j].prev_mark_roi,0);
			if(verbose > 1) cvPutText(img_map, np,cvPoint(cal_points[j].x,cal_points[j].y),&font,COLOR_TEXTO);
		}
    millis_dbg("DrawAllMarks()");
}
void DrawAllCircles(void)
{
	int j=0;
	int thickness,radius;
	GetChartBounds();
	CvScalar color=CV_RGB(0x00,0x00,0xFF);
	thickness=calib_mark_dim/64;
	if(thickness<1)thickness=1;
	radius=calib_mark_dim/4;
	for(j=0;j<nPly;j++)
	{
//		char np[5];
	//	sprintf(np,"%d",j);
		//cvPutText(img_map, np,PlyMarks[j],&font,COLOR_TEXTO);
		cvCircle(img_map,PlyMarks[j],radius,color,thickness,CV_AA,0);
	}
      millis_dbg("DrawAllCircles()");

}
void DrawChartBounds(char mode)
{
	int j=0;
	int thickness,radius;
	thickness=calib_mark_dim/64;
	if(thickness<1)thickness=1;
	radius=calib_mark_dim/4;
	/* Override the marks with the original image */
	for(j=0;j<MAX_CAL_POINTS;j++)
	{
		if(ply_points[j].has_data)
		{
			if(ply_points[j].prev_mark_roi.x != -1 ) // has previous mark
			{
				if(verbose > 2) printf("Override old ply mark, %d,%d %d %d\n",ply_points[j].prev_mark_roi.x,ply_points[j].prev_mark_roi.y,ply_points[j].prev_mark_roi.width,ply_points[j].prev_mark_roi.height);
				if((ply_points[j].prev_mark_roi.width > 0)  && (ply_points[j].prev_mark_roi.height > 0))
				{
					cvSetImageROI(img_map,ply_points[j].prev_mark_roi);
					cvSetImageROI(img_map_orig,ply_points[j].prev_mark_roi);
					cvCopy(img_map_orig,img_map,NULL);
					cvResetImageROI(img_map);
					cvResetImageROI(img_map_orig);
				}
			}
			/* Store the area to draw for override next time */
			ply_points[j].prev_mark_roi.width=2*(radius+thickness+1);
			ply_points[j].prev_mark_roi.height=2*(radius+thickness+1);
			ply_points[j].prev_mark_roi.x=ply_points[j].x-ply_points[j].prev_mark_roi.width/2;
			ply_points[j].prev_mark_roi.y=ply_points[j].y-ply_points[j].prev_mark_roi.height/2;
			CheckBoundaries(&ply_points[j].prev_mark_roi);
		}
	}

	/* Dark outside chart boundaries */
/*	if(mode!=UPDATE_NO_SHADOW)
	{
		GetChartBounds();
		ShadowOutsideOfBounds(PlyMarks,nPly);
		//cvPolyLine(img_map, &PlyMarks, &nPly, 1, 1, color,thickness,CV_AA, 0);
	}
	*/	
	millis_dbg("DrawChartBounds()");

}
/* Very inefficient, something to improve */
void ShadowOutsideOfBounds(CvPoint * points,int npoints)
{
	CvPoint p1,p2;

	IplImage * mask_tmp=cvCreateImage(mapsize,img_map->depth, img_map->nChannels );
	cvZero(mask_tmp);
	/* white image */
	p1=cvPoint(0,0);
	p2=cvPoint(mapsize.width,mapsize.height);
	cvRectangle(mask_tmp,p1,p2,CV_RGB(40,40,40),-1,8,0);
	cvFillPoly(mask_tmp, &points, &npoints, 1, CV_RGB(0,0,0),8,0);
	cvSub(img_map_orig,mask_tmp,img_map,0);
	cvReleaseImage(&mask_tmp);
}

void DrawCalibMark(int x,int y,char has_data,CvRect * prev_mark_roi,char only_clean)
{
	int x0,y0,x1,y1;  // Limits of square
	int new_x,new_y;
	int mid,thickness;
	int xmark,ymark;
	CvScalar color;

	// Try to keep the aspect of the calibration mark independent from zoom
	mid=calib_mark_dim/2;
	thickness=mid/8;
	x0=x-calib_mark_dim/2;
	xmark=x0+ mid;
	if(x0<0) x0=0;
	x1=x+calib_mark_dim/2;
	if(x1>mapsize.width)
		x1=mapsize.width;
	y0=y-calib_mark_dim/2;
	ymark=y0 + mid;
	if(y0<0)
		y0=0;
	y1=y+calib_mark_dim/2;
	if(y1>mapsize.height)
		y1=mapsize.height;
	if(has_data)
		color=CV_RGB(0,255,0);
	else
		color=CV_RGB(255,0,0);
	
	new_x=MAX(x0-thickness,0); 
	new_y=MAX(y0-thickness,0); 
	
	// Override the mark with the original image
	if(prev_mark_roi->x != -1 ) // has previous mark
	{
		if(verbose > 2) printf("Override old mark, %d,%d %d %d\n",prev_mark_roi->x,prev_mark_roi->y,prev_mark_roi->width,prev_mark_roi->height);
		if((prev_mark_roi->width > 0)  && (prev_mark_roi->height > 0))
		{
			cvSetImageROI(img_map,*prev_mark_roi);
			cvSetImageROI(img_map_orig,*prev_mark_roi);
			cvCopy(img_map_orig,img_map,NULL);
			cvResetImageROI(img_map);
			cvResetImageROI(img_map_orig);
		}
	}
	//calib_mark_dim= roi.width*CALIB_MARK_WIDTH_FACTOR/win_factor;
	/* Store the region of mark to be overridden next time */	
	prev_mark_roi->x=new_x;
	prev_mark_roi->y=new_y;
	prev_mark_roi->width=MAX(x1-x0+2*thickness+1,0);
	prev_mark_roi->height=MAX(y1-y0+2*thickness+1,0);
	
	if(!only_clean) 
	{
		// Must draw the marks
		// Squares
		cvLine(img_map,cvPoint(x0,y0),cvPoint(x1,y0),color,thickness,1,0);
		cvLine(img_map,cvPoint(x1,y0),cvPoint(x1,y1),color,thickness,1,0);
		cvLine(img_map,cvPoint(x1,y1),cvPoint(x0,y1),color,thickness,1,0);
		cvLine(img_map,cvPoint(x0,y1),cvPoint(x0,y0),color,thickness,1,0);
		// mid lines
		if(xmark >0) //vertical
		{
			cvLine(img_map,cvPoint(xmark,y0),cvPoint(xmark,y1),color,thickness,1,0);
		}
		if(ymark >0) //horizontal
		{
			cvLine(img_map,cvPoint(x0,ymark),cvPoint(x1,ymark),color,thickness,1,0);	
		}
	}
	if(verbose > 1) printf("Calib Mark drawed at %d,%d %d,%d\n",x0,y0,x1,y1);
	
}
void UpdateMapWindow(char mode)
{
	double ahora=millis();
	millis_dbg(0);
	cvResetImageROI(img_map);
	DrawChartBounds(mode);
	DrawAllMarks();
	DrawAllCircles();
	cvSetImageROI(img_map,roi);
	millis_dbg("SetImageROI()");
	cvShowImage(MAIN_WINDOW,img_map);
	millis_dbg("ShowImage()");
	cvResetImageROI(img_map);
	millis_dbg("UpdateMapWindow()");
	update_time_map=1.1*(millis()-ahora);
}

void DrawZoomWindow(int x, int y)
{
	int x0,y0,x1,y1;
	if(cvGetWindowHandle(ZOOM_WINDOW)==NULL)
	  return;
	// Becasue the image could be zoomed, the x,y coordinates
	// relatives to the actual zoom level must be translated
	// to the original picture coordinates
	x+=roi.x;	y+=roi.y;
	// Get the zoom rectangle around x,y; check limits first
	x0=x-ZOOM_WINDOW_WIDTH/2;
	x1=x+ZOOM_WINDOW_WIDTH/2;
	if(x0<0)
	{
		x0=0;  
		x1=ZOOM_WINDOW_WIDTH;
	}
	if(x1>mapsize.width)
	{
		x1=mapsize.width;  
		x0=mapsize.width-ZOOM_WINDOW_WIDTH;
	}
	y0=y-ZOOM_WINDOW_HEIGHT/2;
	y1=y+ZOOM_WINDOW_HEIGHT/2;
	if(y0<0)
	{
		y0=0;  
		y1=ZOOM_WINDOW_HEIGHT;
	}
	if(y1>mapsize.height)
	{
		y1=mapsize.height;  
		y0=mapsize.height-ZOOM_WINDOW_HEIGHT;
	}
	if(verbose > 2) printf("x:%d y%d gx*zoom:%d gy*zoom:%d \n",x,y,x*(actual_zoom+1),y*(actual_zoom+1));
	cvSetImageROI(img_map_orig,cvRect(x0,y0,ZOOM_WINDOW_WIDTH,ZOOM_WINDOW_HEIGHT));
	cvCopy(img_map_orig,img_zoom,NULL);
	// Draw a cross
	cvLine(img_zoom,cvPoint(0,ZOOM_WINDOW_HEIGHT/2),cvPoint(ZOOM_WINDOW_WIDTH,ZOOM_WINDOW_HEIGHT/2),CV_RGB(100,100,100),0,1,0);
	cvLine(img_zoom,cvPoint(ZOOM_WINDOW_WIDTH/2,0),cvPoint(ZOOM_WINDOW_WIDTH/2,ZOOM_WINDOW_HEIGHT),CV_RGB(100,100,100),0,1,0);
	// Show the zoomed window
	cvShowImage(ZOOM_WINDOW,img_zoom);
	cvResetImageROI(img_map_orig);
}

void on_mouse(int event, int x, int y, int flags, void* param )
{
	static int xdrag_start=0,ydrag_start=0,dragging_map=0,dragging_mark=0,dragging_ply=0;
	static int which_mark=0;
	int button_event=0;
#ifdef COMPILE_LINUX	
	static double prev_time=0;
	flags&=0x0f;     // I'm guessing. I don't know why CV flags doesn't work on Linux
	if(dragging_map||dragging_mark||dragging_ply)
	{
		if(event!=CV_EVENT_LBUTTONUP)
		{
			// need some time to process data
			// No problem on windows
		       //	printf("eta:%f\n",update_time_map);
			if(((millis()-prev_time) < update_time_map))
			{
				return;
			}
		}
	}
	 
	prev_time=millis();
#endif

	if(x > mapsize.width || y > mapsize.height)	 
	{
		// keep previous values
		
		x=gx;	
		y=gy;
	}
	// Update globals
	gx=x; gy=y;
   switch( event )
    {
		case CV_EVENT_LBUTTONDOWN: 
				button_event=1;
				xdrag_start=x;
				ydrag_start=y;
#ifdef COMPILE_LINUX				
				if(flags==CV_EVENT_FLAG_CTRLKEY )
#else
				if(flags==(CV_EVENT_FLAG_CTRLKEY +CV_EVENT_LBUTTONDOWN))	
#endif				
				{
					if((which_mark=CheckNearPoint(x+roi.x,y+roi.y,POINT_PLY))) // dragg a mark
					{
						if(verbose )
							printf("Dragg PLY mark at %d,%d\n",x+roi.x,y+roi.y);
						dragging_ply=1;
					}
				}
				else if((which_mark=CheckNearPoint(x+roi.x,y+roi.y,POINT_CAL))) // dragg a mark
				{
				  	if(verbose )
					    printf("Dragging Cal mark at %d,%d\n",x+roi.x,y+roi.y);

					dragging_mark=1;
				}
				else
				{
					dragging_map=1;
				}
				break;
		case CV_EVENT_LBUTTONUP: 
				button_event=1;
				dragging_map=0;
				dragging_mark=0;
				dragging_ply=0;
				break;
		case CV_EVENT_RBUTTONUP: 
				button_event=1;
				break;
		case CV_EVENT_RBUTTONDOWN: 
				button_event=1;
				if((which_mark=CheckNearPoint(x+roi.x,y+roi.y,POINT_CAL))) // Right button on mark
				{
					ShowCalPointDialog(which_mark-1);
				}
				else
				{
					ShowMapDialog();
				}
				break;
		case 0: // every time mouse moves
				if(dragging_map)
				{
					if(x!=xdrag_start || y!=ydrag_start)
					{
						// drag de map
						ProcessMapDrag(xdrag_start,ydrag_start,x,y);
						//update coordinates
						xdrag_start=x;
						ydrag_start=y;
					}
				}
				if(dragging_mark)
				{
					if(x!=xdrag_start || y!=ydrag_start)
					{
						// drag de map
						ProcessMarkDrag(xdrag_start,ydrag_start,x,y,which_mark);
						//update coordinates
						xdrag_start=x;
						ydrag_start=y;
					}
				}
				if(dragging_ply)
				{
					if(x!=xdrag_start || y!=ydrag_start)
					{
						// drag de map
						ProcessPlyMarkDrag(xdrag_start,ydrag_start,x,y,which_mark);
						//update coordinates
						xdrag_start=x;
						ydrag_start=y;
					}
				}
				DrawZoomWindow(x,y);
				break;

    }
	if((verbose > 2) || (verbose && button_event))
		printf("Mouse Event:%d flags:%x\n",event,flags);
}
void ProcessMapDrag(int startx, int starty,int endx ,int endy)
{
	if(!startx && !starty && !endx && !endy) // center de image
	{
		if(verbose) printf("center to mouse\n");
		roi.x+=gx-roi.width/2;
		roi.y+=gy-roi.height/2;
	}
	else
	{
		if(verbose > 1) printf("Drag from %d,%d to %d,%d\n",startx,starty,endx,endy);
		roi.x+=startx-endx;
		roi.y+=starty-endy;
	}
	CheckBoundaries(&roi);
	UpdateMapWindow(UPDATE_NO_SHADOW);
}

void ProcessMarkDrag(int startx, int starty,int endx ,int endy,int which)
{
	which--; // the array starts at 0
	startx=MAX(startx,0); // Limit to image W and H
	startx=MIN(startx,mapsize.width); // Limit to image W and H
	endx=MAX(endx,0); // Limit to image W and H
	endx=MIN(endx,mapsize.width); // Limit to image W and H
	starty=MAX(starty,0); // Limit to image W and H
	starty=MIN(starty,mapsize.height); // Limit to image W and H
	endy=MAX(endy,0); // Limit to image W and H
	endy=MIN(endy,mapsize.height); // Limit to image W and H
	
	if(verbose > 1) printf("Drag mark %d,%d to %d,%d\n",startx,starty,endx,endy);
	cal_points[which].x=roi.x+endx;
	cal_points[which].y=roi.y+endy;
	cal_points[which].phi=CalcAngle(cal_points[which].x,cal_points[which].y);
	CheckBoundaries(&roi);
	UpdateMapWindow(UPDATE_ALL);
}
void ProcessPlyMarkDrag(int startx, int starty,int endx ,int endy,int which)
{
	which--; // the array starts at 0
	startx=MAX(startx,0); // Limit to image W and H
	startx=MIN(startx,mapsize.width); // Limit to image W and H
	endx=MAX(endx,0); // Limit to image W and H
	endx=MIN(endx,mapsize.width); // Limit to image W and H
	starty=MAX(starty,0); // Limit to image W and H
	starty=MIN(starty,mapsize.height); // Limit to image W and H
	endy=MAX(endy,0); // Limit to image W and H
	endy=MIN(endy,mapsize.height); // Limit to image W and H
	
	if(verbose > 2) printf("Drag PLY mark %d,%d to %d,%d\n",startx,starty,endx,endy);
	ply_points[which].x=roi.x+endx;
	ply_points[which].y=roi.y+endy;
	ply_points[which].phi=CalcAngle(cal_points[which].x,cal_points[which].y);
	UpdateMapWindow(UPDATE_ALL);
}

void CheckBoundaries(CvRect * roi)
{
	if((roi->x+roi->width) > mapsize.width)	    roi->x=mapsize.width-roi->width;
	if((roi->y+roi->height) > mapsize.height)	roi->y=mapsize.height-roi->height;
	if(roi->x < 0)roi->x=0;
	if(roi->y < 0)roi->y=0;
}

void ProcessMapZoom(int zoom_level)
{
	static int prev_zoom=0;
	//zoom == 0 show the entire image
	if(verbose) printf("Zoom actual:%d\n",zoom_level);
	if(!zoom_level)
	{
		roi.x=roi.y=0;
		roi.width=mapsize.width;
		roi.height=mapsize.height;
	}
	else
	{
		if(zoom_level > prev_zoom) // +
		{
			// Go to mouse position and zoom in
			roi.width=mapsize.width/(MAP_ZOOM_FACTOR*zoom_level);
			roi.height=mapsize.height/(MAP_ZOOM_FACTOR*zoom_level);
			roi.x+=gx-roi.width/2;
			roi.y+=gy-roi.height/2;
		}
		if(zoom_level < prev_zoom) // -
		{
			// Zoom out
			roi.x+=roi.width/2;
			roi.y+=roi.height/2;
			roi.width=mapsize.width/(MAP_ZOOM_FACTOR*zoom_level);
			roi.height=mapsize.height/(MAP_ZOOM_FACTOR*zoom_level);
			roi.x-=roi.width/2;
			roi.y-=roi.height/2;
		}
	}
	CheckBoundaries(&roi);
	calib_mark_dim= roi.width*CALIB_MARK_WIDTH_FACTOR/win_factor;
	if(verbose > 2) printf("x:%d y:%d w:%d h:%d gx:%d gy:%d\n",roi.x,roi.y,roi.width,roi.height,gx,gy);
	prev_zoom=zoom_level;
	UpdateMapWindow(UPDATE_NO_SHADOW);
	
}
void ProcessKey(int c)
{

	int res;
	int kflags=0;
#ifdef COMPILE_LINUX	
	kflags=(c&0xff0000) >>16 ;
	c&=0xffff;
#endif	
	if(verbose )
		printf("Key:%d 0x%x c:%c  - flag:%d\n",c,c,c,kflags);

	switch(c)
	{
		case KEY_PGUP:
		case 0xffab: // OpenCv GTK +
		case '+':     // Zoom in
		
			ProcessMapZoom(++actual_zoom);
			break;
		case KEY_PGDOWN:
		case 0xffad:
		case '-': // Zoom out
			if(actual_zoom) // don't over zoom-out
				ProcessMapZoom(--actual_zoom);
			break;
		case '0': // restore
		case 0xffb0:
			actual_zoom=0;
			ProcessMapZoom(actual_zoom);
			break;
		case KEY_INSERT:
		case 'i':
		case 'I':
			AddMarkAtCursor(POINT_CAL);
			UpdateMapWindow(UPDATE_ALL);
			break;
		case 'b':
		case 'B':
			AddMarkAtCursor(POINT_PLY);
			OrderCalPoints(POINT_PLY);
			UpdateMapWindow(UPDATE_ALL);
			break;
		case 'v':
		case 'V':
			res=UnDrawCalPoint(POINT_PLY);
			if(res!=-1)
				DeleteCalPoint(res,POINT_PLY);
			UpdateMapWindow(UPDATE_ALL);
			break;
		case KEY_DELETE:
		case 'd':
		case 'D':
			res=UnDrawCalPoint(POINT_CAL);
			if(res!=-1)
				DeleteCalPoint(res,POINT_CAL);
			UpdateMapWindow(UPDATE_ALL);
			break;
		case KEY_HOME:
		case 'c':
		case 'C':
			ProcessMapDrag(0,0,0,0);
			break;
		case KEY_CTRL_S:
		case 'S':
		case 's':
			CallSaveFileDialog();
			break;
		case KEY_F1:
			disp_help();
			break;
	}
}

void ParseLatLon(double lat, double lon, char * str)
{
	char str_aux[100];
	int sign = 1;  
	int deg;
	double min;
    if (lat < 0)  
    {  
         sign = -sign;  
         lat = -lat;  
    }  
    deg = floor(lat);  
    lat -= deg;  
    lat *= 60.0;  
    min = lat;  
	sprintf(str,"%2d%c %2.2f",deg,(sign>0)?'N':'S',min);
	sign=1;
    if (lon < 0)  
    {  
         sign = -sign;  
         lon = -lon;  
    }  
    deg = floor(lon);  
    lon -= deg;  
    lon *= 60.0;  
    min = lon;  
	sprintf(str_aux," %2d%c% 2.2f",deg,(sign>0)?'E':'W',min);
	strcat(str,str_aux);
}

void ShowCalPointDialog(int wich)
{
	/* Fill the data for the dialog */
	if(verbose) printf("ShowCalPointDialog()\n");
	data_dialog.dialog_type=DIALOG_CAL_POINT;
	ParseLatLon(cal_points[wich].lat_dec,cal_points[wich].lng_dec,data_dialog.input_str);
	strcpy(data_dialog.title,"Edit coordinates");
	strcpy(data_dialog.comment,"Enter latitud and longitude\n");
	if(CallInputDialog()) // Ok Button
	{
		// check limits
		if((data_dialog.input_lat < -85) || (data_dialog.input_lat > 85)) 
		{
			ShowMsg("Error","Latitude out of limits [-85,85]",T_OK);
			return;
		}
		if((data_dialog.input_lon < -180) || (data_dialog.input_lon > 180))
		{
			ShowMsg("Error","Longitude out of limits [-180,180]",T_OK);
			return;
		}
		if((data_dialog.input_lat ==0) || (data_dialog.input_lon == 0))
		{
			ShowMsg("Error","Error on input",T_OK);
			return;
		}
		cal_points[wich].lat_dec=data_dialog.input_lat;
		cal_points[wich].lng_dec=data_dialog.input_lon;
		if(cal_points[wich].lat_dec && cal_points[wich].lng_dec) 
			cal_points[wich].has_data=1;
		else
			cal_points[wich].has_data=0;
		UpdateMapWindow(UPDATE_ALL);
	}
}

void ShowMapDialog(void)
{
	if(verbose) printf("ShowMapDialog()\n");
	/* Fill the data for the dialog */
	data_dialog.dialog_type=DIALOG_CHART;
	strcpy(data_dialog.title,"Chart info");
	if(CallInputDialog()) // Ok Button
	{
		UpdateMapWindow(UPDATE_ALL);
	}
}

void CreateKapFile(char * out_filename)
{
	FILE * fd;
	int j,ret;
	time_t t;
    struct tm *tmp;
	cal_point_t *points_tmp;
	char header_filename[200];
	double pp=0.0,dx=0.0,dy=0.0;
	char release_date[50];
	int optkap=0; // Cambiar
	int optcolors=1; // Cambiar
	char * optiontitle=NULL; // Cambiar
	if(CountValidCalPoints(POINT_CAL)<2)
	{
		ShowMsg("Error","Introduce at least two calibration points",T_OK);
		return;
	}
	/* Release date */
    t = time(NULL);
    tmp = localtime(&t);
	strftime(release_date, sizeof(release_date),"%m/%d/%Y",tmp);
	sprintf(header_filename,"%s.%s",out_filename,"txt");
	fd=fopen(header_filename,"w");
	if(fd==NULL)
	{
		char msg[250];
		sprintf(msg,"Error openging %s for writing",header_filename);
		ShowMsg("Error",msg,T_OK);
		return;
	}
	/* Now, write de KAP header */
	fprintf(fd,"!BSB Header generated by imgcalkap\n");
	fprintf(fd,"VER/3.0\n");
	fprintf(fd,"CRR/%s\n",data_dialog.chart_desc);
	fprintf(fd,"BSB/NA=%s\n",data_dialog.chart_name);
	fprintf(fd,"    NU=%d,RA=%d,%d,DU=100\n",data_dialog.chart_number,mapsize_orig.width,mapsize_orig.height);
	fprintf(fd,"KNP/SC=%ld,GD=%s,PR=%s\n",data_dialog.scale,strtoupper(data_dialog.datum),strtoupper(data_dialog.proj));
	pp=CalcProjectionParameter(data_dialog.proj);
	fprintf(fd,"    PP=%lf,PI=UNKNOWN,SP=UNKNOWN,SK=0.0,TA=90.0\n",pp);
	fprintf(fd,"    UN=%s,DX=%lf,DY=%lf\n",strtoupper(data_dialog.depth_units),dx,dy);
	fprintf(fd,"CED/SE=0,RE=0,ED=%s\n",release_date);
	fprintf(fd,"OST/1\n");
	/* Now, the points we edited */
	OrderCalPoints(POINT_CAL); // points with data first
	if(CountValidCalPoints(POINT_CAL)==2) // two corners case: Generate two aditional points
	{
		// index 0 an 1 are the points already have
		// copy the two points and generate two additional corners
		memcpy(&cal_points[2],&cal_points[0],sizeof(cal_point_t));
		memcpy(&cal_points[3],&cal_points[1],sizeof(cal_point_t));
		cal_points[2].x=cal_points[1].x;
		cal_points[2].y=cal_points[0].y;

		cal_points[2].lat_dec=cal_points[0].lat_dec;
		cal_points[2].lat=cal_points[0].lat;
		cal_points[2].lat_min=cal_points[0].lat_min;
		cal_points[2].lng_dec=cal_points[1].lng_dec;
		cal_points[2].lng=cal_points[1].lng;
		cal_points[2].lng_min=cal_points[1].lng_min;
		
		cal_points[2].phi=CalcAngle(cal_points[2].x,cal_points[2].y);
		cal_points[2].is_extra=1;
		
		cal_points[3].x=cal_points[0].x;
		cal_points[3].y=cal_points[1].y;
		
		cal_points[3].lat_dec=cal_points[1].lat_dec;
		cal_points[3].lat=cal_points[1].lat;
		cal_points[3].lat_min=cal_points[1].lat_min;
		cal_points[3].lng_dec=cal_points[0].lng_dec;
		cal_points[3].lng=cal_points[0].lng;
		cal_points[3].lng_min=cal_points[0].lng_min;
		cal_points[3].phi=CalcAngle(cal_points[3].x,cal_points[3].y);
		cal_points[3].is_extra=1;
		OrderCalPoints(POINT_CAL); // Order again, for the new angles
	}
	j=0;
	while(cal_points[j].has_data)
	{
		int x,y;
		x=cal_points[j].x*global_scale_factor;
		y=cal_points[j].y*global_scale_factor;
		fprintf(fd,"REF/%d,%d,%d,%lf,%lf\n",j+1,x,y,cal_points[j].lat_dec,cal_points[j].lng_dec);
		j++;	
	}
	/* Whe have the coordinates, so we need to calculate the relative coordinates
	   of boundaries. First will save the points because we will multiply the x,y coordinates
       by the picture resizing scale factor  */
	points_tmp=malloc(sizeof(cal_point_t)*MAX_CAL_POINTS);
	memcpy(points_tmp,ply_points,sizeof(cal_point_t)*MAX_CAL_POINTS);
	PLYCoordXYtoLatLon(global_scale_factor);
	OrderCalPoints(POINT_PLY);
	j=0;
	//while(cal_points[j].has_data)
	while(ply_points[j].has_data)
	{
		fprintf(fd,"PLY/%d,%lf,%lf\n",j+1,ply_points[j].lat_dec,ply_points[j].lng_dec);
		j++;	
	}
	/* Restore the boundary point data */
	memcpy(ply_points,points_tmp,sizeof(cal_point_t)*MAX_CAL_POINTS);
	free(points_tmp);
	fclose(fd);
	ret=imgheadertokap(FreeImage_GetFileType(img_filename,0),  // typein
					img_filename, // filein
					FIF_TXT, //type header
					optkap,  // optkap
					optcolors,  // color
					optiontitle, // tittle
					header_filename, // filehader 
					out_filename); // fileout
	
	if(ret) // something wrong
	{
		char s[300];
		sprintf(s,"Kap file not generated.imgheadertokap() error:%d\n",ret);
		ShowMsg("Error",s,T_OK);
	}
	DeleteExtraCalPoints(POINT_CAL);
	//SaveCalData();
}



void disp_help(void)
{
	ShowMsg("Help",
	"Use: imgcalkap <optional_image_file or kap_file>\n\n"
	"Keys:\n\n"
	"[+],[-] or [PgUp],[PgDown]       Zoom\n"
	"[Insert] or [i]                              Insert a new calibration point at mouse position\n"
	"[Delete] or [d]                             Delete calibration point under mouse position\n"
	"[b]	                                    Insert a boundary point at mouse position\n"
	"[v]	                                    Delete a boundary point under mouse position\n"
	"[home]   or [c]                             Move image to cursor\n"
	"[0]                                               Restore zoom\n"
	"[Ctrl +s] or [s]                             Open save dialog\n"
	"[Esc]                                            Exit program\n\n" 
	"-Zoom in and drag the map with the mouse\n\n"
	"-Right click on calibration mark to enter coordinates\n"
	"-Right click elsewhere to edit map information\n\n"
	"-To drag a boundary point, you must keep pressed the CTRL key and then drag\n"
	"  the blue circles using the left button of the mouse\n\n"
	"-Click on the splash to start\n\n"
	"               This is free software under GPLv3 License.\n"
	"                   See LICENSING.txt for details.\n\n",T_OK);
}

 
/*****************************************************************
 This function is called fropm a callback in mousewheel.c when
 a mouse wheel event is captured.
 Zoom in if direction==1, Zoom out if direction==0 
 ********************************************************/
void ProcessMouseWheel(int direction)
{
		// Zoom process
		if(actual_zoom && (!direction) ) // don't over zoom-out
				ProcessMapZoom(--actual_zoom);
		if(direction)
			ProcessMapZoom(++actual_zoom);
}

/******************************************************************************************
 These functions are a patch because there is no default way in OpenCV to trap the "close" 
 button. I should to move these functions to other files.
 For Windows, I can disable the close button.
 For Linux, I don't know yet, I must to call exit() for now :-(
 *********************************************************************************************/
#ifdef COMPILE_LINUX
gint close_event( GtkWidget *widget,  GdkEvent  *event,  gpointer   data )
{
//	exit(0);
    return(TRUE);
}
#endif
void DisableXButton(char * name)
{
#ifndef COMPILE_LINUX
	// Windows: Disable Close button
	HWND hwnd = (HWND)FindWindow(0,name);
	HMENU hmenu = GetSystemMenu(hwnd, FALSE);
	EnableMenuItem(hmenu, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
#else
	// Linux: I dont' know how to do it. Set up a callback for leaving the program
	GtkWidget * widget;
	widget=(GtkWidget * )cvGetWindowHandle(name); 
	gtk_signal_connect (GTK_OBJECT (widget), "delete_event", GTK_SIGNAL_FUNC (close_event), NULL);
	gtk_signal_connect (GTK_OBJECT (widget), "destroy", GTK_SIGNAL_FUNC (close_event), NULL);
#endif
}


 
 
