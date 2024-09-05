#ifndef AST_H
#define AST_H

#include "common.h"
#include <string.h>

#include "scanner.h"
#include "memory.h"
#include "table.h"

typedef enum {
	BLCK_TYPE_SCOPE,
	
	CONS_TYPE_CLASS,
	CONS_TYPE_FNDEF,
	
	CONS_TYPE_IF,
	CONS_TYPE_WHILE,
	CONS_TYPE_FOR,

	STMT_TYPE_DECLARE,
	STMT_TYPE_ASSIGN,
	STMT_TYPE_INIT,
	STMT_TYPE_ECHO,
	STMT_TYPE_BREAK,
	STMT_TYPE_RETURN,

	STMT_TYPE_EXIT,

	EXPR_TYPE_GROUP,
	EXPR_TYPE_COMPUTE,

	TERM_TYPE_VAR,
	TERM_TYPE_PROP,
	TERM_TYPE_FNCALL,
	TERM_TYPE_INDEX,
	TERM_TYPE_NUM,
	TERM_TYPE_STRING,

	OPER_TYPE_MULT,
	OPER_TYPE_DIV,
	OPER_TYPE_SUM,
	OPER_TYPE_DIFF,
	OPER_TYPE_MODULUS,

	OPER_TYPE_AND,
	OPER_TYPE_OR,
	OPER_TYPE_NOT,
	OPER_TYPE_EQUAL_EQUAL,
	OPER_TYPE_NOT_EQUAL,
	OPER_TYPE_GREATER_EQUAL,
	OPER_TYPE_LESS_EQUAL,

	OPER_TYPE_EQUALS,
	OPER_TYPE_PLUS_EQUAL,
	OPER_TYPE_MINUS_EQUAL,
	OPER_TYPE_STAR_EQUAL,
	OPER_TYPE_DIV_EQUAL,
	OPER_TYPE_MOD_EQUAL,

	OPER_TYPE_INCREMENT,
	OPER_TYPE_DECREMENT,
	ERR_TYPE,
} NODE_TYPE;

typedef enum {
	GRAM_SEQ,
	GRAM_OR,
	GRAM_ZEROONE,
	GRAM_ZEROMANY,
} RULE_TYPE;

/*
"int a = 3 * (5 + 4);"

	BLCK_TYPE_SCOPE: (main)
		- STMT_TYPE_INIT: "int a = 3 * (5 + 4);"
			- LVAL_TYPE_VAR "a"
			- OPER_TYPE_EQUALS "="
			- EXPR_TYPE_COMPUTE:
				- TERM_TYPE_PROP "3"
				- OPER_TYPE_MULT "*""
				- EXPR_TYPE_COMPUTE:
					- TERM_TYPE_FACTOR "5"
					- OPER_TYPE_SUM "+"
					- TERM_TYPE_FACTOR "4"
*/


typedef struct s_SyntaxNode {
	NODE_TYPE type;

	bool evaluated;
	Object object;

	size_t n_children;
	struct s_SyntaxNode *children;
} SyntaxNode;

typedef struct s_GrammarNode {
	bool isToken;
	size_t type;

	Token *termToken;
	struct s_GrammarNode *rule;
	// fn pointer to terminal definition?

	size_t n_children;
	struct s_GrammarNode *children;
} GrammarNode;

typedef struct s_GrammarRule {
	NODE_TYPE type;
	GrammarNode *head;
} GrammarRule;

typedef struct s_RuleArray {
	size_t n_rules;
	GrammarRule *rules;
} RuleArray;

typedef struct s_GrammarParser {
	size_t n_tokens;
	Token *tokens;
	GrammarNode *par_node;
	GrammarNode *curr_node;
	MemPool pool;
} GrammarParser;

void initGrammarNode(GrammarNode *node);

void initGrammarParser(GrammarParser *parser);

NODE_TYPE getNodeTypeFromLiteral(char *str);

#endif // AST_H