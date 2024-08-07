#include <stdio.h>

#include "common.h"

#include "scanner.h"
#include "file.h"
#include "vm.h"
#include "table.h"
#include "memory.h"

struct testStruct {
	size_t test[100];
	size_t test2[100];
};

int main(void) {
	char *source = read_file("./resources/test.tl");

	initScanner(source);

	size_t line = -1;
	for (;;) {
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

	MemPool pool;
	initMemPool(&pool, 1024);

#define BIGNUM 100

	struct testStruct **ptrs = palloc(&pool, BIGNUM * sizeof(struct testStruct *));

	for(size_t i = 0; i < BIGNUM; i++) {

		ptrs[i] = palloc(&pool, sizeof(struct testStruct));
		//ptrs[i]->test = i * 5000;
		//ptrs[i]->test2 = i * 10000;
		printf("%zu\n", i + 1);
		printf("addresses: %p, %p, %p\n", (void *)ptrs[i], (void *)&ptrs[i]->test, (void *)&ptrs[i]->test2);
		//printf("values: %zu, %zu\n", ptrs[i]->test, ptrs[i]->test2);
	}
	printf("%p\n", (void *)ptrs);

	Table table;
	initTable(&table, &pool);


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
	getObject(&table, "test", &obj);
	printf("%ld\n", obj->val.itr);
	getObject(&table, "test2", &obj);
	printf("%ld\n", obj->val.itr);
	getObject(&table, "test3", &obj);
	printf("%ld\n", obj->val.itr);
	getObject(&table, "test4", &obj);
	printf("%ld\n", obj->val.itr);
	getObject(&table, "test5", &obj);
	printf("%ld\n", obj->val.itr);
	getObject(&table, "test6", &obj);
	printf("%f\n", obj->val.dbl);
	getObject(&table, "test7", &obj);
	printf("%f\n", obj->val.dbl);
	getObject(&table, "test8", &obj);
	printf("%ld\n", obj->val.itr);
	getObject(&table, "test9", &obj);
	printf("%ld\n", obj->val.itr);
	getObject(&table, "test0", &obj);
	printf("%s\n", obj->val.str);
	//free_table(&table);
	freeMemPool(&pool);
	return 0;
}
