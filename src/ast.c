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

void __printTokens(Token *tokens, size_t n) {
	for(size_t i = 0; i < n; i++) {
		printf("LINE: %6zu TK%6zu: TYPE: %16s - \"%.*s\"\n", 
			tokens[i].line,
			i,
			tokenTypeLiteralLookup(tokens[i].type), 
			(int)tokens[i].len, tokens[i].start);
	}
	return;
}


size_t getSemicolonOffset(Token *tokens) {
	size_t ret = 0;
	//printf("in semioffset: %.*s\n", (int)tokens[0].len, tokens[0].start);
	while(tokens[ret].type != TK_SEMICOLON) {
		//printf("ret: %zu, type: %s\n", ret, tokenTypeLiteralLookup(tokens[ret].type));
		ret++;
	}
	return ret;
}

GTREE_NODE_TYPE getPrevalentType(Token *tokens, size_t n) {
	if(n == 1) {
		switch(tokens[0].start[0]) {
			case 'S': return GTREE_STX;
			case 'T': return GTREE_TK;
		}
	}
	return GTREE_GRM;
}

GRAMMAR_TYPE getPrevalentGrammarType(Token *tokens, size_t n) {
	bool inGroup = false;
	bool commaFound = false;
	bool pipeFound = false;

	for(size_t i = 0; i < n; i++) {
		switch(tokens[i].type) {
			case TK_LPAREN:
			case TK_LBRACE:
			case TK_LSQUARE:
				inGroup = true;
				continue;
			case TK_RPAREN:
			case TK_RBRACE:
			case TK_RSQUARE:
				inGroup = false;
				continue;
			default:
				break;
		}
		if(!inGroup && tokens[i].type == TK_COMMA) {
			commaFound = true;
		}
		if(!inGroup && tokens[i].type == TK_PIPE) {
			pipeFound = true;
		}
	}
	if(commaFound) return GRM_AND;
	if(pipeFound) return GRM_OR;

	if(tokens[0].type == TK_LPAREN) return GRM_GROUP;
	if(tokens[0].type == TK_LSQUARE) return GRM_IFONE;
	if(tokens[0].type == TK_LBRACE) return GRM_IFMANY;
	return GRM_AND;
}

size_t getRuleStartIndex(SYNTAX_TYPE type, Token *tokens, size_t n) {
	char *typeLiteral = syntaxTypeLiteralLookup(type);
	size_t literalLen = strlen(typeLiteral);
	for(size_t i = 0; i < n; i++) {
		if(tokens[i].len == literalLen) {
			if(0 == strncmp(typeLiteral, tokens[i].start, literalLen) && tokens[i + 1].type == TK_EQUAL) {
				return i + 2;
			}
		}
	}
	return 0;
}

size_t countChildren(GRAMMAR_TYPE type, Token *tokens, size_t n) { // should only be called on GRM_AND, GRM_OR
	size_t n_children = 1;
	bool inGroup = false;
	TOKEN_TYPE delim;
	switch(type) {
		case GRM_AND:
			delim = TK_COMMA;
			break;
		case GRM_OR:
			delim = TK_PIPE;
			break;
		case GRM_GROUP:
		case GRM_IFONE:
		case GRM_IFMANY:
		default:
			break;
	}

	for(size_t i = 0; i < n; i++) {
		switch(tokens[i].type) {
			case TK_LPAREN:
			case TK_LBRACE:
			case TK_LSQUARE:
				inGroup = true;
				continue;
			case TK_RPAREN:
			case TK_RBRACE:
			case TK_RSQUARE:
				inGroup = false;
				continue;
			default:
				break;
		}
		if(!inGroup && tokens[i].type == delim) {
			n_children++;
		}
	}

	return n_children;
}

size_t getDelimOffset(GRAMMAR_TYPE type, Token *tokens) {
	bool inGroup = false;
	TOKEN_TYPE delim;
	size_t ret = 0;
	switch(type) {
		case GRM_AND:
			delim = TK_COMMA;
			break;
		case GRM_OR:
			delim = TK_PIPE;
			break;
		case GRM_GROUP:
		case GRM_IFONE:
		case GRM_IFMANY:
		default:
			break;
	}
	while(true) {
		printf("here3\n");
		switch(tokens[ret].type) {
			case TK_LPAREN:
			case TK_LBRACE:
			case TK_LSQUARE:
				inGroup = true;
				ret++;
				continue;
			case TK_RPAREN:
			case TK_RBRACE:
			case TK_RSQUARE:
				inGroup = false;
				ret++;
				continue;
			default:
				break;
		}
		if(!inGroup && tokens[ret].type == delim) {
			return ret;
		}
		ret++;
	}
	return 0;
}

