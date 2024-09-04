#ifndef CHUNK_H
#define CHUNK_H

#include "common.h"
#include "ast.h"
#include "scanner.h"
#include "memory.h"
#include "file.h"

typedef struct s_Chunk {
	char *source;

	size_t n_tokens;
	Token *tokens;

	Node *head;

	struct s_MemPool pool;

	// int err;
} Chunk;

// int __growChunkTokens()

int initChunk(Chunk *chunk);

int termChunk(Chunk *chunk);

int scanTokensFromSource(Chunk *chunk, char *source);

void printTokens(Chunk *chunk);

#endif // CHUNK_H