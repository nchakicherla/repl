#include "common.h"
#include "ast.h"

#include <stdio.h>

char *__node_type_literals[] = {

	"STX_SCOPE",
	
	"STX_CLASS",
	"STX_FNDEF",
	
	"STX_IF",
	"STX_WHILE",
	"STX_FOR",

	"STX_DECLARE",
	"STX_ASSIGN",
	"STX_INIT",
	"STX_ECHO",
	"STX_BREAK",
	"STX_RETURN",
	
	"STX_EXIT",

	"STX_GROUPING",
	"STX_COMPUTE",

	"STX_VAR",
	"STX_PROP",
	"STX_FNCALL",
	"STX_INDEX",
	"STX_NUM",
	"STX_STRING",

	"STX_MULT",
	"STX_DIV",
	"STX_SUM",
	"STX_DIFF",
	"STX_MOD",

	"STX_AND",
	"STX_OR",
	"STX_NOT",
	"STX_EQUAL_EQUAL",
	"STX_NOT_EQUAL",
	"STX_GREATER_EQUAL",
	"STX_LESS_EQUAL",

	"STX_EQUAL",
	"STX_PLUS_EQUAL",
	"STX_MINUS_EQUAL",
	"STX_STAR_EQUAL",
	"STX_DIV_EQUAL",
	"STX_MOD_EQUAL",

	"STX_INCREMENT",
	"STX_DECREMENT",
	"ERR_TYPE"
};

void initGrammarNode(GrammarNode *node) {
	return;
}

void initGrammarParser(GrammarParser *parser) {
	parser->n_tokens = 0;
	parser->tokens = NULL;

	parser->par_node = NULL;
	parser->curr_node = NULL;

	initMemPool(&(parser->pool));
	return;
}

SYNTAX_TYPE getSNodeTypeFromLiteral(char *str) {
	for (size_t i = 0; i < (sizeof(__node_type_literals) / sizeof(char *)); i++) {
		if(0 == strcmp(str, __node_type_literals[i])) {
			return i;
		}
	}
	return 0;
}