int fillGrammarNode(GrammarNode *node, Token *tokens, size_t n, MemPool *pool) {
	GTREE_NODE_TYPE nodeType = getPrevalentType(tokens, n);
	node->node_type = nodeType;
	printf("prev type: %d, n: %zu\n", nodeType, n);
	printf("before switch:");
	__printTokens(tokens, n);
	//Token *start = tokens;
	//Token *end = tokens;

	switch(nodeType) {
		case GTREE_GRM: {
			printf("prev grm type: ");
			switch(getPrevalentGrammarType(tokens, n)) {
				case GRM_AND: {
					node->nested_type.g = GRM_AND;
					node->n_children = countChildren(GRM_AND, tokens, n);
					node->children = palloc(pool, node->n_children * sizeof(GrammarNode));
					printf("GRM_AND\n");
					printf("n_children: %zu\n", node->n_children);

					Token *start = tokens;
					size_t delimOffset = 0;
					for(size_t i = 0; i < node->n_children - 1; i++) {
						delimOffset = getDelimOffset(GRM_AND, start);
						printf("in fill node i = %zu\n", i);
						__printTokens(start, delimOffset);
						printf("delimoffset: %zu\n", delimOffset);
						fillGrammarNode(&(node->children[i]), start, delimOffset, pool);
						start = start + delimOffset + 1;
					}
					printf("%p, %p, %zu\n", (void *)tokens, (void *)start, start - tokens);
					printf("arg: %zu\n", n - (size_t)(start - tokens));
					fillGrammarNode(&(node->children[node->n_children - 1]), start, n - (size_t)(start - tokens), pool);
					
					break;
				}
				case GRM_OR: {
					node->nested_type.g = GRM_OR;
					node->n_children = countChildren(GRM_OR, tokens, n);
					node->children = palloc(pool, node->n_children * sizeof(GrammarNode));
					printf("GRM_OR\n");
					printf("n_children: %zu\n", node->n_children);
					break;
				}
				case GRM_GROUP: {
					node->nested_type.g = GRM_GROUP;
					printf("GRM_GROUP\n");
					break;
				}
				case GRM_IFONE: {
					node->nested_type.g = GRM_IFONE;
					printf("GRM_IFONE\n");
					break;
				}
				case GRM_IFMANY: {
					node->nested_type.g = GRM_IFMANY;
					printf("GRM_IFMANY\n");
					break;
				}
				default: 
					break;
			}
			break;
		}
		case GTREE_STX: {
			printf("stx node\n");
			node->nested_type.s = getSNodeTypeFromNChars(tokens[0].start, tokens[0].len);
			break;
		}
		case GTREE_TK: {
			printf("tk node\n");
			node->nested_type.t = getTKTypeFromNChars(tokens[0].start, tokens[0].len);
			break;
		}
	}
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

	for(size_t i = 0; i < n_tokens; i++) {
		printf("LINE: %6zu TK%6zu: TYPE: %16s - \"%.*s\"\n", 
			tokens[i].line,
			i,
			tokenTypeLiteralLookup(tokens[i].type), 
			(int)tokens[i].len, tokens[i].start);
	}

	ruleArray->n_rules = STX_ERR - STX_SCOPE;
	ruleArray->rules = palloc(pool, ruleArray->n_rules * sizeof(GrammarRule));

	for(size_t i = 0; i < ruleArray->n_rules; i++) {
		ruleArray->rules[i].stype = (SYNTAX_TYPE)i;
		ruleArray->rules[i].head = palloc(pool, sizeof(GrammarNode));

		size_t ruleStart = getRuleStartIndex((SYNTAX_TYPE)i, tokens, n_tokens);
		size_t semiOffset = getSemicolonOffset(&tokens[ruleStart]);
		printf("\nBuilding rule for STX: %s\n", syntaxTypeLiteralLookup(i));
		//printf("rule starts: %zu\n", ruleStart);
		//printf("start token: %.*s\n", (int)tokens[ruleStart].len, tokens[ruleStart].start);
		//printf("semi offset: %zu\n", semiOffset);

		fillGrammarNode(ruleArray->rules[i].head, &tokens[ruleStart], semiOffset, pool);
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
	return STX_ERR;
}

SYNTAX_TYPE getSNodeTypeFromNChars(char *str, size_t n) {
	for (size_t i = 0; i < (sizeof(__node_type_literals) / sizeof(char *)); i++) {
		if(0 == strncmp(str, __node_type_literals[i], n)) {
			return i;
		}
	}
	return STX_ERR;
}
