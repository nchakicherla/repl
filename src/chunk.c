#include "common.h"
#include "chunk.h"

#include <string.h>

void initChunk(Chunk *chunk) {
	chunk->source = NULL;

	chunk->n_tokens = 0;
	chunk->tokens = NULL;

	initMemPool(&(chunk->pool));
	return;
}

void termChunk(Chunk *chunk) {
	termMemPool(&(chunk->pool));
	return;
}
