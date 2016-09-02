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
#include <string.h>
#include "imgcalkap.h"
#include "proc.h"
#include "points.h"

cal_point_t cal_points[MAX_CAL_POINTS];
cal_point_t ply_points[MAX_CAL_POINTS];
CvPoint *PlyMarks=NULL;
int nPly=0;

void CleanCalPoint(int j, int type)
{
	cal_point_t * p;
	if(type==POINT_CAL)
		p=&cal_points[j];
	else
		p=&ply_points[j];
	
	memset(p,0,sizeof(cal_point_t));
	p->x=-1;
	p->prev_mark_roi.x=-1;
	
}
int CountValidCalPoints(int type)
{
	int j, ret=0;
	cal_point_t * p;
	if(type==POINT_CAL)
		p=cal_points;
	else
		p=ply_points;
	
	for(j=0;j<MAX_CAL_POINTS;j++)
		if(p[j].has_data)
			ret++;
	return ret;
}
/*
int cmpfunc (cal_point_t * q, cal_point_t * p)
{
   if(p->has_data &&  q->has_data)
		return sqrt((p->x - q->x) * (p->x - q->x) + (p->y - q->y) * (p->y - q->y));
	return 	 9999999;
	
}
*/
void OrderCalPoints(int type)
{
	int j,k;
	cal_point_t tmp;
	cal_point_t * p;
	if(type==POINT_CAL)
		p=cal_points;
	else
		p=ply_points;
		
	for(j=0;j<MAX_CAL_POINTS;j++)
		if(p[j].has_data)
			p[j].phi=CalcAngle(p[j].x,p[j].y);
	// has data first
	for(k=0;k<MAX_CAL_POINTS;k++)
	{
		for(j=0;j<MAX_CAL_POINTS-1;j++)
		{
			if(p[j+1].has_data > p[j].has_data) 
			{
				memcpy(&tmp,&p[j+1],sizeof(cal_point_t));
				memcpy(&p[j+1],&p[j],sizeof(cal_point_t));
				memcpy(&p[j],&tmp,sizeof(cal_point_t));
			}
		}
	}
	// order by angle
//	if(type=POINT_CAL)
//	{
		for(k=0;k<MAX_CAL_POINTS;k++)
		{
			for(j=0;j<MAX_CAL_POINTS-1;j++)
			{
				if(p[j+1].has_data && (p[j+1].phi > p[j].phi))
				{
					memcpy(&tmp,&p[j+1],sizeof(cal_point_t));
					memcpy(&p[j+1],&p[j],sizeof(cal_point_t));
					memcpy(&p[j],&tmp,sizeof(cal_point_t));
				}
			}
		}
//	}
//	else
//		qsort(p,CountValidCalPoints(POINT_PLY),sizeof(cal_point_t),cmpfunc);
	if(verbose)
	{
		for(j=0;j<MAX_CAL_POINTS;j++)
			if(p[j].has_data)
				printf("Point %d lat:%f lng:%f phi:%f\n",j,p[j].lat_dec,p[j].lng_dec,p[j].phi);
	}
}

int CheckNearPoint(int x,int y, int type)
{
	int j;
	double dist,min_dist=0;
	cal_point_t *p;
	if(type==POINT_CAL)
	{
		p=cal_points;
		min_dist=calib_mark_dim/2;
	}
	else
	{
		p=ply_points;
		for(j=0;j<MAX_CAL_POINTS;j++)
			if(p[j].x!=-1)
			{
				min_dist=p[j].prev_mark_roi.width/2;
				continue;
			}
	}
	for(j=0;j<MAX_CAL_POINTS;j++)
	{
		if(p[j].x==-1)
			continue;
		 dist= sqrt((x - p[j].x) * (x - p[j].x) + (y - p[j].y) * (y - p[j].y) );
		 if(verbose > 2) printf("distance:%f min:dist:%f\n",dist,min_dist);
		 if(dist < min_dist) // this point is close enough
			 return j+1;
	}
	return 0;
}

