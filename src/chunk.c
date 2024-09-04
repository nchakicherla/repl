#include "common.h"
#include "chunk.h"

#include <string.h>

void initChunk(Chunk *chunk) {
	chunk->source = NULL;

	chunk->n_tokens = 0;
	chunk->tokens = NULL;

	chunk->head = NULL;
	initMemPool(&(chunk->pool));
	return;
}

void resetChunk(Chunk *chunk) {
	chunk->source = NULL;

	chunk->n_tokens = 0;
	chunk->tokens = NULL;
	
	chunk->head = NULL;
	resetMemPool(&(chunk->pool));
}

void termChunk(Chunk *chunk) {
	termMemPool(&(chunk->pool));
	return;
}
