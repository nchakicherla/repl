#include "table.h"

#define N_BUCKETS 32

// https://bpa.st/4P7Q 
uint32_t FNV_1a_hash(const char* str) {

    uint32_t h = 0x811c9dc5u;

    for (unsigned char *p = (unsigned char *) str; *p != '\0'; p++) {
        h = (h ^ *p) * 0x1000193u;
    }
    h = (h ^ (h >> 16)) * 0x7feb352du;
    h = (h ^ (h >> 15)) * 0x846ca68bu;

    return h ^ (h >> 16);
}

void init_table(Table *table) {
	table->count = 0;
	table->capacity = 0;
	table->entries = NULL;
}

void free_table(Table *table) {
	free(table->entries);
	free(table);
}
