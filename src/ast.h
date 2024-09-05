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
} SYNTAX_TYPE;

typedef enum {
	GRAM_SEQ,
	GRAM_OR,
	GRAM_IFONE,
	GRAM_IFMANY,
} RULE_TYPE;

typedef enum {
	GRAMMAR,
	SYNTAX,
	TOKEN,
} TARGET_TYPE;

typedef struct s_SyntaxNode {
	SYNTAX_TYPE type;

	bool evaluated;
	Object object;

	size_t n_children;
	struct s_SyntaxNode *children;
} SyntaxNode;

typedef struct s_GrammarNode {

	TARGET_TYPE ntype;

	int64_t target_type;

	struct s_GrammarNode *rule_head;
	// fn pointer to non terminal definition?

	size_t n_children;
	struct s_GrammarNode *children;
} GrammarNode;

typedef struct s_GrammarRule {
	SYNTAX_TYPE stype;
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

SYNTAX_TYPE getSNodeTypeFromLiteral(char *str);

#endif // AST_H