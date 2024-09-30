#ifndef AST_H
#define AST_H

#include <string.h>
#include "common.h"

#include "mempool.h"
#include "table.h"
#include "scanner.h"
#include "file.h"
#include "syntax_tree.h"
#include "syntax_types.h"

typedef enum {
	GRM_AND,
	GRM_OR,
	GRM_GROUP,
	GRM_IFONE,
	GRM_IFMANY,
} GRAMMAR_TYPE;

typedef enum {
	RULE_GRM,
	RULE_STX,
	RULE_TK,
} RULE_NODE_TYPE;

union u_TargetType {
	GRAMMAR_TYPE g;
	SYNTAX_TYPE s;
	TOKEN_TYPE t;
};

typedef struct s_RuleNode {

	RULE_NODE_TYPE node_type;

	union u_TargetType nested_type;

	struct s_RuleNode *rule_head;

	size_t n_children;
	struct s_RuleNode *children;
	struct s_RuleNode *parent;
} RuleNode;

typedef struct s_GrammarRule {
	SYNTAX_TYPE stype;
	RuleNode *head;
} GrammarRule;

typedef struct s_GrammarRuleArray {
	size_t n_rules;
	GrammarRule *rules;
} GrammarRuleArray;

typedef struct s_SyntaxSyntaxTree {
	RuleNode *current_rule;
	Token *current_token;
	//size_t token_idx;
} SyntaxSyntaxTree;

typedef struct s_TokenStream {
	Token *tk;
	size_t pos;
	size_t n;
} TokenStream;

int scanTokensFromSource(SyntaxTree *tree, char *source);

void initRuleNode(RuleNode *node);

void initTokenStream(TokenStream *stream, SyntaxTree *tree);

size_t getSemicolonOffset(Token *tokens);

RULE_NODE_TYPE getPrevalentType(Token *tokens, size_t n);

GRAMMAR_TYPE getPrevalentGrammarType(Token *tokens, size_t n);

size_t getRuleStartIndex(SYNTAX_TYPE type, Token *tokens, size_t n);

size_t countChildren(GRAMMAR_TYPE type, Token *tokens, size_t n);

size_t getDelimOffset(GRAMMAR_TYPE type, Token *tokens);

int fillGrammarNode(RuleNode *node, Token *tokens, size_t n, MemPool *pool);

int populateGrammarRulePointers(RuleNode *node, GrammarRuleArray *array);

int populateGrammarRuleArrayReferences(GrammarRuleArray *rule_array);

int initGrammarRuleArray(GrammarRuleArray *rule_array, char *fileName, MemPool *pool);

void defineParentPtrs(SyntaxNode *head);

void addChild(SyntaxNode *parent, SyntaxNode *child, MemPool *pool);

SyntaxNode *wrapNode(SyntaxNode *child, SYNTAX_TYPE stype, MemPool *pool);

SyntaxNode *parseAnd(RuleNode *rnode, TokenStream *stream, MemPool *pool);

SyntaxNode *parseOr(RuleNode *rnode, TokenStream *stream, MemPool *pool);

SyntaxNode *parseIfOne(RuleNode *rnode, TokenStream *stream, MemPool *pool);

SyntaxNode *parseIfMany(RuleNode *rnode, TokenStream *stream, MemPool *pool);

SyntaxNode *parseSyntax(RuleNode *rnode, TokenStream *stream, MemPool *pool);

SyntaxNode *parseToken(RuleNode *rnode, TokenStream *stream, MemPool *pool);

SyntaxNode *parseGrammar(RuleNode* rnode, TokenStream *stream, MemPool *pool);

char *syntaxTypeLiteralLookup(SYNTAX_TYPE type);

SYNTAX_TYPE getSNodeTypeFromLiteral(char *str);

SYNTAX_TYPE getSNodeTypeFromNChars(char *str, size_t n);

void printTokenStream(TokenStream *stream);

void printSyntaxNode(SyntaxNode *node, unsigned int indent);

void printGrammarNode(RuleNode *node, unsigned int indent);

void printGrammarRule(GrammarRule *rule);

void printGrammarRuleByIndex(GrammarRuleArray *array, int i);

void printGrammarRuleArray(GrammarRuleArray *array);

void fPrintGrammarNode(RuleNode *node, unsigned int indent, FILE *file);

void fPrintGrammarRule(GrammarRule *rule, FILE *file);

void fPrintGrammarRuleByIndex(GrammarRuleArray *array, int i, FILE *file);

void fPrintGrammarRuleArray(GrammarRuleArray *array, FILE *file);

#endif // AST_H
