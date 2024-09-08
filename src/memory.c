#include "common.h"

#include "memory.h"
#include <stdio.h>

#define MEMORY_HOG_FACTOR 8
#define DEF_BLOCK_INIT_SIZE 1024
/*
static inline Block *getLastBlock(MemPool *pool) {
	
	Block *current = pool->first_block;
	Block *next = current->next;

	while(next) {
		current = next;
		next = next->next;
	}
	return current;
}
*/
static Block *newInitBlock(size_t block_size) {
	
	Block *block =  malloc(sizeof(Block));
	if(!block) {
		fprintf(stderr, "block alloc failed! exiting...\n");
		exit(1);
	}

	block->data = malloc(block_size);
	if(!block->data) {
		fprintf(stderr, "block data alloc failed! exiting...\n");
		exit(2);
	}

	block->data_size = block_size;
	block->next = NULL;
	
	return block;
}

int initMemPool(MemPool *pool) {
	size_t block_size = DEF_BLOCK_INIT_SIZE;

	pool->first_block = malloc(sizeof(Block));
	// pool->first_block = calloc(1, sizeof(Block));
	if(!pool->first_block) return 1;
	
	pool->first_block->data = malloc(block_size);
	// pool->first_block->data = calloc(1, block_size);
	if(!pool->first_block->data) return 2;

	pool->first_block->data_size = DEF_BLOCK_INIT_SIZE;
	pool->first_block->next = NULL;

	pool->bytes_used = 0;
	pool->bytes_allocd = sizeof(MemPool) + sizeof(Block) + block_size;
	pool->next_free = pool->first_block->data;
	pool->next_free_size = pool->first_block->data_size;
	pool->last_block_size = block_size;
	pool->last_block = pool->first_block;
	return 0;
}

int termMemPool(MemPool *pool) {

	Block *curr = pool->first_block;
	Block *next = NULL;

	while(curr) {
		next = curr->next;
		free(curr->data);
		free(curr);
		curr = next;
	}
	return 0;
}

int resetMemPool(MemPool *pool) {

	Block *curr = pool->first_block;
	Block *next = NULL;

	while(curr) {
		next = curr->next;
		free(curr->data);
		free(curr);
		curr = next;
	}

	pool->first_block = newInitBlock(pool->last_block_size);
	pool->last_block = pool->first_block;

	pool->next_free = pool->first_block->data;
	pool->next_free_size = pool->first_block->data_size;
	// pool->last_block_size = pool->block->data_size;

	pool->bytes_used = 0;
	pool->bytes_allocd = sizeof(MemPool) + sizeof(Block) + pool->last_block_size;
	return 0;
}

void *palloc(MemPool *pool, size_t size) {
	
	// Block *last_block = getLastBlock(pool);

	if(pool->next_free_size <= size) {
		Block *last_block = pool->last_block;
		Block *new_block = NULL;

		size_t new_block_size = pool->last_block_size;

		while(new_block_size < size * MEMORY_HOG_FACTOR) {
			new_block_size = new_block_size * 2;
		}

		new_block = newInitBlock(new_block_size);
		if(!new_block) {
			return NULL;
		}
		last_block->next = new_block;
		last_block = new_block;

		pool->last_block = new_block;
		pool->last_block_size = new_block_size;
		pool->next_free = (char *)last_block->data + size;
		pool->next_free_size = new_block_size - size;

		pool->bytes_used += size;
		pool->bytes_allocd += sizeof(Block) + new_block_size;

		return last_block->data;
	}

	void *output = pool->next_free;
	pool->next_free = (char *)pool->next_free + size;
	pool->next_free_size  = pool->next_free_size - size;

	pool->bytes_used += size;

	return output;
}

char *pStrCpy(char *str, MemPool *pool) {
	
	char *output = NULL;
	size_t len = strlen(str);

	output = palloc(pool, len + 1);
	// if(!output) return NULL;

	for(size_t i = 0; i < len; i++) {
		output[i] = str[i];
	}
	output[len] = '\0';
	return output;
}

size_t getBytesUsed(MemPool *pool) {
	return pool->bytes_used;
}

size_t getBytesAllocd(MemPool *pool) {
	return pool->bytes_allocd;
}

void printPoolInfo(MemPool *pool) {
	printf("pool info - \n");
	printf("\tbytes used: %zu, (%f MB)\n", getBytesUsed(pool), (double)getBytesUsed(pool) / (1024 * 1024));
	printf("\tbytes allocd: %zu, (%f MB)\n", getBytesAllocd(pool), (double)getBytesAllocd(pool) / (1024 * 1024));
	return;
}

/*
int memSwap(void *ptr1, void *ptr2, size_t size, MemPool *mPool) {
	void *temp = palloc(mPool, size);
	if(!temp) return 1;

	memcpy(temp, ptr1, size);
	memcpy(ptr1, ptr2, size);
	memcpy(ptr2, temp, size);

	return 0;
}
*/
