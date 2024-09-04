#ifndef VM_H
#define VM_H

#include "common.h"
#include "memory.h"
#include "scanner.h"
#include "ast.h"
#include "chunk.h"

typedef struct s_VM {
	Chunk chunk;
	Table table;
	MemPool pool;
	char *name;
	char *source;
} VM;

void initVM(VM *vm, char *script);

void termVM(VM *vm);

int scanTokensFromSource(Chunk *chunk, char *source);

void printTokens(VM *vm);

void printSource(VM *vm);

#endif // VM_H