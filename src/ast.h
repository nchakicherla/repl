#ifndef AST_H
#define AST_H

#include "scanner.h"
#include "memory.h"
#include "table.h"

typedef enum {
	ERR_TYPE,
	BLCK_TYPE_SCOPE,
	
	BLCK_TYPE_CLASS,
	BLCK_TYPE_FNDEF,
	
	BLCK_TYPE_IF,
	BLCK_TYPE_WHILE,
	BLCK_TYPE_FOR,

	STMT_TYPE_DECLARE,
	STMT_TYPE_ASSIGN,
	STMT_TYPE_INIT,
	STMT_TYPE_ECHO,
	STMT_TYPE_BREAK,
	STMT_TYPE_RETURN,

	STMT_TYPE_EXIT,

	EXPR_TYPE_GROUP,
	EXPR_TYPE_EVAL,
	EXPR_TYPE_BOOL,

	TERM_TYPE_VAR,
	TERM_TYPE_FNCALL,
	TERM_TYPE_INDEX,
	TERM_TYPE_PROP,
	TERM_TYPE_FACTOR,
	TERM_TYPE_STRING,

	LVAL_TYPE_VAR_DECL,
	LVAL_TYPE_VAR,
	LVAL_TYPE_PROP,

	OPER_TYPE_MULT,
	OPER_TYPE_DIV,
	OPER_TYPE_SUM,
	OPER_TYPE_DIFF,
	OPER_TYPE_MOD,

	OPER_TYPE_AND,
	OPER_TYPE_OR,
	OPER_TYPE_NOT,

	OPER_TYPE_EQUALS,
	OPER_TYPE_EQEQUALS,
	OPER_TYPE_PLUSEQUALS,
	OPER_TYPE_MINUSEQUALS,
	OPER_TYPE_TIMESEQUALS,
	OPER_TYPE_DIVEQUALS,
	OPER_TYPE_MODEQUALS,

	OPER_TYPE_INCREMENT,
	OPER_TYPE_DECREMENT,
} NODE_TYPE;

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


typedef struct s_Node {
	NODE_TYPE type;

	bool evaluated;
	Object object;

	size_t n_children;
	struct s_Node *children;
} Node;

#endif // AST_H