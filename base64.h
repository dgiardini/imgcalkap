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
#ifndef __BASE_64_IMGCALKAP_
#define __BASE_64_IMGCALKAP_
void encode( FILE *infile, FILE *outfile, int linesize );
// Codifica en memoria: in es puntero a la data a codificar, 
//						out: puntero donde la rutina copairá la nueva data, dimensionar
//  						 al menos un 33% mayor a la cantaida de datos a codificar
// 						in_size: cantidad de datos a codificar
//                      out_size: cantidad de datos devueltos.
void encode_mem(char *indata , char * outdata, int insize, int * outsize, int linesize);
int decode_mem(char *indata , char * outdata, int size);
#endif
