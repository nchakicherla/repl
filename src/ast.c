#include "common.h"
#include "scanner.h"
#include "ast.h"
#include "file.h"

#include <stdio.h>

char *__syntaxTypeLiterals[] = {

	"STX_SCOPE", "STX_CLASS", "STX_FNDEF", "STX_IF", "STX_WHILE", "STX_FOR",

	"STX_DECLARE", "STX_ASSIGN", "STX_INIT", "STX_ECHO", "STX_BREAK", 
	"STX_RETURN", "STX_EXIT",

	"STX_GEXPR", "STX_EXPR",

	"STX_VTYPE", "STX_VAR", "STX_MEMBER", "STX_THIS", "STX_FNCALL", "STX_INDEX",
	"STX_NUM", "STX_STRING",

	"STX_ARITHOP", "STX_BOOLOP",

	"STX_MULT", "STX_DIV", "STX_SUM", "STX_DIFF", "STX_MOD",

	"STX_TRUE", "STX_FALSE", "STX_AND", "STX_OR", "STX_NOT", "STX_EQUAL_EQUAL",
	"STX_NOT_EQUAL", "STX_GREATER_EQUAL", "STX_LESS_EQUAL",

	"STX_EQUAL", "STX_PLUS_EQUAL", "STX_MINUS_EQUAL", "STX_STAR_EQUAL",
	"STX_DIV_EQUAL", "STX_MOD_EQUAL",

	"STX_INCREMENT", "STX_DECREMENT", "STX_ERR"
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

void initSyntaxNode(SyntaxNode *node) {
	node->children = NULL;
	node->parent = NULL;
}

void initGrammarNode(RuleNode *node) {
	node->rule_head = NULL;
	node->n_children = 0;
	node->children = NULL;
	node->parent = NULL;
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

void __printNTabs(unsigned int n) {
	for(unsigned int i = 0; i < n; i++) {
		putchar('\t');
	}
	return;
}

void __fPrintNTabs(unsigned int n, FILE *file) {
	for(unsigned int i = 0; i < n; i++) {
		fputc('\t', file);
	}
	return;
}

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
	while(tokens[ret].type != TK_SEMICOLON) {
		ret++;
	}
	return ret;
}

RULE_NODE_TYPE getPrevalentType(Token *tokens, size_t n) {
	if(n == 1) {
		switch(tokens[0].start[0]) {
			case 'S': {
				return RULE_STX;
			}
			case 'T': {
				return RULE_TK;
			}
		}
	}
	return RULE_GRM;
}

GRAMMAR_TYPE getPrevalentGrammarType(Token *tokens, size_t n) {
	size_t inGroup = 0;
	bool commaFound = false;
	bool pipeFound = false;

	for(size_t i = 0; i < n; i++) {
		switch(tokens[i].type) {
			case TK_LPAREN:
			case TK_LBRACE:
			case TK_LSQUARE:
				inGroup++;
				continue;
			case TK_RPAREN:
			case TK_RBRACE:
			case TK_RSQUARE:
				inGroup--;
				continue;
			default:
				break;
		}
		if(0 == inGroup && tokens[i].type == TK_COMMA) {
			commaFound = true;
		}
		if(0 == inGroup && tokens[i].type == TK_PIPE) {
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
	size_t inGroup = 0;
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
				inGroup++;
				continue;
			case TK_RPAREN:
			case TK_RBRACE:
			case TK_RSQUARE:
				inGroup--;
				continue;
			default:
				break;
		}
		if(0 == inGroup && tokens[i].type == delim) {
			n_children++;
		}
	}

	return n_children;
}

size_t getDelimOffset(GRAMMAR_TYPE type, Token *tokens) {
	size_t inGroup = 0;
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
		switch(tokens[ret].type) {
			case TK_LPAREN:
			case TK_LBRACE:
			case TK_LSQUARE:
				inGroup++;
				ret++;
				continue;
			case TK_RPAREN:
			case TK_RBRACE:
			case TK_RSQUARE:
				inGroup--;
				ret++;
				continue;
			default:
				break;
		}
		if(0 == inGroup && tokens[ret].type == delim) {
			return ret;
		}
		ret++;
	}
	return 0;
}