void InsertPoint(int x,int y,int type)
{
	int j;
	cal_point_t * p;
	char has_data;
	if(type==POINT_CAL)
	{
		p=cal_points;
		has_data=0;
	}
	else
	{
		p=ply_points;
		has_data=1;
	}
	
	for(j=0;j<MAX_CAL_POINTS;j++)
	{
		if(p[j].x==-1) // free slot 
		{
			p[j].x=x;
			p[j].y=y;
			p[j].phi=CalcAngle(cal_points[j].x,cal_points[j].y);
			p[j].has_data=has_data;
			return;
		}	
	}
}

void AddMarkAtCursor(int type)
{
	if(verbose)	
		printf("Check to add point at %d,%d type %s\n",gx+roi.x,gy+roi.y,type==POINT_CAL?("Cal Point"):("PLY Boundary Point"));
	if(!CheckNearPoint(gx+roi.x,gy+roi.y,type))
	{
		if(verbose)
			printf("No points around, adding at %d,%d\n",gx+roi.x,gy+roi.y);
		InsertPoint(gx+roi.x,gy+roi.y,type);
	}
	else
	{
		if(verbose) printf("Too close from other point of same type\n");
	}

}
void DeleteCalPoint(int which_one, int type)
{
	cal_point_t *p;
	if(type==POINT_CAL)
		p=cal_points;
	else
		p=ply_points;
	memcpy(&p[which_one],&p[which_one+1],sizeof(cal_point_t)*(MAX_CAL_POINTS-which_one-1));
	CleanCalPoint(MAX_CAL_POINTS-1,type);
}

int UnDrawCalPoint(int type)
{
	int which_one;
	cal_point_t * p;
	if(type==POINT_CAL)
		p=cal_points;
	else
		p=ply_points;
	if(verbose) printf("Delete cal point at %d,%d\n",gx+roi.x,gy+roi.y);
	which_one=CheckNearPoint(gx+roi.x,gy+roi.y,type);
	if(which_one)
	{
		which_one--; // The array is zero biased
		if(verbose) printf("Cal Point %d,%d un-drawed an deleted\n",p[which_one].x,p[which_one].y);
		DrawCalibMark(p[which_one].x,p[which_one].y,p[which_one].has_data,&p[which_one].prev_mark_roi,1);
		return which_one;
	}else
	{
		if(verbose) printf("No near point to delete\n");
	}
	return -1;
}

