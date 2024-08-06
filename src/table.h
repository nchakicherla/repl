#ifndef TABLE_H
#define TABLE_H

#include "common.h"
#include <stdlib.h>

typedef struct {
	char *key;
	uint64_t value;
	uint32_t hash;
} Entry;

typedef struct {
	size_t count;
	size_t capacity;
	Entry *entries;
} Table;

uint32_t FNV_1a_hash(const char* str);

void init_table(Table *table);

void free_table(Table *table);

#endif // TABLE_H
