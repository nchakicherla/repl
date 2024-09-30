#include "common.h"
#include "scanner.h"
#include "ast.h"
#include "file.h"
#include "vm.h"
#include "color.h"

#include "syntax_labels.h"

#include <stdio.h>

void __swapSyntaxNodes(SyntaxNode *node1, SyntaxNode *node2) {
	SyntaxNode tmp;
	memcpy(&tmp, node1, sizeof(SyntaxNode));
	memcpy(node1, node2, sizeof(SyntaxNode));
	memcpy(node2, &tmp, sizeof(SyntaxNode));
}

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

char *syntaxTypeLiteralLookup(SYNTAX_TYPE type) {
	return __syntaxTypeLiterals[type];
}

/*
static SYNTAX_TYPE getSNodeTypeFromLiteral(char *str) {
	for (size_t i = 0; i < (sizeof(__syntaxTypeLiterals) / sizeof(char *)); i++) {
		if(0 == strcmp(str, __syntaxTypeLiterals[i])) {
			return i;
		}
	}
	return STX_ERR;
}

static SYNTAX_TYPE getSNodeTypeFromNChars(char *str, size_t n) {
	for (size_t i = 0; i < (sizeof(__syntaxTypeLiterals) / sizeof(char *)); i++) {
		if(0 == strncmp(str, __syntaxTypeLiterals[i], n)) {
			return i;
		}
	}
	return STX_ERR;
}
*/
void initTokenStream(TokenStream *stream, SyntaxTree *tree) {
	stream->tk = tree->tokens;
	stream->pos = 0;
	stream->n = tree->n_tokens;
}

int scanTokensFromSource(SyntaxTree *tree, char *source) {

	size_t len = strlen(source);
	tree->source = palloc(&(tree->pool), len + 1);
	memcpy(tree->source, source, len + 1);
	tree->source[len] = '\0';

	initScanner(tree->source);

	size_t n_tokens = 0;
	while(true) {
		Token token = scanToken();
		n_tokens++;
		if (token.type == TK_EOF) break;
	}

	tree->n_tokens = n_tokens;
	tree->tokens = palloc(&(tree->pool), (n_tokens * sizeof(Token)));

	initScanner(tree->source);

	for(size_t i = 0; i < n_tokens; i++) {
		tree->tokens[i] = scanToken();
	}
	return 0;
}

void __printTokens(Token *tokens, size_t n) {
	for(size_t i = 0; i < n; i++) {
		printf("LINE: %6zu TK%6zu: TYPE: %16s - \"%.*s\"\n", 
			tokens[i].line,
			i,
			tokenLabelLookup(tokens[i].type), 
			(int)tokens[i].len, tokens[i].start);
	}
	return;
}

void defineParentPtrs(SyntaxNode *node) {
	for(size_t i = 0; i < node->n_children; i++) {
		node->children[i]->parent = node;
		if(false == node->children[i]->is_token) {
			defineParentPtrs(node->children[i]);
		}
	}
}

void addChild(SyntaxNode *parent, SyntaxNode *child, MemPool *pool) {
	if(parent->n_children == 0) {
		parent->children = palloc(pool, sizeof(SyntaxNode *));
	} else {
		SyntaxNode **new_array = pGrowAlloc(parent->children, parent->n_children * sizeof(SyntaxNode *), (parent->n_children + 1) * sizeof(SyntaxNode *), pool);
		parent->children = new_array;
	}
	parent->children[parent->n_children] = child;
	parent->n_children++;
}

SyntaxNode *wrapNode(SyntaxNode *child, SYNTAX_TYPE stype, MemPool *pool) {
	SyntaxNode *parent = palloc(pool, sizeof(SyntaxNode));
	initSyntaxNode(parent);

	parent->type = stype;
	parent->children = palloc(pool, sizeof(SyntaxNode*));
	parent->children[0] = child;
	parent->n_children = 1;
	return parent;
}

SyntaxNode *parseAnd(RuleNode *rnode, TokenStream *stream, MemPool *pool) {
	SyntaxNode *node = palloc(pool, sizeof(SyntaxNode));
	initSyntaxNode(node);
	size_t start_pos = stream->pos;

	for(size_t i = 0; i < rnode->n_children; i++) {
		SyntaxNode *child = parseGrammar(&rnode->children[i], stream, pool);
		if(!child) {
			if(rnode->children[i].node_type == RULE_GRM && ( 
				rnode->children[i].nested_type.g == GRM_IFONE ||
				rnode->children[i].nested_type.g == GRM_IFMANY)
			) {
				continue;
			}
			stream->pos = start_pos;
			return NULL;
		}

		if(child->type == STX_UNSPECIFIED && child->is_token == false)

		{
			if(rnode->children[i].nested_type.g == GRM_IFONE) {
				if((rnode->children[i].children[0].node_type == RULE_GRM &&
					rnode->children[i].children[0].nested_type.g == GRM_OR)

					|| rnode->children[i].children[0].node_type == RULE_STX) {
					addChild(node, child, pool);
					continue;
				}
			}
			for(size_t j = 0; j < child->n_children; j++) {
				addChild(node, child->children[j], pool);
			}
		} else {
			addChild(node, child, pool);
		}
	}
	return node;
}

