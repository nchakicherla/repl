#include "common.h"
#include "scanner.h"
#include "ast.h"
#include "file.h"

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

void initChunk(Chunk *chunk) {
	chunk->source = NULL;

	chunk->n_tokens = 0;
	chunk->tokens = NULL;

	chunk->head = NULL;
	initMemPool(&(chunk->pool));
	return;
}

void resetChunk(Chunk *chunk) {
	chunk->source = NULL;

	chunk->n_tokens = 0;
	chunk->tokens = NULL;
	
	chunk->head = NULL;
	resetMemPool(&(chunk->pool));
}

void termChunk(Chunk *chunk) {
	termMemPool(&(chunk->pool));
	return;
}

int scanTokensFromSource(Chunk *chunk, char *source) {

	size_t len = strlen(source);
	chunk->source = palloc(&(chunk->pool), len + 1);
	memcpy(chunk->source, source, len + 1);
	chunk->source[len] = '\0';

	initScanner(chunk->source);

	// scanner passes twice to get count then write to chunk

	size_t n_tokens = 0;
	while(true) {
		Token token = scanToken();
		n_tokens++;
		if (token.type == TK_EOF) break;
	}

	chunk->n_tokens = n_tokens;
	chunk->tokens = palloc(&(chunk->pool), (n_tokens * sizeof(Token)));

	initScanner(chunk->source);

	for(size_t i = 0; i < n_tokens; i++) {
		chunk->tokens[i] = scanToken();
	}
	return 0;
}

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
size_t getSemicolonOffset(Token *tokens) {
	size_t ret = 0;
	printf("in semioffset: %.*s\n", (int)tokens[0].len, tokens[0].start);
	while(tokens[ret].type != TK_SEMICOLON) {
		printf("ret: %zu, type: %s\n", ret, tokenTypeLiteralLookup(tokens[ret].type));
		ret++;
	}
	return ret;
}

GTREE_NODE_TYPE getPrevalentType(Token *tokens, size_t n) {
	if(n == 1) {
		printf("here\n");
		printf("c: %c\n", tokens[0].start[0]);
		switch(tokens[0].start[0]) {
			case 'S': return GTREE_STX;
			case 'T': return GTREE_TK;
		}
	}
	return GTREE_GRM;
}

size_t getRuleStartIndex(SYNTAX_TYPE type, Token *tokens, size_t n) {
	for(size_t i = 0; i < n; i++) {
		if(0 == strncmp(tokens[i].start, syntaxTypeLiteralLookup(type), tokens[i].len)) {
			if(tokens[i + 1].type == TK_EQUAL) return i + 2;
		}
	}
	return 0;
}

int makeGrammarNode(GrammarNode *node, GTREE_NODE_TYPE type, Chunk *chunk, size_t n) {

	return 0;
}

int initGrammarRuleArray(GrammarRuleArray *ruleArray, char *fileName, MemPool *pool) {

	char *source = readFile(fileName, pool);
	initScanner(source);

	size_t n_tokens = 0;
	while(true) {
		Token token = scanToken();
		n_tokens++;
		if (token.type == TK_EOF) break;
	}

	Token *tokens = palloc(pool, (n_tokens * sizeof(Token)));

	initScanner(source);

	for(size_t i = 0; i < n_tokens; i++) {
		tokens[i] = scanToken();
	}

	ruleArray->n_rules = STX_ERR - STX_SCOPE;
	ruleArray->rules = palloc(pool, ruleArray->n_rules * sizeof(GrammarRule));

	for(size_t i = 0; i < ruleArray->n_rules; i++) {
		ruleArray->rules[i].stype = (SYNTAX_TYPE)i;
		size_t ruleStart = getRuleStartIndex((SYNTAX_TYPE)i, tokens, n_tokens);
		printf("set STX type: %s\n", syntaxTypeLiteralLookup(i));
		printf("rule starts: %zu\n", ruleStart);
		printf("start token: %.*s\n", (int)tokens[ruleStart].len, tokens[ruleStart].start);
		printf("semi offset: %zu\n", getSemicolonOffset(&tokens[ruleStart]));
		printf("prev type: %d\n", getPrevalentType(&tokens[ruleStart], getSemicolonOffset(&tokens[ruleStart])));
	}

	return 0;
}

char *syntaxTypeLiteralLookup(SYNTAX_TYPE type) {
	return __node_type_literals[type];
}

SYNTAX_TYPE getSNodeTypeFromLiteral(char *str) {
	for (size_t i = 0; i < (sizeof(__node_type_literals) / sizeof(char *)); i++) {
		if(0 == strcmp(str, __node_type_literals[i])) {
			return i;
		}
	}
	return 0;
}
