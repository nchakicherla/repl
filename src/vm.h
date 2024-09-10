#ifndef VM_H
#define VM_H

#include "common.h"
#include "memory.h"
//#include "scanner.h"
#include "ast.h"

typedef struct s_VM {
	Chunk chunk;
	GrammarRuleArray ruleArray;
	Table table;
	MemPool pool;
	char *name;
	char *source;
} VM;

void initVM(VM *vm, char *script, char *grammar);

void termVM(VM *vm);

void printTokens(VM *vm);

void printSource(VM *vm);

#endif // VM_H
