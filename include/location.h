#ifndef LOCATION_H
#define LOCATION_H

#include <stddef.h>


typedef struct {
	long
		pos,
		len,
		end;
} Location;

/* long pos, long len */
Location location_create();

#endif
