#include "common.h"
#include "chunk.h"

#include <string.h>

int initChunk(Chunk *chunk) {
	chunk->source = NULL;

	chunk->n_tokens = 0;
	chunk->tokens = NULL;

	chunk->head = NULL;

	initMemPool(&(chunk->pool));
	// return initMemPool(&(chunk->pool));
	return 0;
}

int termChunk(Chunk *chunk) {
	termMemPool(&(chunk->pool));
	return 0;
}

int scanTokensFromSource(Chunk *chunk, char *source) {

	size_t len = strlen(source);
	chunk->source = palloc(&(chunk->pool), len);
	memcpy(chunk->source, source, len);
	chunk->source[len] = '\0';

	initScanner(chunk->source);

	// scanner passes twice to get count

	size_t counter = 0;
	while(true) {
		Token token = scanToken();
		counter++;
		if (token.type == TK_EOF) break;
	}

	chunk->n_tokens = counter;
	chunk->tokens = palloc(&(chunk->pool), (counter * sizeof(Token)));

	initScanner(chunk->source);

	counter = 0;
	while(true) {
		chunk->tokens[counter] = scanToken();
		if(chunk->tokens[counter].type == TK_EOF) break;
		counter++;
	}

	return 0;
}

void printTokens(Chunk *chunk) {

	for(size_t i = 0; i < chunk->n_tokens; i++) {
		printf("token %4zu: %d\n", i, chunk->tokens[i].type);
	}
	return;
}