int fillGrammarNode(RuleNode *node, Token *tokens, size_t n, MemPool *pool) {
	initGrammarNode(node);
	RULE_NODE_TYPE nodeType = getPrevalentType(tokens, n);
	node->node_type = nodeType;

	switch(nodeType) {
		case RULE_GRM: {
			GRAMMAR_TYPE grammarType = getPrevalentGrammarType(tokens, n);
			switch(grammarType) {
				case GRM_AND:
				case GRM_OR: {
					node->nested_type.g = grammarType;
					node->n_children = countChildren(grammarType, tokens, n);
					node->children = palloc(pool, node->n_children * sizeof(RuleNode));

					Token *start = tokens;
					size_t delimOffset = 0;
					for(size_t i = 0; i < node->n_children - 1; i++) {
						delimOffset = getDelimOffset(grammarType, start);
						fillGrammarNode(&(node->children[i]), start, delimOffset, pool);
						node->children[i].parent = node;
						start = start + delimOffset + 1;
					}
					fillGrammarNode(&(node->children[node->n_children - 1]), start, n - (size_t)(start - tokens), pool);
					break;
				}
				case GRM_GROUP:
				case GRM_IFONE:
				case GRM_IFMANY: {
					node->nested_type.g = grammarType;
					node->n_children = 1;					
					node->children = palloc(pool, sizeof(RuleNode));
					node->children->parent = node;
					fillGrammarNode(node->children, tokens + 1, n - 2, pool);
					break;
				}
				default: 
					break;
			}
			break;
		}
		case RULE_STX: {
			node->nested_type.s = getSNodeTypeFromNChars(tokens[0].start, tokens[0].len);
			break;
		}
		case RULE_TK: {
			node->nested_type.t = tokenTypeValFromNChars(tokens[0].start, tokens[0].len);
			break;
		}
	}
	return 0;
}

int populateGrammarRulePointers(RuleNode *node, GrammarRuleArray *array) {
	switch(node->node_type) {
		case RULE_GRM: {
			switch(node->nested_type.g) {
				case GRM_AND:
				case GRM_OR:
				case GRM_GROUP:
				case GRM_IFONE:
				case GRM_IFMANY:
					for(size_t i = 0; i < node->n_children; i++) {
						populateGrammarRulePointers(&(node->children[i]), array);
					}
					break;
				default:
					break;
			}
		}
		case RULE_STX: {
			node->rule_head = array->rules[node->nested_type.s].head;
			break;
		}
		case RULE_TK:
		default:
			break;
	}
	return 0;
}

