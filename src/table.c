#include "table.h"
#include "memory.h"
#include "string.h"

#define DEF_N_BUCKETS 64

#include <stdio.h>

// https://bpa.st/4P7Q 
uint32_t FNV_1a_hash(char *str) {

    uint32_t h = 0x811c9dc5u;

    for (unsigned char *p = (unsigned char *) str; *p != '\0'; p++) {
        h = (h ^ *p) * 0x1000193u;
    }
    h = (h ^ (h >> 16)) * 0x7feb352du;
    h = (h ^ (h >> 15)) * 0x846ca68bu;

    return h ^ (h >> 16);
}

void init_table(Table *table, MemPool *pool) {
	table->count = 0;
	table->n_buckets = 0;
	table->entries = NULL;
    table->pool = pool;
}

void free_table(Table *table) {
	free(table->entries);
	//free(table);
}

typedef struct {
    char *key;
    Entry *entry;
} Sortee;

void _printSortee(Sortee *sortee, size_t n) {
    for(size_t i = 0; i < n; i++) {
        printf("i: %zu, key: %s, entry addr: %p\n", i, sortee[i].key, (void *)sortee[i].entry);
    }
}

Sortee *makeSortee(Entry *head, MemPool *pool, size_t *counter) {

    size_t n = 1;
    Entry *curr = head;
    if(!curr) return NULL;
    
    while(curr->next) {
        n++;
        curr = curr->next;
    }
    *counter = n;

    Sortee *sorting = palloc(pool, n * sizeof(Sortee));

    curr = head;
    for(size_t i = 0; i < n; i++) {
        sorting[i].key = curr->key;
        sorting[i].entry = curr;
        curr = curr->next;
    }
    return sorting;
}

void swapSortees(Sortee *s1, Sortee *s2) {
    Sortee temp = {.key = s1->key, .entry = s1->entry};
    s1->key = s2->key;
    s1->entry = s2->entry;
    s2->key = temp.key;
    s2->entry = temp.entry;
}

bool str_in_order(char *str1, char *str2) {
    char *it1 = str1;
    char *it2 = str2;

    while(*it1 != '\0' && *it2 != '\0') {
        if(*it1 < *it2) {
            return true;
        } else if (*it2 < *it1) {
            return false;
        } else {
            it1++;
            it2++;
        }
    }
    return true;
}

int sort_entries_by_key(Sortee *sorting, size_t n, MemPool *pool) { // quicksort

    if(n <= 1) {
        return 0;
    }
    size_t pivot_idx = SIZE_MAX;
    char *pivot = sorting[n - 1].key;

    for(size_t j = 0; j < n - 1; j++) {

        if(str_in_order(sorting[j].key, pivot)) {
            pivot_idx++;
            swapSortees(&sorting[j], &sorting[pivot_idx]);
        }
    }
    pivot_idx++;
    swapSortees(&sorting[pivot_idx], &sorting[n - 1]);

    sort_entries_by_key(sorting, pivot_idx, pool);
    sort_entries_by_key(&sorting[pivot_idx + 1], n - pivot_idx - 1, pool);

    return 0;
}

int insert_key(Table *table, char *key, void *value, OBJ_TYPE type) {
    if(table->entries == NULL) {
        table->entries = palloc(table->pool, DEF_N_BUCKETS * sizeof(Entry *));
        for(size_t i = 0; i < DEF_N_BUCKETS; i++) {
            table->entries[i] = NULL;
        }
        table->n_buckets = DEF_N_BUCKETS;
    }

    uint32_t hash = FNV_1a_hash(key);
    size_t bucket = hash % table->n_buckets;    

    size_t n_entries_in_bucket = 0;
    Sortee *sorting = NULL;

    if(table->entries[bucket]) {

        sorting = makeSortee(table->entries[bucket], table->pool, &n_entries_in_bucket);
        sort_entries_by_key(sorting, n_entries_in_bucket, table->pool);
        
        for(size_t i = 0; i < n_entries_in_bucket; i++) {
            if(0 == strcmp(key, sorting[i].key)) {
                return 1;
            }
        }
    }

    Entry *curr = table->entries[bucket];
    Entry *prev = NULL;

    if(curr) {
        while(curr && str_in_order(curr->key, key)) {
            prev = curr;
            curr = curr->next;
        }
    }

    Entry *new_entry = palloc(table->pool, sizeof(Entry));
    new_entry->key = newStrCopy(key, table->pool);

    new_entry->obj.type = type;
    switch(type) {
        case STR_TYPE:
            new_entry->obj.val.str = (char *)value;
            break;
        case ITR_TYPE:
            new_entry->obj.val.itr = *(int64_t *)value;
            break;
        case DBL_TYPE:
            new_entry->obj.val.dbl = *(double *)value;
            break;
    }

    new_entry->hash = hash;
    new_entry->next = NULL;

    if(prev) {
        prev->next = new_entry;
        new_entry->next = curr;
    } else {
        new_entry->next = curr;
        table->entries[bucket] = new_entry;
    }
    table->count += 1;
    return 0;
}

int getObject(Table *table, char *key, Object **obj) {
    if(table->count == 0) return 1;

    uint32_t hash = FNV_1a_hash(key);
    size_t bucket = hash % table->n_buckets;

    if(!table->entries[bucket]) return 2;

    Entry *curr = table->entries[bucket];
    while(curr) {
        if(0 == strcmp(curr->key, key)) {
            *obj = &curr->obj;
            return 0;
        }
        curr = curr->next;
    }
    return 3;
}
