#ifndef AST_H
#define AST_H

#include "scanner.h"

//typedef struct s_Block;
//typedef struct s_Statement;
//typedef struct s_Expression;
/*
typedef enum {
	BLCK_TYPE_SCOPE,
	BLCK_TYPE_FN_DECL,
	BLCK_TYPE_IF,
	BLCK_TYPE_WHILE,
	BLCK_TYPE_FOR,
	BLCK_TYPE_RANGE,
} BLCK_TYPE;

typedef enum {
	STMT_TYPE_DECLARE,
	STMT_TYPE_ASSIGN,
	STMT_TYPE_RETURN,
	STMT_TYPE_EXIT,
} STMT_TYPE;

typedef enum {
	EXPR_ECHO,
	EXPR_MATH,
	EXPR_TRUTHY,
	EXPR_FNCALL,
	EXPR_UNARY,
} EXPR_TYPE;
*/
typedef enum {
	BLCK_TYPE_SCOPE,
	BLCK_TYPE_CLASS,
	BLCK_TYPE_FN,
	
	BLCK_TYPE_IF,
	BLCK_TYPE_WHILE,
	BLCK_TYPE_DOWHILE,
	BLCK_TYPE_FOR,
	BLCK_TYPE_RANGE,

	STMT_TYPE_DECLARE,
	// STMT_TYPE_ASSIGN,
	STMT_TYPE_INIT,
	STMT_TYPE_ECHO,
	STMT_TYPE_RETURN,
	STMT_TYPE_EXIT,
	STMT_TYPE_FNCALL,
	//STMT_TYPE_FNCALL,

	EXPR_TYPE_FNCALL,
	EXPR_TYPE_TRUTHY,
	// EXPR_TYPE_VAR,
	EXPR_TYPE_MATH,
	// EXPR_TYPE_UNARY,

	// arithmetic
	OPER_TYPE_SUM,
	OPER_TYPE_DIFF,
	OPER_TYPE_DIV,
	OPER_TYPE_MULT,
	OPER_TYPE_MODULUS,

	// logical
	OPER_TYPE_AND,
	OPER_TYPE_OR,
	OPER_TYPE_NOT,

	// assignment
	OPER_TYPE_EQUALS,
	OPER_TYPE_PLUSEQUALS,
	OPER_TYPE_MINUSEQUALS,
	OPER_TYPE_TIMESEQUALS,
	OPER_TYPE_DIVEQUALS,
	OPER_TYPE_MODEQUALS,

	// postfix increment / decrement
	OPER_TYPE_INCREMENT,
	OPER_TYPE_DECREMENT,

	// l-value
	LABEL_TYPE,
} NODE_TYPE;

typedef struct s_Node {
	NODE_TYPE type;
	size_t n_children;
	struct s_Node **children;
} Node;

#endif // AST_H