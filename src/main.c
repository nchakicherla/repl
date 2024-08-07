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

	init_scanner(source);

	size_t line = -1;
	for (;;) {
		Token token = scan_token();
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

#define BIGNUM 10000000

	struct testStruct **ptrs = palloc(&pool, BIGNUM * sizeof(struct testStruct *));

	for(size_t i = 0; i < BIGNUM; i++) {

		ptrs[i] = palloc(&pool, sizeof(struct testStruct));
		//ptrs[i]->test = i * 5000;
		//ptrs[i]->test2 = i * 10000;
		printf("%zu\n", i + 1);
		printf("addresses: %p, %p, %p\n", 	(void *)ptrs[i], (void *)&ptrs[i]->test, (void *)&ptrs[i]->test2);
		//printf("values: %zu, %zu\n", ptrs[i]->test, ptrs[i]->test2);
	}
	printf("%p\n", (void *)ptrs);
	freeMemPool(&pool);
	return 0;
}
