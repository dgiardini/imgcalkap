/* ***********************************************************************************************
*   parse_dms.h
*
*   Copyright 2014 Daniel Baston <dbaston@gmail.com>
*
*   This file provides a function, parse_dms, that takes a string containing a 
*   pair of geographic coordinates, extracts the values of latitude and longitude
*   in decimal degrees, and saves them to supplied pointers.  It returns 0 for
*   success, or an informative error code.
*
*   parse_dms requires the following of its input string:
*	
*   1) Latitude and Longitude have the same formatting (they're both DMS or DD.DDDD, but not mixed)
*      In "strict" mode, parse_dms will attempt to verify this by checking to see if the delimiters
*      between degrees/minutes and degrees/seconds are the same for each coordinate.
*   2) Either cardinal directions are provided, or latitude can be assumed to come before longitude.
*   3) If N, S, E, and W appear in the text, they can be assumed to represent cardinal directions.
*   4) Some kind of delimiter is used between degrees, minutes, and seconds.
*      (you don't have 431720.33 for 43°17'20.33)
*
** ************************************************************************************************/

#ifndef parse_dms_h
#define parse_dms_h
#ifdef __cplusplus
extern "C"
{
#endif
typedef enum {SUCCESS,
	      INVALID_NUMBER_NUMERIC_COMPONENTS,
	      NUMERIC_PARSE_ERROR,
          TOO_MANY_CARDINAL_DIRECTIONS,
	      COORDINATES_NOT_SAME_FORMAT
		  } dms_parser_state;

dms_parser_state parse_dms(const char*, double*, double*);

#ifdef __cplusplus
}
#endif
#endif