int populateGrammarRuleArrayPointers(GrammarRuleArray *ruleArray) {
	for(size_t i = 0; i < ruleArray->n_rules; i++) {
		populateGrammarRulePointers(ruleArray->rules[i].head, ruleArray);
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

	ruleArray->n_rules = STX_ERR - STX_SCOPE;
	ruleArray->rules = palloc(pool, ruleArray->n_rules * sizeof(GrammarRule));

	for(size_t i = 0; i < ruleArray->n_rules; i++) {
		ruleArray->rules[i].stype = i;
		ruleArray->rules[i].head = palloc(pool, sizeof(RuleNode));

		size_t ruleStart = getRuleStartIndex((SYNTAX_TYPE)i, tokens, n_tokens);
		size_t semiOffset = getSemicolonOffset(&tokens[ruleStart]);

		fillGrammarNode(ruleArray->rules[i].head, &tokens[ruleStart], semiOffset, pool);
		ruleArray->rules[i].head->parent = NULL;
	}
	populateGrammarRuleArrayPointers(ruleArray);
	printGrammarRuleArray(ruleArray);

	FILE *rulesLog = checkFileOpen("./debug/grammar_tree.log", "w");
	if(rulesLog) {
		fPrintGrammarRuleArray(ruleArray, rulesLog);
	}
	fclose(rulesLog);
	return 0;
}

char *syntaxTypeLiteralLookup(SYNTAX_TYPE type) {
	return __syntaxTypeLiterals[type];
}

SYNTAX_TYPE getSNodeTypeFromLiteral(char *str) {
	for (size_t i = 0; i < (sizeof(__syntaxTypeLiterals) / sizeof(char *)); i++) {
		if(0 == strcmp(str, __syntaxTypeLiterals[i])) {
			return i;
		}
	}
	return STX_ERR;
}

SYNTAX_TYPE getSNodeTypeFromNChars(char *str, size_t n) {
	for (size_t i = 0; i < (sizeof(__syntaxTypeLiterals) / sizeof(char *)); i++) {
		if(0 == strncmp(str, __syntaxTypeLiterals[i], n)) {
			return i;
		}
	}
	return STX_ERR;
}

void printGrammarNode(RuleNode *node, unsigned int indent) {
	switch(node->node_type) {
		case RULE_GRM: {
			__printNTabs(indent);
			char *label = NULL;
			switch(node->nested_type.g) {
				case GRM_AND:
					label = "AND"; break;
				case GRM_OR:
					label = "OR"; break;
				case GRM_GROUP:
					label = "GROUP"; break;
				case GRM_IFONE:
					label = "IFONE"; break;
				case GRM_IFMANY:
					label = "IFMANY"; break;
			}
			printf("%s\n", label);
			for(size_t i = 0; i < node->n_children; i++) {
				printGrammarNode(&(node->children[i]), indent + 1);
			}
			break;
		}
		case RULE_STX: {
			__printNTabs(indent);
			printf("SYNTAX --- %s (%p)\n", 
				syntaxTypeLiteralLookup(node->nested_type.s),
				(void *)node->rule_head);
			break;
		}
		case RULE_TK: {
			__printNTabs(indent);
			printf("TOKEN --- %s\n", tokenTypeLiteralLookup(node->nested_type.t));
			break;
		}
	}
	return;
}

void printGrammarRule(GrammarRule *rule) {
	printf("RULE: %s\n", syntaxTypeLiteralLookup(rule->stype));
	printf("-\n");
	printGrammarNode(rule->head, 1);
	putchar('\n');
	printf("----\n");
	printf("----\n\n");
	return;
}

void printGrammarRuleByIndex(GrammarRuleArray *array, int i) {
	printGrammarRule(&(array->rules[i]));
	return;
}

void printGrammarRuleArray(GrammarRuleArray *array) {
	for(size_t i = 0; i < array->n_rules; i++) {
		printGrammarRule(&(array->rules[i]));
	}
	return;
}

void fPrintGrammarNode(RuleNode *node, unsigned int indent, FILE *file) {
	switch(node->node_type) {
		case RULE_GRM: {
			__fPrintNTabs(indent, file);
			char *label = NULL;
			switch(node->nested_type.g) {
				case GRM_AND:
					label = "AND"; break;
				case GRM_OR:
					label = "OR"; break;
				case GRM_GROUP:
					label = "GROUP"; break;
				case GRM_IFONE:
					label = "IFONE"; break;
				case GRM_IFMANY:
					label = "IFMANY"; break;
			}
			fprintf(file, "%s\n", label);
			for(size_t i = 0; i < node->n_children; i++) {
				fPrintGrammarNode(&(node->children[i]), indent + 1, file);
			}
			break;
		}
		case RULE_STX: {
			__fPrintNTabs(indent, file);
			fprintf(file, "SYNTAX --- %s\n", syntaxTypeLiteralLookup(node->nested_type.s));
			break;
		}
		case RULE_TK: {
			__fPrintNTabs(indent, file);
			fprintf(file, "TOKEN --- %s\n", tokenTypeLiteralLookup(node->nested_type.t));
			break;
		}
	}
	return;
}

void fPrintGrammarRule(GrammarRule *rule, FILE *file) {
	fprintf(file, "RULE: %s\n", syntaxTypeLiteralLookup(rule->stype));
	fprintf(file, "-\n");
	fPrintGrammarNode(rule->head, 1, file);
	fputc('\n', file);
	fprintf(file, "----\n");
	fprintf(file, "----\n\n");
	return;
}

void fPrintGrammarRuleByIndex(GrammarRuleArray *array, int i, FILE *file) {
	fPrintGrammarRule(&(array->rules[i]), file);
	return;
}
void fPrintGrammarRuleArray(GrammarRuleArray *array, FILE *file) {
	for(size_t i = 0; i < array->n_rules; i++) {
		fPrintGrammarRule(&(array->rules[i]), file);
	}
	return;
}
