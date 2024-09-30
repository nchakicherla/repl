#include <stdio.h>

#include "common.h"

//#include "scanner.h"
#include "file.h"
#include "table.h"
#include "mempool.h"
#include "vm.h"
#include "ast.h"

#include "random.h"
#include "color.h"

#include <time.h>

#define INPUT_BUFFER_SIZE 512

int main(void) {

	char *grammar = "./resources/grammar.txt";

	VM vm;
	initVM(&vm, grammar);

	while(true) {
		//char *input_buffer = pzalloc(&vm.tree.pool, INPUT_BUFFER_SIZE);
		vm.tree.source = pzalloc(&vm.tree.pool, INPUT_BUFFER_SIZE);

		setColor(ANSI_CYAN);
		while(0 == strchr(vm.tree.source, '\n')) {
			fgets(vm.tree.source, INPUT_BUFFER_SIZE, stdin);
		}
		vm.tree.source[INPUT_BUFFER_SIZE - 1] = '\0';
		resetColor();

		scanTokensFromSource(&vm.tree, vm.tree.source);

		bool had_error = false;
		for(size_t i = 0; i < vm.tree.n_tokens; i++) {
			if(vm.tree.tokens[i].type == TK_ERROR) {
				setColor(ANSI_RED);
				printf("%.*s\n", (int)vm.tree.tokens[i].len, vm.tree.tokens[i].start);
				resetColor();
				had_error = true;
				break;
			}
		}

		if(had_error) {
			resetSyntaxTree(&vm.tree);
			continue;
		}

		TokenStream stream;

		for(size_t i = 0; i < vm.rule_array.n_rules; i++) {
			initTokenStream(&stream, &vm.tree);
			vm.tree.head = parseGrammar(vm.rule_array.rules[i].head, &stream, &vm.tree.pool);
			if(vm.tree.head) {
				if(stream.tk[stream.pos].type != TK_EOF) {
					//break; // ignore match if didn't consume all tokens in line
					continue;
				}
				if(vm.rule_array.rules[i].head->node_type == RULE_TK) {
					vm.tree.head = wrapNode(vm.tree.head, (SYNTAX_TYPE) i, &vm.tree.pool);
				} else {
					vm.tree.head->type = (SYNTAX_TYPE) i;
				}
				defineParentPtrs(vm.tree.head);
				printSyntaxNode(vm.tree.head, 0);
				break;
			}
		}

		if(0 == strncmp(vm.tree.source, ".exit", 5)) {
			break;
		}

		//printTokens(&vm);
		printPoolInfo(&vm.tree.pool);

		resetSyntaxTree(&vm.tree);
	}
	printPoolInfo(&vm.pool);

	termVM(&vm);
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


size_t fib(size_t n) {
	if(n == 0) return 0;
	if(n == 1) return 1;
	return fib(n - 2) + fib(n - 1);
}

size_t fibFast(size_t n) {
	if(n == 0) return 0;
	if(n == 1) return 1;
	size_t *arr = malloc(n + 1 * sizeof(size_t));
	size_t ret = 0;
	arr[0] = 0;
	arr[1] = 1;
	for(size_t i = 2; i <= n; i++) {
		arr[i] = arr[i - 2] + arr[i - 1];
	}
	ret = arr[n];
	free(arr);
	return ret;
}

	*/
