#ifndef ERR_H
#define ERR_H

#define ERR(c)              \
	if ((err = c)) return err

extern int err;

#endif
