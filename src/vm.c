#include "vm.h"
#include "file.h"
#include "ast.h"
#include "parser.h"

void initVM(VM *vm, char *grammarFile) {
	initParser(&(vm->parser));
	initMemPool(&(vm->pool));
	initGrammarRuleArray(&(vm->rule_array), grammarFile, &(vm->pool));
	initTable(&(vm->table));
	//vm->name = pNewStr(script, &(vm->pool));
	//vm->source = pReadFile(script, &(vm->pool));
	vm->grammar_name = pNewStr(grammarFile, &(vm->pool));
	return;
}

void termVM(VM *vm) {
	termParser(&(vm->parser));
	termTable(&(vm->table));
	termMemPool(&(vm->pool));
	return;
}

void printTokens(VM *vm) {

	for(size_t i = 0; i < vm->parser.n_tokens; i++) {
		printf("LINE: %6zu TK%6zu: TYPE: %16s - \"%.*s\"\n", 
			vm->parser.tokens[i].line,
			i,
			tokenTypeLiteralLookup(vm->parser.tokens[i].type), 
			(int)vm->parser.tokens[i].len, vm->parser.tokens[i].start);
	}
	return;
}
/*
void printSource(VM *vm) {
	printf("SOURCE:\n\"\n%s\n\"\n", vm->source);
	return;
}
*/
