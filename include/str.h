#ifndef STR_H
#define STR_H

#include <stddef.h>


typedef struct StrView StrView;
struct StrView {
	char *data;
	long size;
};

/* char *data, int len */
StrView strview_create();

typedef struct StrBuilder StrBuilder;
struct StrBuilder {
	char *data;
	long size;
	long cap;

	/* StrBuilder *self, char ch */
	int (*append)();
	/* StrBuilder *self, const char *cstr, long len */
	int (*append_cstr)();
	/* StrBuilder *self, StrView *view */
	int (*extract_view)();
	/* StrBuilder *self */
	void (*free)();
};

/* const char *data, int size, StrBuilder *sb */
int strbuilder_create();

#endif
