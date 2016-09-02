#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "parse_dms.h"
	
#define DMS_PARSE_MAX_NUMERIC_COMPONENTS 6
#define DMS_PARSE_MAX_DELIMITERS 6
#define DMS_PARSE_CLEANUP() \
{int j; for (j = 0; j < DMS_PARSE_MAX_DELIMITERS; j++) {  \
	if (delimiters[j] != NULL) { \
		free(delimiters[j]); \
	} \
} \
free(input); }

#define COMPARE_DELIM_ADVANCE(ptr) while( ++*ptr == ' ') {}


static int compare_delim(char*, char*);
static inline int is_cardinal(char a);

static inline int is_cardinal(char a) {
	return a == 'N' || a == 'S' || a == 'E' || a == 'W';
}

static int compare_delim(char* a, char* b) {
	/* Shouldn't be null with reasonable input, but check to avoid possibility
	   of dereferncing null pointer */
	if (a == NULL || b == NULL) {
		return 0;
	}

	/* Consider delimiter strings equal if they have the same number of
	   non-space characeters, and all non-space and non-cardinal 
 	   characters are the same. */
	while (*a != '\0' && *b != '\0') {
		if (*a == *b || (is_cardinal(*a) && is_cardinal(*b))) {
			COMPARE_DELIM_ADVANCE(a);
			COMPARE_DELIM_ADVANCE(b);
		} else {
			return 0;
		}
	}
	return *a == *b; /* both equal to '\0' */
}

dms_parser_state parse_dms(const char* orig_input, double* lat, double* lon) {
	typedef enum {NUMERIC, NON_NUMERIC} state;
	char* input = strdup(orig_input);
	char cur = input[0];
	char* num_start = NULL; /* start point of a substring to extract */
	char* delim_start = NULL; /* start point of a delimiter to extract */	

	int mode_strict = 0; /* delimiters must match exactly */

	/* Use these to hold cardinal directions as they are encountered. */
	char first_cardinal = 'A';
	char second_cardinal = 'A';	
	double numeric_components[DMS_PARSE_MAX_NUMERIC_COMPONENTS] = {0};
	char* delimiters[DMS_PARSE_MAX_DELIMITERS] = {NULL};

	double coord1 = 0;
	double coord2 = 0;

	state cur_state = NON_NUMERIC;
	short component_idx = -1;
	short delimiter_idx = -1;
	size_t i;
	
	for (i = 0; cur != '\0'; i++) {
		cur = input[i]; /* assigned after test condition, so loop will process once w/null byte */
				
		if (isdigit(cur) || cur == '.' || (cur == '-' && cur_state == NON_NUMERIC)) {
			if (cur_state == NON_NUMERIC) {
				/* End of a delimiter */
				if (delim_start != NULL) {
					char swap = input[i];
					input[i] = '\0';
					if (delimiter_idx + 1 > DMS_PARSE_MAX_DELIMITERS) {
						DMS_PARSE_CLEANUP();
						return COORDINATES_NOT_SAME_FORMAT;
					}
					delimiters[delimiter_idx] = strdup(delim_start);
					input[i] = swap;
					//printf("delim= \%s\n", delimiters[delimiter_idx]);
					//free(delim);
				}

				/* Start of a number */
				component_idx++;
				if (component_idx + 1 > DMS_PARSE_MAX_NUMERIC_COMPONENTS) {
					DMS_PARSE_CLEANUP();
					return INVALID_NUMBER_NUMERIC_COMPONENTS;
				}				
				cur_state = NUMERIC;
				num_start = &input[i];
			}
		} else {
			if (is_cardinal(cur)) {
				if (first_cardinal == 'A') {
					first_cardinal = cur;
				} else if (second_cardinal == 'A') {
					second_cardinal = cur;
				} else {
					DMS_PARSE_CLEANUP();
					return TOO_MANY_CARDINAL_DIRECTIONS;
				}
			}

			if (cur_state == NUMERIC) {
				/* End of a number */
				char swap = input[i];
				cur_state = NON_NUMERIC;
				delimiter_idx++;
				input[i] = '\0';
				
				/* Attempt to convert substring to double */
				char* endptr;
				numeric_components[component_idx] = strtod(num_start, &endptr);
				if (endptr != NULL && endptr[0] != '\0') {
					//printf("Converting %s to float, got %f with endptr:  >%s<\n", num_start, numeric_components[component_idx], endptr);
					DMS_PARSE_CLEANUP();
					return NUMERIC_PARSE_ERROR;		
				}

				input[i] = swap; /* revert null char that was inserted */
				delim_start = &input[i];
			}
		}		
	}		

	/* Interpret the meaning of the numeric elements according to the total number of
	   numeric elements. */
	switch (component_idx+1) {
		case 6: coord1 = (numeric_components[0] < 0 ? -1 : 1)*(abs(numeric_components[0]) + (numeric_components[1] / 60.0) + (numeric_components[2] / 3600.0));
			coord2 = (numeric_components[3] < 0 ? -1 : 1)*(abs(numeric_components[3]) + (numeric_components[4] / 60.0) + (numeric_components[5] / 3600.0));
			break;
		case 4: coord1 = (numeric_components[0] < 0 ? -1 : 1)*(abs(numeric_components[0]) + (numeric_components[1] / 60.0));
			coord2 = (numeric_components[2] < 0 ? -1 : 1)*(abs(numeric_components[2]) + (numeric_components[3] / 60.0));
			break;
		case 2: coord1 = numeric_components[0];
			coord2 = numeric_components[1];			
			break;
		default: DMS_PARSE_CLEANUP();
			 return INVALID_NUMBER_NUMERIC_COMPONENTS; 
	}
	
	/* Inspect our cardinals.  Do they tell us the order of the components?
	   If not, assume lat-lon.  Use cardinals, if we have them, do set the
	   sign of the coordinates. */
	if (first_cardinal == 'E' || first_cardinal == 'W') {
		/* lon-lat */
		*lon = (coord1 > 0 && first_cardinal  == 'W' ? -1*coord1 : coord1);
		*lat = (coord2 > 0 && second_cardinal == 'S' ? -1*coord2 : coord2);
	} else {
		*lat = (coord1 > 0 && first_cardinal  == 'S' ? -1*coord1 : coord1);
		*lon = (coord2 > 0 && second_cardinal == 'W' ? -1*coord2 : coord2);
	}

	/* If we're in "strict mode", make sure that the coordinates had the same delimiter structure. 
	   This is to prevent cases like this from being output: 32°26′46″ N 122.61458° W */
	if (mode_strict) {
		short delimiters_match = 1;
		switch (component_idx + 1) {
			case 6: /* Compare delimiters between D/M and M/S */
				delimiters_match = (compare_delim(delimiters[0], delimiters[3]) &&
				                    compare_delim(delimiters[1], delimiters[4]));
				break;
			case 4: /* Compare delimiters between D/M only */
				delimiters_match =  compare_delim(delimiters[0], delimiters[2]);
				break;
		}
		if (!delimiters_match) {
			DMS_PARSE_CLEANUP();
			return COORDINATES_NOT_SAME_FORMAT;
		}
	}
	
	DMS_PARSE_CLEANUP();
	return SUCCESS;
}
 
