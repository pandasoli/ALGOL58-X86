#include <hashtable.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static unsigned hash(key, len)
	const void *key;
	unsigned len;
{
	unsigned val = 0;
	unsigned int i;

	for (i = 0; i < len; ++i) {
		val += ((unsigned char *) key)[i];
		val = (val * ((unsigned char *) key)[i]) % TABLE_SIZE;
	}

	return val;
}

hashtable_init(table)
	HashItem **table;
{
	*table = calloc(sizeof(HashItem), TABLE_SIZE);
	if (table == NULL) {
		fprintf(stderr, "calloc(%ld, %d) return NULL on %s\n", sizeof(HashItem), TABLE_SIZE, __func__);
		return 1;
	}

	return 0;
}

hashtable_append(table, obj, key, key_len, cmp, out_obj)
	HashItem *table;
	const void *obj;
	const char *key;
	unsigned key_len;
	int (*cmp)(const void *, const void *);
	const void **out_obj;
{
	unsigned i = hash(key, key_len);

	HashItem *current = &table[i];
	int found = 1;

	if (current->obj)
		while (((found = cmp(current->obj, obj)) != 0) && (current->next != NULL))
			current = current->next;

	if (found) {
		HashItem stack = (HashItem) { .obj = obj, .next = NULL };

		HashItem *heap = malloc(sizeof(HashItem));
		if (heap == NULL) {
			fprintf(stderr, "malloc(%ld) returned NULL on %s\n", sizeof(HashItem), __func__);
			return 1;
		}

		memcpy(heap, &stack, sizeof(HashItem));
		current->next = heap;
		*out_obj = obj;
	}
	else
		*out_obj = current->obj;

	return 0;
}
