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

	"STX_GEXPR",
	"STX_EXPR",

	"STX_VTYPE",
	"STX_VAR",
	"STX_MEMBER",
	"STX_THIS",
	"STX_FNCALL",
	"STX_INDEX",
	"STX_NUM",
	"STX_STRING",

	"STX_ARITHOP",
	"STX_BOOLOP",

	"STX_MULT",
	"STX_DIV",
	"STX_SUM",
	"STX_DIFF",
	"STX_MOD",

	"STX_TRUE",
	"STX_FALSE",
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
	"STX_ERR"
};

void initGrammarNode(GrammarNode *node) {
	node->rule_head = NULL;
	node->children = NULL;
	return;
}
/*
void initGrammarParser(GrammarParser *parser) {
	parser->n_tokens = 0;
	parser->tokens = NULL;

	parser->par_node = NULL;
	parser->curr_node = NULL;

	initMemPool(&(parser->pool));
	return;
}
*/
GTREE_NODE_TYPE getPrevalentType(Tokens *tokens, size_t start, size_t n);

int makeGrammarNode(GrammarNode *node, GTREE_NODE_TYPE type, Chunk *chunk, size_t start, size_t n) {

	return 0;
}

SYNTAX_TYPE getSNodeTypeFromLiteral(char *str) {
	for (size_t i = 0; i < (sizeof(__node_type_literals) / sizeof(char *)); i++) {
		if(0 == strcmp(str, __node_type_literals[i])) {
			return i;
		}
	}
	return 0;
}
