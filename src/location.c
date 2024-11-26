#include <location.h>


Location location_create(pos, len)
	long pos, len;
{
	return (Location) {
		.pos = pos,
		.len = len,
		.end = pos + len
	};
}
