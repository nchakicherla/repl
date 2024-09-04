#ifndef VM_H
#define VM_H

#include "common.h"
#include "memory.h"
#include "scanner.h"
#include "ast.h"
#include "chunk.h"

typedef struct s_VM {
	Chunk *chunk;
	MemPool *pool;
} *VM;

int scanTokensFromSource(Chunk *chunk, char *source);

void printTokens(Chunk *chunk);

#endif // VM_H