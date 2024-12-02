#ifndef HASH_TABLE
#define HASH_TABLE

#define TABLE_SIZE 10


typedef struct HashItem HashItem;
struct HashItem {
	const void *obj;
	HashItem *next;
};

typedef HashItem *HashTable;

/* HashItem **table */
int hashtable_init();
/* HashItem *table, const void *obj, const char *key, unsigned key_len, int (*cmp)(const void *, const void *) */
int hashtable_append();

#endif
