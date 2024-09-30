#ifndef PARSER_H
#define PARSER_H

#include "mempool.h"
#include "syntax_types.h"
#include "syntax_node.h"

typedef struct s_Parser {
	char *source;

	size_t n_tokens;
	Token *tokens;

	SyntaxNode *head;

	MemPool pool;
	// int err;
} Parser;

void initParser(Parser *parser);

void resetParser(Parser *parser);

void termParser(Parser *parser);
#endif // PARSER_H