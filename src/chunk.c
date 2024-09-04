#include "common.h"
#include "chunk.h"

#include <string.h>

int initChunk(Chunk *chunk) {
	chunk->source = NULL;

	chunk->n_tokens = 0;
	chunk->tokens = NULL;

	chunk->head = NULL;

	initMemPool(&(chunk->pool));
	return 0;
}

int termChunk(Chunk *chunk) {
	termMemPool(&(chunk->pool));
	return 0;
}
