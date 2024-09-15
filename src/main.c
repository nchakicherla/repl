#include <stdio.h>

#include "common.h"

//#include "scanner.h"
#include "file.h"
#include "table.h"
#include "memory.h"
#include "vm.h"
#include "ast.h"

#include "random.h"

int main(void) {

	//char *script1 = "./resources/test2.tl";
	char *script = "./resources/testscript.tl";
	char *grammar = "./resources/grammar.txt";

	VM vm;
	initVM(&vm, script, grammar); // grammar build happens here

	scanTokensFromSource(&(vm.chunk), vm.source);
	printSource(&vm);

	vm.chunk.head = palloc(&(vm.pool), sizeof(SyntaxNode));
	//printf("ret: %zu\n", matchGrammar(vm.ruleArray.rules[STX_INIT].head, vm.chunk.tokens, vm.chunk.head, &(vm.pool)));
	//printTokens(&vm);
	//printSyntaxNode(vm.chunk.head, 0);

	TokenStream stream;
	stream.tk = vm.chunk.tokens;
	stream.pos = 0;
	stream.n = vm.chunk.n_tokens;

	printTokenStream(&stream);

	SyntaxNode *node = parseGrammar(vm.ruleArray.rules[STX_EXPR].head, &stream, &vm.pool);
	if(node) {
		printf("parsed\n");
	}

	printPoolInfo(&(vm.pool));
	// build grammar tree here
	//initGrammarRuleArray(&(vm.chunk.ruleArray), "./resources/testgrammar.txt", &(vm.pool));

	termVM(&vm);

/*
	MemPool scratch;
	initMemPool(&scratch);
#define BIGNUM 10000000

	int64_t *array = palloc(&scratch, BIGNUM * sizeof(int64_t));
	for(size_t i = 0; i < BIGNUM; i++) {
		array[i] = randI64(INT64_MAX);
		// printf("num %zu: %" PRId64 "\n", i, array[i]);
	}

	printPoolInfo(&scratch);
	termMemPool(&scratch);
*/
	return 0;
}
	/*
	// part 1
	
	char *source = read_file("./resources/test.tl");

	initScanner(source);

	size_t line = -1;
	while (true) {
		Token token = scanToken();
		if (token.line != line) {
			printf("%4zu ", token.line);
			line = token.line;
		} else {
			printf("   | ");
		}
		printf("%2d '%.*s'\n", token.type, (int)token.len, token.start); 

		if (token.type == TK_EOF) break;
	}

	free(source);

	// part 2

	MemPool pool;
	initMemPool(&pool);

	struct testStruct *ptrs = palloc(&pool, BIGNUM * sizeof(struct testStruct));

	for(size_t i = 0; i < BIGNUM; i++) {

		// ptrs[i] = palloc(&pool, sizeof(struct testStruct));
		ptrs[i].test = palloc(&pool, 1000 * sizeof(size_t));
		ptrs[i].test2 = palloc(&pool, 1000 * sizeof(size_t));
		for(size_t j = 0; j < 1000; j++) {
			ptrs[i].test[j] = j + 1;
			ptrs[i].test2[j] = 2 * (j + 1);
		}
		//printf("%zu\n", i + 1);
		//printf("addresses: %p, %p, %p\n", (void *)ptrs[i], (void *)&ptrs[i]->test, (void *)&ptrs[i]->test2);
		//printf("values: %zu, %zu\n", ptrs[i]->test, ptrs[i]->test2);
	}
	printf("%p\n", (void *)ptrs);

	// part 3

	Table table;
	initTable(&table);

	char *teststr = "teststr";
	int64_t a = 5;
	double testdbl = 1.200;

	insertKey(&table, "test", &a, ITR_TYPE);
	insertKey(&table, "test2", &a, ITR_TYPE);
	insertKey(&table, "test3", &a, ITR_TYPE);
	insertKey(&table, "test4", &a, ITR_TYPE);
	insertKey(&table, "test5", &a, ITR_TYPE);
	insertKey(&table, "test6", &testdbl, DBL_TYPE);
	insertKey(&table, "test7", &testdbl, DBL_TYPE);
	insertKey(&table, "test7", &a, ITR_TYPE);
	insertKey(&table, "test8", &a, ITR_TYPE);
	insertKey(&table, "test9", &a, ITR_TYPE);
	insertKey(&table, "test0", teststr, STR_TYPE);

	Object *obj = NULL;
	obj = getObject(&table, "test");
	printf("%ld\n", obj->val.itr);
	obj = getObject(&table, "test2");
	printf("%ld\n", obj->val.itr);
	obj = getObject(&table, "test3");
	printf("%ld\n", obj->val.itr);
	obj = getObject(&table, "test4");
	printf("%ld\n", obj->val.itr);
	obj = getObject(&table, "test5");
	printf("%ld\n", obj->val.itr);
	obj = getObject(&table, "test6");
	printf("%f\n", obj->val.dbl);
	obj = getObject(&table, "test7");
	printf("%f\n", obj->val.dbl);
	obj = getObject(&table, "test8");
	printf("%ld\n", obj->val.itr);
	obj = getObject(&table, "test9");
	printf("%ld\n", obj->val.itr);
	obj = getObject(&table, "test0");
	printf("%s\n", obj->val.str);
	//free_table(&table);

	int ret = removeKey(&table, "test0");
	printf("removal ret: %d\n", ret);

	obj = getObject(&table, "test0");
	if(!obj) {
		printf("was removed!\n");
	}	

	termTable(&table);

	printf("pool bytes used: %zu\n", getBytesUsed(&pool));
	printf("bytes used (GB): %f\n", (double) getBytesUsed(&pool) / (1024 * 1024 * 1024));
	printf("pool bytes allocd: %zu\n", getBytesAllocd(&pool));
	printf("bytes allocd (GB): %f\n", (double) getBytesAllocd(&pool) / (1024 * 1024 * 1024));
	termMemPool(&pool);

	int32_t *test = realloc(NULL, 16 * sizeof(int32_t));
	printf("%p\n", (void *)test);
	test = realloc(test, 8 * sizeof(int32_t));
	// pointer changes in Valgrind likely due to re-defining realloc()
	// pointer remains the same when run normally
	printf("%p\n", (void *)test);

	free(test);

	printf("IDX_MAX: %zu\n", IDX_MAX);
	printf("IDX_ERROR: %zu\n", IDX_ERROR);
	*/