SyntaxNode *parseOr(RuleNode *rnode, TokenStream *stream, MemPool *pool) {
	for(size_t i = 0; i < rnode->n_children; i++) {
		SyntaxNode *node = parseGrammar(&rnode->children[i], stream, pool);
		if(node) {
			return node;
		}
	}
	return NULL;
}

SyntaxNode *parseIfOne(RuleNode *rnode, TokenStream *stream, MemPool *pool) {
	return parseGrammar(rnode->children, stream, pool);
}

SyntaxNode *parseIfMany(RuleNode *rnode, TokenStream *stream, MemPool *pool) {
	SyntaxNode *node = palloc(pool, sizeof(SyntaxNode));
	initSyntaxNode(node);
	size_t reset = stream->pos;
	while(1) {
		reset = stream->pos;
		SyntaxNode *child = parseGrammar(rnode->children, stream, pool);
		if(child) {
			if(rnode->children[0].node_type == RULE_GRM && rnode->children[0].nested_type.g == GRM_AND) {
				for(size_t i = 0; i < child->n_children; i++) {
					addChild(node, child->children[i], pool);
				}
			} else {
				addChild(node, child, pool);
			}
		} else {
			stream->pos = reset;
			break;
		}
	}
	return node;
}

SyntaxNode *parseSyntax(RuleNode *rnode, TokenStream *stream, MemPool *pool) {
	SyntaxNode *node = parseGrammar(rnode->rule_head, stream, pool);
	if(node) {
		if(node->is_token == true || (rnode->rule_head->node_type == RULE_GRM && rnode->rule_head->nested_type.g == GRM_OR)) {
			SyntaxNode *wrap = wrapNode(node, rnode->nested_type.s, pool);
			return wrap;
		} else {
			node->type = rnode->nested_type.s;
		}
		//printf("matched syntax: \n");
		//printGrammarNode(rnode->rule_head, 0);
	} else {
		return NULL;
	}
	return node;
}

SyntaxNode *parseToken(RuleNode *rnode, TokenStream *stream, MemPool *pool) {
	if(stream->tk[stream->pos].type == rnode->nested_type.t) {
		SyntaxNode *node = palloc(pool, sizeof(SyntaxNode));
		initSyntaxNode(node);
		node->is_token = true;
		memcpy(&node->token, &stream->tk[stream->pos], sizeof(Token));
		stream->pos++;
		//printf("made tk node: %.*s\n", (int)node->token.len, node->token.start);
		return node;
	} else {
		return NULL;
	}
}

SyntaxNode *parseGrammar(RuleNode* rnode, TokenStream *stream, MemPool *pool) {
	switch(rnode->node_type) {
		case RULE_GRM: {
			switch(rnode->nested_type.g) {
				case GRM_AND: {
					return parseAnd(rnode, stream, pool);
				}
				case GRM_OR: {
					return parseOr(rnode, stream, pool);
				}
				case GRM_GROUP: {
					return parseGrammar(rnode->children, stream, pool);
				}
				case GRM_IFONE: {
					return parseIfOne(rnode, stream, pool);
				}
				case GRM_IFMANY: {
					return parseIfMany(rnode, stream, pool);
				}
				default: {
					printf("GRM_* not recognized...\n");
					return NULL;
				}
			}
		}
		case RULE_STX: {
			return parseSyntax(rnode, stream, pool);	
		}
		case RULE_TK: {
			return parseToken(rnode, stream, pool);
		}
		default:
			printf("RULE_* not recognized...\n");
			break;
	}
	return NULL;
}

void printTokenStream(TokenStream *stream) {
	for(size_t i = 0; i < stream->n; i++) {
		printf("LINE: %6zu TK%6zu: TYPE: %16s - \"%.*s\"\n", 
			stream->tk[i].line,
			i,
			tokenLabelLookup(stream->tk[i].type), 
			(int)stream->tk[i].len, stream->tk[i].start);
	}
}

void printSyntaxNode(SyntaxNode *node, unsigned int indent) {
	if(node->is_token == true) {
		__printNTabs(indent);
		printf("- ");

		setColor(ANSI_GREEN);
		printf("%s ", tokenLabelLookup(node->token.type));
		resetColor();

		setColor(ANSI_CYAN);
		printf("%.*s\n", (int)node->token.len, node->token.start);
		resetColor();
	} else {
		__printNTabs(indent);
		printf("- ");

		setColor(ANSI_YELLOW);
		printf("/%s\n", syntaxTypeLiteralLookup(node->type));
		resetColor();

		for(size_t i = 0; i < node->n_children; i++) {
			printSyntaxNode(node->children[i], indent + 1);
		}
	}
}
