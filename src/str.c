#include <str.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


static append_cstr(self, cstr, len)
	StrBuilder *self;
	const char *cstr;
	long len;
{
	assert(self != NULL);
	assert(cstr != NULL);

	if (self->size + len + 1 > self->cap) {
		if (self->cap == 0) self->cap++;

		while (self->size + len + 1 > self->cap)
			self->cap *= 2;

		char *old_data = self->data;
		self->data = realloc(self->data, self->cap);

		if (self->data == NULL) {
			fprintf(stderr, "realloc(%p, %ld) returned NULL on %s\n", old_data, self->cap, __func__);
			return 1;
		}
	}

	strncpy(self->data + self->size, cstr, len);
	self->data[self->size + len] = '\0';
	self->size += len;

	return 0;
}

static append(self, ch)
	StrBuilder *self;
	char ch;
{
	assert(self != NULL);
	return append_cstr(self, &ch, 1);
}

static extract_view(self, view)
	StrBuilder *self;
	StrView *view;
{
	assert(self != NULL);
	assert(view != NULL);

	char *data = malloc(self->size);
	if (data == NULL) {
		fprintf(stderr, "malloc(%ld) returned NULL on %s for data\n", self->size, __func__);
		return 1;
	}

	strncpy(data, self->data, self->size);

	*view = (StrView) {
		.data = data,
		.size = self->size
	};

	return 0;
}

static void free_(self)
	StrBuilder *self;
{
	assert(self != NULL);

	if (!self->cap) return;

	free(self->data);
	self->data = NULL;
	self->size = 0;
	self->cap = 0;
}

strbuilder_create(data, len, sb)
	const char *data;
	long len;
	StrBuilder *sb;
{
	assert(sb != NULL);

	*sb = (StrBuilder) {
		.data = NULL,
		.size = 0,
		.cap = 0,
		.append = &append,
		.append_cstr = &append_cstr,
		.extract_view = &extract_view,
		.free = &free_
	};

	return data != NULL ? append_cstr(sb, data, len) : 0;
}
