#include <stdlib.h>
#include <stdio.h>
#include "readkapdetails.h"
extern char verbose;
/* Return the field given by str */
char * read_field(char * line,char * search)
{
	char * s;
	static char field[100];
	s = strstr(line,search);
	if(s!=NULL)
	{
		char * p=s;
		while ( ((*p) != ',') && ((*p) != 0)) p++;
		memcpy(field,s,p-s);
		field[p-s]=0;
		if(verbose)
			printf("field:%s found in %s\n",field,line);
		return field+strlen(search);
	}
	return NULL;
 }
 
 void readkapdetails(char * line)
 {
 	/* coordinates and boundaries */
	int     x,y,refindex;
	double  lat,lon;   
	char *s;
	if((s = read_field(line,"NA=")))
	{
		strcpy(data_dialog.chart_name,s);
		if(verbose)
			printf("line:%s NA:%s\n",line,data_dialog.chart_name);
	}
	if((s = read_field(line,"CRR/")))
	{
		strcpy(data_dialog.chart_desc,s);
		if(verbose)
			printf("line:%s CRR:%s\n",line,data_dialog.chart_desc);
	}

	if((s = read_field(line,"PR=")))
	{
		strcpy(data_dialog.proj,s);
		if(verbose)
			printf("line:%s PR:%s field:%s\n",line,data_dialog.proj,s);
	}
	if((s = read_field(line,"SC=")))
	{
		data_dialog.scale=atol(s);
		if(verbose)
			printf("line:%s SC:%ld field:%s\n",line,data_dialog.scale,s);
	}
	if((s = read_field(line,"GD=")))
	{
		strcpy(data_dialog.datum,s);
		if(verbose)
			printf("line:%s GD:%s field:%s\n",line,data_dialog.datum,s);
	}
	if((s = read_field(line,"UN=")))
	{
		strcpy(data_dialog.depth_units,s);
		if(verbose)
			printf("line:%s UN:%s field:%s\n",line,data_dialog.depth_units,s);
	}
	if (sscanf(line, "REF/%d,%d,%d,%lf,%lf", &refindex, &x, &y, &lat,&lon) == 5)
	{
		cal_points[refindex-1].x=x;
		cal_points[refindex-1].y=y;
		cal_points[refindex-1].lat_dec=lat;
		cal_points[refindex-1].lng_dec=lon;
		cal_points[refindex-1].lat=(int)lat;
		cal_points[refindex-1].lat_min=60.0*(double)(lat-(int)lat);
		cal_points[refindex-1].lng=(int)lon;
		cal_points[refindex-1].lng_min=60.0*(double)(lon-(int)lon);
		cal_points[refindex-1].has_data=1;
	}
	if (sscanf(line, "PLY/%d,%lf,%lf", &refindex, &lat,&lon) == 3)
	{
		ply_points[refindex-1].lat_dec=lat;
		ply_points[refindex-1].lng_dec=lon;
		ply_points[refindex-1].has_data=1;
	}
}


