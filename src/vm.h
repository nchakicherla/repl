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

#endif // VM_H