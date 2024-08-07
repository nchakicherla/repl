#ifndef TABLE_H
#define TABLE_H

#include "common.h"

#include "memory.h"

#include <stdlib.h>

struct s_Entry;

typedef enum {
	STR_TYPE,
	ITR_TYPE,
	DBL_TYPE,
} OBJ_TYPE;

union Value {
	char *str;
	int64_t itr;
	double dbl;
};

typedef struct {
	OBJ_TYPE type;
	union Value val;
} Object;

typedef struct s_Entry {
	char *key;
	Object obj;
	uint32_t hash;
	struct s_Entry *next;
} Entry;

typedef struct {
	size_t count;
	size_t n_buckets;
	Entry **entries;
	MemPool *pool;
} Table;

uint32_t FNV_1a_hash(char* str);

void init_table(Table *table, MemPool *pool);

void free_table(Table *table);

int insert_key(Table *table, char *key, void *value, OBJ_TYPE type);

int getObject(Table *table, char *key, Object **obj);

#endif // TABLE_H
