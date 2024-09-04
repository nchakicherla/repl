#include "vm.h"
#include "file.h"

void initVM(VM *vm, char *script) {
	initChunk(&(vm->chunk));
	initTable(&(vm->table));
	initMemPool(&(vm->pool));
	vm->name = pStrCpy(script, &(vm->pool));
	vm->source = readFile(script, &(vm->pool));
	return;
}

void termVM(VM *vm) {
	termChunk(&(vm->chunk));
	termTable(&(vm->table));
	termMemPool(&(vm->pool));
	return;
}

int scanTokensFromSource(Chunk *chunk, char *source) {

	size_t len = strlen(source);
	chunk->source = palloc(&(chunk->pool), len + 1);
	memcpy(chunk->source, source, len + 1);
	chunk->source[len] = '\0';

	initScanner(chunk->source);

	// scanner passes twice to get count then write to chunk

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
	}
	return 0;
}

void printTokens(VM *vm) {

	for(size_t i = 0; i < vm->chunk.n_tokens; i++) {
		printf("TK%6zu: TYPE: %16s - \"%.*s\"\n", 
			i,
			getTKTypeLiteral(vm->chunk.tokens[i].type), 
			(int)vm->chunk.tokens[i].len, vm->chunk.tokens[i].start);
	}
	return;
}

void printSource(VM *vm) {
	printf("SOURCE:\n\"\n%s\n\"\n", vm->source);
	return;
}