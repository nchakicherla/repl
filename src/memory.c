#include "common.h"

#include "memory.h"
#include <stdio.h>

#define MEMORY_HOG_FACTOR 0.5

static Block *getLastBlock(MemPool *pool) {
	
	Block *current = pool->block;
	Block *next = current->next;

	while(next) {
		current = next;
		next = next->next;
	}
	return current;
}

static Block *newInitBlock(size_t block_size) {
	
	Block *block =  malloc(sizeof(Block));
	block->data = malloc(block_size);
	block->data_size = block_size;
	block->next = NULL;
	
	return block;
}

int initMemPool(MemPool *pool, size_t block_size) {
	
	pool->block = calloc(1, sizeof(Block));
	if(!pool->block) return 1;
	
	pool->block->data = calloc(block_size, 1);
	// pool->block->data = malloc(block_size);
	if(!pool->block->data) return 2;

	pool->block->data_size = block_size;
	// pool->block->next = NULL;

	pool->next_free = pool->block->data;
	pool->next_free_size = pool->block->data_size;
	pool->last_block_size = block_size;
	return 0;
}

int freeMemPool(MemPool *pool) {

	Block *current = pool->block;
	Block *next = NULL;

	while(current) {
		next = current->next;
		free(current->data);
		free(current);
		current = next;	 
	}

	// free(pool);
	return 0;
}

void *palloc(MemPool *pool, size_t size) {
	
	Block *last_block = getLastBlock(pool);
	Block *new_block = NULL;

	if(pool->next_free_size <= size) {
		size_t new_block_size = pool->last_block_size;

		while(new_block_size < size * MEMORY_HOG_FACTOR * 8) {
			new_block_size = new_block_size * 2;
		}

		new_block = newInitBlock(new_block_size);
		if(!new_block) {
			return NULL;
		}
		last_block->next = new_block;
		last_block = new_block;

		pool->last_block_size = new_block_size;
		pool->next_free = (char *)last_block->data + size;
		pool->next_free_size = new_block_size - size;

		return last_block->data;
	}
	void *output = pool->next_free;
	pool->next_free = (char *)pool->next_free + size;
	pool->next_free_size  = pool->next_free_size - size;

	return output;
}

char *newStrCopy(char *str, MemPool *pool) {
	
	char *output = NULL;
	size_t len = strlen(str);

	output = palloc(pool, len + 1);
	if(!output) return NULL;

	for(size_t i = 0; i < len; i++) {
		output[i] = str[i];
	}
	output[len] = '\0';
	return output;
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