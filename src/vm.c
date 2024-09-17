#include "vm.h"
#include "file.h"
#include "ast.h"
//#include "chunk.h"

void initVM(VM *vm, char *grammarFile) {
	initChunk(&(vm->chunk));
	initMemPool(&(vm->pool));
	initGrammarRuleArray(&(vm->ruleArray), grammarFile, &(vm->pool));
	initTable(&(vm->table));
	//vm->name = pNewStr(script, &(vm->pool));
	//vm->source = readFile(script, &(vm->pool));
	vm->grammar_name = pNewStr(grammarFile, &(vm->pool));
	return;
}

void termVM(VM *vm) {
	termChunk(&(vm->chunk));
	termTable(&(vm->table));
	termMemPool(&(vm->pool));
	return;
}

void printTokens(VM *vm) {

	for(size_t i = 0; i < vm->chunk.n_tokens; i++) {
		printf("LINE: %6zu TK%6zu: TYPE: %16s - \"%.*s\"\n", 
			vm->chunk.tokens[i].line,
			i,
			tokenTypeLiteralLookup(vm->chunk.tokens[i].type), 
			(int)vm->chunk.tokens[i].len, vm->chunk.tokens[i].start);
	}
	return;
}
/*
void printSource(VM *vm) {
	printf("SOURCE:\n\"\n%s\n\"\n", vm->source);
	return;
}
*/
