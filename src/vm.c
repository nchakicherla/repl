#include "vm.h"

int scanTokensFromSource(Chunk *chunk, char *source) {

	size_t len = strlen(source);
	chunk->source = palloc(&(chunk->pool), len + 1);
	memcpy(chunk->source, source, len + 1);
	chunk->source[len] = '\0'; 

	initScanner(chunk->source);

	// scanner passes twice to get count

	size_t n_tokens = 0;
	while(true) {
		Token token = scanToken();
		n_tokens++;
		if (token.type == TK_EOF) break;
	}

	chunk->n_tokens = n_tokens;
	chunk->tokens = palloc(&(chunk->pool), (n_tokens * sizeof(Token)));

	initScanner(chunk->source);

	for(size_t i = 0; i < n_tokens; i++) {
		chunk->tokens[i] = scanToken();
		if(chunk->tokens[n_tokens].type == TK_EOF) break;
	}
	return 0;
}

void printTokens(Chunk *chunk) {

	for(size_t i = 0; i < chunk->n_tokens; i++) {
		printf("TK%6zu: TYPE:%3d - \"%.*s\"\n", i, chunk->tokens[i].type, (int)chunk->tokens[i].len, chunk->tokens[i].start);
	}
	return;
}
