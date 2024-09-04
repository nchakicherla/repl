#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <string.h>

struct s_Block;

typedef struct s_MemPool {
	size_t bytes_used;
	size_t bytes_allocd;

	void *next_free;
	size_t next_free_size;
	size_t last_block_size;
	struct s_Block *last_block;
	struct s_Block *first_block;
} MemPool;

typedef struct s_Block {
	void *data;
	size_t data_size;
	struct s_Block *next;
} Block;

int initMemPool(MemPool *pool);

int termMemPool(MemPool *pool);

void *palloc(MemPool *pool, size_t size);

char *pStrCpy(char *str, MemPool *pool);

size_t getBytesUsed(MemPool *pool);

size_t getBytesAllocd(MemPool *pool);

// int memSwap(void *ptr1, void *ptr2, size_t size);

#endif // MEMORY_H