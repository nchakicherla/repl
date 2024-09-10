#ifndef AST_H
#define AST_H

#include <string.h>
#include "common.h"

#include "scanner.h"
#include "memory.h"
#include "table.h"
#include "chunk.h"

typedef enum {
	STX_SCOPE,
	
	STX_CLASS,
	STX_FNDEF,
	
	STX_IF,
	STX_WHILE,
	STX_FOR,

	STX_DECLARE,
	STX_ASSIGN,
	STX_INIT,
	STX_ECHO,
	STX_BREAK,
	STX_RETURN,

	STX_EXIT,

	STX_GEXPR,
	STX_EXPR,

	STX_VTYPE,
	STX_VAR,
	STX_MEMBER,
	STX_THIS,
	STX_FNCALL,
	STX_INDEX,
	STX_NUM,
	STX_STRING,

	STX_ARITHOP,
	STX_BOOLOP,

	STX_MULT,
	STX_DIV,
	STX_SUM,
	STX_DIFF,
	STX_MOD,

	STX_TRUE,
	STX_FALSE,
	STX_AND,
	STX_OR,
	STX_NOT,
	STX_EQUAL_EQUAL,
	STX_NOT_EQUAL,
	STX_GREATER_EQUAL,
	STX_LESS_EQUAL,

	STX_EQUAL,
	STX_PLUS_EQUAL,
	STX_MINUS_EQUAL,
	STX_STAR_EQUAL,
	STX_DIV_EQUAL,
	STX_MOD_EQUAL,

	STX_INCREMENT,
	STX_DECREMENT,
	STX_ERR,
} SYNTAX_TYPE;

typedef enum {
	GRM_AND,
	GRM_OR,
	GRM_GROUP,
	GRM_IFONE,
	GRM_IFMANY,
} GRAMMAR_TYPE;

typedef enum {
	GTREE_GRM,
	GTREE_STX,
	GTREE_TK,
} GTREE_NODE_TYPE;

union u_TargetType {
	GRAMMAR_TYPE g;
	SYNTAX_TYPE s;
	TOKEN_TYPE t;
};

typedef struct s_SyntaxNode {
	SYNTAX_TYPE type;

	bool evaluated;
	bool isBool;
	Object object;

	size_t n_children;
	struct s_SyntaxNode *children;
} SyntaxNode;

typedef struct s_GrammarNode {

	GTREE_NODE_TYPE node_type;

	union u_TargetType type;

	struct s_GrammarNode *rule_head;
	// fn pointer to non terminal definition?

	size_t n_children;
	struct s_GrammarNode *children;
} GrammarNode;

typedef struct s_GrammarRule {
	SYNTAX_TYPE stype;
	GrammarNode *head;
} GrammarRule;

typedef struct s_GrammarRuleArray {
	size_t n_rules;
	GrammarRule *rules;
} GrammarRuleArray;
/*
typedef struct s_GrammarParser {
	size_t n_tokens;
	Token *tokens;
	GrammarNode *par_node;
	GrammarNode *curr_node;
	MemPool pool;
} GrammarParser;
*/
void initGrammarNode(GrammarNode *node);

//void initGrammarParser(GrammarParser *parser);

GTREE_NODE_TYPE getPrevalentType(Token *tokens, size_t start, size_t n);

int makeGrammarNode(GrammarNode *node, GTREE_NODE_TYPE type, Chunk *chunk, size_t start, size_t n);

SYNTAX_TYPE getSNodeTypeFromLiteral(char *str);

#endif // AST_H
