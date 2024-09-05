#include "common.h"
#include "ast.h"

#include <stdio.h>

char *__node_type_literals[] = {

	"BLCK_TYPE_SCOPE",
	
	"CONS_TYPE_CLASS",
	"CONS_TYPE_FNDEF",
	
	"CONS_TYPE_IF",
	"CONS_TYPE_WHILE",
	"CONS_TYPE_FOR",

	"STMT_TYPE_DECLARE",
	"STMT_TYPE_ASSIGN",
	"STMT_TYPE_INIT",
	"STMT_TYPE_ECHO",
	"STMT_TYPE_BREAK",
	"STMT_TYPE_RETURN",
	
	"STMT_TYPE_EXIT",

	"EXPR_TYPE_GROUP",
	"EXPR_TYPE_COMPUTE",

	"TERM_TYPE_VAR",
	"TERM_TYPE_PROP",
	"TERM_TYPE_FNCALL",
	"TERM_TYPE_INDEX",
	"TERM_TYPE_NUM",
	"TERM_TYPE_STRING",

	"OPER_TYPE_MULT",
	"OPER_TYPE_DIV",
	"OPER_TYPE_SUM",
	"OPER_TYPE_DIFF",
	"OPER_TYPE_MODULUS",

	"OPER_TYPE_AND",
	"OPER_TYPE_OR",
	"OPER_TYPE_NOT",
	"OPER_TYPE_EQUAL_EQUAL",
	"OPER_TYPE_NOT_EQUAL",
	"OPER_TYPE_GREATER_EQUAL",
	"OPER_TYPE_LESS_EQUAL",
	
	"OPER_TYPE_EQUALS",
	"OPER_TYPE_PLUS_EQUAL",
	"OPER_TYPE_MINUS_EQUAL",
	"OPER_TYPE_STAR_EQUAL",
	"OPER_TYPE_DIV_EQUAL",
	"OPER_TYPE_MOD_EQUAL",

	"OPER_TYPE_INCREMENT",
	"OPER_TYPE_DECREMENT",
	"ERR_TYPE"
};

void initGrammarNode(GrammarNode *node) {
	node->isToken = false;
	node->type = 0;

	node->termToken = NULL;
	node->rule = NULL;

	node->n_children = 0;
	node->children = NULL;
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

NODE_TYPE getNodeTypeFromLiteral(char *str) {
	for (size_t i = 0; i < (sizeof(__node_type_literals) / sizeof(char *)); i++) {
		if(0 == strcmp(str, __node_type_literals[i])) {
			return i;
		}
	}
	return 0;
}