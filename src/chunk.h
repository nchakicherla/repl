#ifndef CHUNK_H
#define CHUNK_H

#include "memory.h"
#include "syntax_types.h"
#include "syntax_node.h"

typedef struct s_Chunk {
	char *source;

	size_t n_tokens;
	Token *tokens;

	SyntaxNode *head;

	MemPool pool;
	// int err;
} Chunk;

void initChunk(Chunk *chunk);

void resetChunk(Chunk *chunk);

void termChunk(Chunk *chunk);
#endif // CHUNK_H