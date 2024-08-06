#include <stdio.h>

#include "scanner.h"
#include "file.h"

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
}