void DeleteExtraCalPoints(int type)
{
	int j;
	if(type==POINT_CAL)
	{
		for(j=0;j<MAX_CAL_POINTS;j++) // Brute force..
			for(j=0;j<MAX_CAL_POINTS;j++)
				if(cal_points[j].is_extra)
					DeleteCalPoint(j,type);
	}
}
double CalcAngle(int x, int y)
{
	return atan2((double)((mapsize.height/2)-y),(double)(x-(mapsize.width/2)));
}
/* from x,y to lat,lon */
void PLYCoordXYtoLatLon(double factor)
{
	/* Search for two representative coordinates */
	int j;
	double min_dist=9999999;
	double max_dist=-1;
	int min_idx=0,max_idx=0; // Index of selected points
	for(j=0;j<MAX_CAL_POINTS;j++)
	{
		if(!cal_points[j].has_data)
			continue;
		double dist_to_zero= sqrt(( cal_points[j].x) * (cal_points[j].x) + (cal_points[j].y) * (cal_points[j].y) );
		if(dist_to_zero > max_dist)
		{
			max_dist=dist_to_zero;
			max_idx=j;
		}
		if(dist_to_zero < min_dist)
		{
			min_dist=dist_to_zero;
			min_idx=j;
		}
	}
	if(verbose)	
		printf("Points %d,%d   %d,%d selected for interpolating coordinates\n",cal_points[min_idx].x,cal_points[min_idx].y,cal_points[max_idx].x,cal_points[max_idx].y);
		
	for(j=0;j<MAX_CAL_POINTS;j++)
	{
		double m,h;
		if(ply_points[j].x==-1)
			continue;
		/* Linear interpolation */
		m = (cal_points[max_idx].lat_dec - cal_points[min_idx].lat_dec) / (cal_points[max_idx].y*factor - cal_points[min_idx].y*factor );
		h = cal_points[min_idx].lat_dec - m * cal_points[min_idx].y*factor;
		ply_points[j].lat_dec=m * ply_points[j].y*factor + h;
		m = (cal_points[max_idx].lng_dec - cal_points[min_idx].lng_dec) / (cal_points[max_idx].x*factor - cal_points[min_idx].x*factor);
		h = cal_points[min_idx].lng_dec - m* cal_points[min_idx].x*factor;
		ply_points[j].lng_dec=m * ply_points[j].x*factor + h;
			
		
		if(verbose)
			printf("Calc Coordinates for x,y PLY %d: %d,%d  %f,%f\n",j,(int)(ply_points[j].x*factor),(int)(ply_points[j].y*factor),ply_points[j].lat_dec,ply_points[j].lng_dec);
	}
}
/* from lat,lon to x,y */
void PLYCoordLatLonToXY(int w, int h)
{
	/* Search for two representative coordinates */
	int j;
	double min_dist=9999999;
	double max_dist=-9999999;
	int min_idx=0,max_idx=0; // Index of selected points
	for(j=0;j<MAX_CAL_POINTS;j++)
	{
		if(!cal_points[j].has_data)
			continue;
		double dist_to_zero= sqrt((cal_points[j].lat_dec) * (cal_points[j].lat_dec) + (cal_points[j].lng_dec) * (cal_points[j].lng_dec) );
		if(dist_to_zero > max_dist)
		{
			max_dist=dist_to_zero;
			max_idx=j;
		}
		if(dist_to_zero < min_dist)
		{
			min_dist=dist_to_zero;
			min_idx=j;
		}
	}
	if(verbose)	
		printf("Coordinates %f,%f   %f,%f selected for interpolating points\n",cal_points[min_idx].lat_dec,cal_points[min_idx].lng_dec,cal_points[max_idx].lat_dec,cal_points[max_idx].lng_dec);
	for(j=0;j<MAX_CAL_POINTS;j++)
	{
		double m,h;
		if(!ply_points[j].has_data)
			continue;
		/* Linear interpolation */
		m = (cal_points[max_idx].y - cal_points[min_idx].y) / (cal_points[max_idx].lat_dec - cal_points[min_idx].lat_dec);
		h = cal_points[min_idx].y - m * cal_points[min_idx].lat_dec;
		ply_points[j].y=m * ply_points[j].lat_dec + h;
		
		m = (cal_points[max_idx].x - cal_points[min_idx].x) / (cal_points[max_idx].lng_dec - cal_points[min_idx].lng_dec);
		h = cal_points[min_idx].x - m* cal_points[min_idx].lng_dec;
		ply_points[j].x=m * ply_points[j].lng_dec + h;
		ply_points[j].x/=global_scale_factor;
		ply_points[j].y/=global_scale_factor;
		
		if(	ply_points[j].x < 0)ply_points[j].x=0;
		if(	ply_points[j].x > w)ply_points[j].x=w;
		if(	ply_points[j].y < 0)ply_points[j].y=0;
		if(	ply_points[j].y > h)ply_points[j].y=h;
		if(verbose)
			printf("Calc pos x,y PLY %d:%d,%d for  %f,%f\n",j,ply_points[j].x,ply_points[j].y,ply_points[j].lat_dec,ply_points[j].lng_dec);
	}
}

void SaveCalData(void)
{
	FILE * fd;
	char chart_data_filename[220];
	sprintf(chart_data_filename,"%s.%s",img_filename,"dat");
	fd=fopen(chart_data_filename,"w");	
	if(fd==NULL)
	{
		printf("Can't open calib file for writing\n");
		return;
	}
	fwrite(&cal_points,sizeof(cal_points),1,fd);
	fwrite(&data_dialog,sizeof(data_dialog),1,fd);
	fclose(fd);
}
void RetrieveCalData(void)
{
	FILE * fd;
	char chart_data_filename[220];
	sprintf(chart_data_filename,"%s.%s",img_filename,"dat");
	fd=fopen(chart_data_filename,"r");	
	if(fd==NULL)
	{
		printf("No previous calib file \n");
		return;
	}
	fread(&cal_points,sizeof(cal_points),1,fd);
	fread(&data_dialog,sizeof(data_dialog),1,fd);
	fclose(fd);
	OrderCalPoints(POINT_CAL); 
}

char * strtoupper(char *str) 
{
	char *p=str;
	while (*str) 
	{
		if ((*str >= 'a') && (*str <= 'z')) 
			*str -= ('a'-'A');
		str++;
	}
	return p;
} 


	