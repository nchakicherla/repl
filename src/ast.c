#include "common.h"
#include "scanner.h"
#include "ast.h"
#include "file.h"
#include "vm.h"

#include <stdio.h>

char *__syntaxTypeLiterals[] = {

	"STX_SCOPE", "STX_CASE", "STX_CLASS", "STX_FNDEF", "STX_IF", "STX_WHILE",
	"STX_FOR", "STX_FORRANGE", "STX_SWITCH",

	"STX_DECLARE", "STX_ASSIGN", "STX_INIT", "STX_ECHO", "STX_BREAK", 
	"STX_RETURN", "STX_EXIT",

	"STX_GEXPR", "STX_EXPR",

	"STX_VTYPE", "STX_VAR", "STX_MEMBER", "STX_THIS", "STX_INDEX", "STX_FNCALL",
	"STX_NUM", "STX_STRLIT", "STX_NIL",

	"STX_ARITHOP", "STX_BOOLOP", "STX_ASSIGNOP",

	"STX_MULT", "STX_DIV", "STX_SUM", "STX_DIFF", "STX_MOD",

	"STX_TRUE", "STX_FALSE", "STX_AND", "STX_OR", "STX_NOT", "STX_GREATER", 
	"STX_LESS", "STX_EQUAL_EQUAL", "STX_NOT_EQUAL", "STX_GREATER_EQUAL", "STX_LESS_EQUAL",

	"STX_RARROW",

	"STX_EQUAL", "STX_PLUS_EQUAL", "STX_MINUS_EQUAL", "STX_STAR_EQUAL",
	"STX_DIV_EQUAL", "STX_MOD_EQUAL",

	"STX_INCREMENT", "STX_DECREMENT",

	"STX_ERR", "STX_UNSPECIFIED",
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

void __swapSyntaxNodes(SyntaxNode *node1, SyntaxNode *node2) {
	SyntaxNode tmp;
	memcpy(&tmp, node1, sizeof(SyntaxNode));
	memcpy(node1, node2, sizeof(SyntaxNode));
	memcpy(node2, &tmp, sizeof(SyntaxNode));
}

void initSyntaxNode(SyntaxNode *node) {
	node->type = STX_UNSPECIFIED;
	node->is_token = false;
	node->object = NULL;
	node->n_children = 0;
	node->children = NULL;
	node->parent = NULL;
	node->msg = NULL;
}

void initRuleNode(RuleNode *node) {
	node->rule_head = NULL;
	node->n_children = 0;
	node->children = NULL;
	node->parent = NULL;
}

void initTokenStream(TokenStream *stream, Chunk *chunk) {
	stream->tk = chunk->tokens;
	stream->pos = 0;
	stream->n = chunk->n_tokens;
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
	size_t len = strlen(typeLiteral);
	for(size_t i = 0; i < n; i++) {
		if(tokens[i].len == len) {
			if(0 == strncmp(typeLiteral, tokens[i].start, len) && tokens[i + 1].type == TK_EQUAL) {
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
	initRuleNode(node);
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
			char *tmp_str = palloc(pool, tokens[0].len + 1);
			for(size_t i = 0; i < tokens[0].len; i++) {
				tmp_str[i] = tokens[0].start[i];
			}
			tmp_str[tokens[0].len] = '\0';
			//node->nested_type.s = getSNodeTypeFromNChars(tokens[0].start, tokens[0].len);
			node->nested_type.s = getSNodeTypeFromLiteral(tmp_str);
			break;
		}
		case RULE_TK: {
			node->nested_type.t = tokenTypeValFromNChars(tokens[0].start, tokens[0].len);
			break;
		}
	}
	return 0;
}

int populateGrammarRulePointers(RuleNode *node, GrammarRuleArray *rule_array) {
	switch(node->node_type) {
		case RULE_GRM: {
			switch(node->nested_type.g) {
				case GRM_AND:
				case GRM_OR:
				case GRM_GROUP:
				case GRM_IFONE:
				case GRM_IFMANY:
					for(size_t i = 0; i < node->n_children; i++) {
						populateGrammarRulePointers(&(node->children[i]), rule_array);
					}
					break;
				default:
					break;
			}
		}
		case RULE_STX: {
			node->rule_head = rule_array->rules[node->nested_type.s].head;
			break;
		}
		case RULE_TK:
		default:
			break;
	}
	return 0;
}

int populateGrammarRuleArrayReferences(GrammarRuleArray *rule_array) {
	for(size_t i = 0; i < rule_array->n_rules; i++) {
		populateGrammarRulePointers(rule_array->rules[i].head, rule_array);
	}
	return 0;
}

int initGrammarRuleArray(GrammarRuleArray *rule_array, char *fileName, MemPool *pool) {

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

	rule_array->n_rules = STX_ERR - STX_SCOPE;
	rule_array->rules = palloc(pool, rule_array->n_rules * sizeof(GrammarRule));

	for(size_t i = 0; i < rule_array->n_rules; i++) {
		rule_array->rules[i].stype = i;
		rule_array->rules[i].head = palloc(pool, sizeof(RuleNode));

		size_t ruleStart = getRuleStartIndex((SYNTAX_TYPE)i, tokens, n_tokens);
		size_t semiOffset = getSemicolonOffset(&tokens[ruleStart]);

		fillGrammarNode(rule_array->rules[i].head, &tokens[ruleStart], semiOffset, pool);
		rule_array->rules[i].head->parent = NULL;
	}
	populateGrammarRuleArrayReferences(rule_array);
	//printGrammarRuleArray(rule_array);

	FILE *rulesLog = checkFileOpen("./debug/grammar_tree.log", "w");
	if(rulesLog) {
		fPrintGrammarRuleArray(rule_array, rulesLog);
	}
	fclose(rulesLog);
	return 0;
}
/*
size_t matchGrammar(RuleNode *rnode, Token *tokens, SyntaxNode *snode, MemPool *pool) {

	if(tokens[0].type == TK_EOF) {
		printf("at EOF\n");
		return IDX_ERR;
	}

	size_t n_children = rnode->n_children;
	initSyntaxNode(snode);
	size_t ret = 0;

	switch(rnode->node_type) {
		case RULE_GRM: {
			printf("is GRM\n");
			//printGrammarNode(rnode, 0);
			SyntaxNode *tempNodes = palloc(pool, n_children * sizeof(SyntaxNode));
			//bool *successes = palloc(pool, rnode->n_children * sizeof(bool));
			size_t *returns = palloc(pool, n_children * sizeof(size_t));
			memset(returns, '\0', n_children * sizeof(size_t));
			Token *current = tokens;

			switch(rnode->nested_type.g) {
				case GRM_AND: {
					printf("is AND\n");
					for(size_t i = 0; i < n_children; i++) {
						printf("i: %zu\n", i);
						//printGrammarNode(&rnode->children[i], 0);
						returns[i] = matchGrammar(&rnode->children[i], current, &tempNodes[i], pool);
						if(returns[i] == 0) {
							if (GRM_IFONE == rnode->children[i].nested_type.g ||
								GRM_IFMANY == rnode->children[i].nested_type.g) {

								continue;
							}	
							return ret;
						} else if (returns[i] == IDX_ERR) {
							return IDX_ERR;
						}
						current += returns[i];
					}
					for(size_t i = 0; i < n_children; i++) {
						ret += returns[i];
					}
					snode->n_children = n_children;
					snode->children = tempNodes;
					break;
				}
				case GRM_OR: {
					printf("is OR\n");
					size_t success = IDX_ERR;
					for(size_t i = 0; i < n_children; i++) {
						returns[i] = matchGrammar(&rnode->children[i], current, &tempNodes[i], pool);
						if(returns[i] != 0 && returns[i] != IDX_ERR) {
							success = i;
							break;
						}
					}
					memcpy(snode, &tempNodes[success], sizeof(SyntaxNode));
					ret += returns[success];
					break;
				}
				case GRM_GROUP: {
					printf("is GROUP\n");
					size_t num_skips = 0;
					for(size_t i = 0; i < n_children; i++) {
						returns[i] = matchGrammar(&rnode->children[i], current, &tempNodes[i], pool);
						if(returns[i] == 0) {
							if (GRM_IFONE == rnode->children[i].nested_type.g ||
								GRM_IFMANY == rnode->children[i].nested_type.g) {

								num_skips++;
								continue;
							}
							return ret;
						} else if(returns[i] == IDX_ERR) {
							return IDX_ERR;
						}
					}
					for(size_t i = 0; i < n_children; i++) {
						ret += returns[i];
					}
					for(size_t i = 0; i < n_children - num_skips; i++) {
						if(returns[i] == 0) {
							for(size_t j = i + 1; j < n_children; j++) {
								if(returns[j] != 0) {
									__swapSyntaxNodes(&tempNodes[i], &tempNodes[j]);
									break;
								}
							}
						}
					}
					snode->n_children = n_children - num_skips;
					snode->children = tempNodes;
					break;
				}
				case GRM_IFONE: {
					printf("is IFONE\n");
					for(size_t i = 0; i < n_children; i++) {
						returns[i] = matchGrammar(&rnode->children[i], current, &tempNodes[i], pool);
						if(returns[i] > 0 && returns[i] < IDX_ERR) {
							current += returns[i];
						}
					}
					for(size_t i = 0; i < n_children; i++) {
						if(returns[i] == 0) {
							ret = 0;
							break;
						}
						ret += returns[i];
					}
					break;
				}
				case GRM_IFMANY: {
					printf("is IFMANY\n");
					size_t total_ret = 0;
					size_t repetitions = 0;
					size_t tempNodesIndex = 0;
					size_t currentTempNodesSize = n_children * sizeof(SyntaxNode);
					bool failedRepetition = false;
					while (true) {
						printf("in while true\n");
						size_t current_repeat_ret = 0;
						for(size_t i = 0; i < n_children; i++) {

							returns[tempNodesIndex] = matchGrammar(&rnode->children[i], current, 
												&tempNodes[tempNodesIndex], pool);
							tempNodesIndex++;
							current += returns[tempNodesIndex];
						}
						for(size_t i = 0; i < n_children; i++) {
							if(returns[i] == 0 || returns[i] == IDX_ERR) {
								ret = total_ret;
								failedRepetition = true;
								break;
							}
							current_repeat_ret += returns[i];
						}
						if(failedRepetition) {
							break;
						}
						printf("successful repetition\n");
						total_ret += current_repeat_ret;
						current += current_repeat_ret;
						repetitions++;
						pGrowAlloc(tempNodes, currentTempNodesSize, currentTempNodesSize * 2, pool);
						currentTempNodesSize *= 2;
					}
					printf("here3\n");
					if(repetitions > 0) {
						printf("%zu repetitions\n", repetitions);
						snode->n_children = n_children * repetitions;
						snode->children = tempNodes;
						ret = total_ret;
					}
					break;
				}
				default:
					break;
			}
			break;
		}
		case RULE_STX: {
			printf("is STX\n");
			//printGrammarNode(rnode, 0);
			ret = matchGrammar(rnode->rule_head, tokens, snode, pool);
			if(ret > 0 && ret < IDX_ERR) {
				printf("matched syntax %s\n", syntaxTypeLiteralLookup(rnode->nested_type.s));
				snode->type = rnode->nested_type.s;
			}
			break;
		}
		case RULE_TK: {
			printf("is TK\n");
			if(rnode->nested_type.t == tokens[0].type) {
				snode->is_token = true;
				memcpy(&snode->token, tokens, sizeof(Token));
				ret += 1;
				printf("matched token %.*s, ret = %zu\n", (int)tokens[0].len, tokens[0].start, ret);
			}
			break;
		}
		default:
			break;
	}
	return ret;
}
*/

// rule which resolves to token means is_token node is created. nodes are returned upwards without type until calling fn from STX grammar node attaches type

void __addChild(SyntaxNode *parent, SyntaxNode *child, MemPool *pool) {
	//printf("in addchild\n");
	if(parent->n_children == 0) {
		parent->children = palloc(pool, sizeof(SyntaxNode *));
	} else {
		SyntaxNode **new_array = pGrowAlloc(parent->children, parent->n_children * sizeof(SyntaxNode *), (parent->n_children + 1) * sizeof(SyntaxNode *), pool);
		parent->children = new_array;
	}
	parent->children[parent->n_children] = child;
	parent->n_children++;
	//printf("added child\n");
	//printf("n_children: %zu\n", parent->n_children);
}
//
//

SyntaxNode *__wrapChild(SyntaxNode *child, SYNTAX_TYPE stype, MemPool *pool) {
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

	//printf("rnode->n_children: %zu\n", rnode->n_children);
	for(size_t i = 0; i < rnode->n_children; i++) {
		//printf("in parseAnd: %zu\n", i);
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
		/*if((child->is_token == false && rnode->children[i].node_type == RULE_GRM) && 
			(	rnode->children[i].nested_type.g == GRM_GROUP ||
				rnode->children[i].nested_type.g == GRM_IFONE ||
				rnode->children[i].nested_type.g == GRM_IFMANY)	
		)*/

		if(child->type == STX_UNSPECIFIED && child->is_token == false)

		{
			if(rnode->children[i].nested_type.g == GRM_IFONE) {
				if((rnode->children[i].children[0].node_type == RULE_GRM &&
					rnode->children[i].children[0].nested_type.g == GRM_OR)

					|| rnode->children[i].children[0].node_type == RULE_STX
					/*|| (rnode->children[i].children[0].node_type == RULE_STX)*/) {
					__addChild(node, child, pool);
					continue;
				}
			}
			for(size_t j = 0; j < child->n_children; j++) {
				__addChild(node, child->children[j], pool);
			}
		} else {
			__addChild(node, child, pool);
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
					__addChild(node, child->children[i], pool);
				}
			} else {
				__addChild(node, child, pool);
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
			SyntaxNode *wrap = __wrapChild(node, rnode->nested_type.s, pool);
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
	//printf("in parseGrammar\n");
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
					//return parseAnd(rnode->children, stream, pool);
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

//
//
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

void printTokenStream(TokenStream *stream) {
	for(size_t i = 0; i < stream->n; i++) {
		printf("LINE: %6zu TK%6zu: TYPE: %16s - \"%.*s\"\n", 
			stream->tk[i].line,
			i,
			tokenTypeLiteralLookup(stream->tk[i].type), 
			(int)stream->tk[i].len, stream->tk[i].start);
	}
}

void printSyntaxNode(SyntaxNode *node, unsigned int indent) {
	if(node->is_token == true) {
		__printNTabs(indent);
		printf("%s %.*s\n", tokenTypeLiteralLookup(node->token.type), (int)node->token.len, node->token.start);
	} else {
		__printNTabs(indent);
		printf("/%s\n", syntaxTypeLiteralLookup(node->type));
		for(size_t i = 0; i < node->n_children; i++) {
			printSyntaxNode(node->children[i], indent + 1);
		}
	}
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
}

void printGrammarRule(GrammarRule *rule) {
	printf("RULE: %s\n", syntaxTypeLiteralLookup(rule->stype));
	printf("-\n");
	printGrammarNode(rule->head, 1);
	printf("\n----\n----\n\n");
}

void printGrammarRuleByIndex(GrammarRuleArray *array, int i) {
	printGrammarRule(&(array->rules[i]));
}

void printGrammarRuleArray(GrammarRuleArray *array) {
	for(size_t i = 0; i < array->n_rules; i++) {
		printGrammarRule(&(array->rules[i]));
	}
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
}

void fPrintGrammarRule(GrammarRule *rule, FILE *file) {
	fprintf(file, "RULE: %s\n", syntaxTypeLiteralLookup(rule->stype));
	fprintf(file, "-\n");
	fPrintGrammarNode(rule->head, 1, file);
	fprintf(file, "\n----\n----\n\n");

}

void fPrintGrammarRuleByIndex(GrammarRuleArray *array, int i, FILE *file) {
	fPrintGrammarRule(&(array->rules[i]), file);
}
void fPrintGrammarRuleArray(GrammarRuleArray *array, FILE *file) {
	for(size_t i = 0; i < array->n_rules; i++) {
		fPrintGrammarRule(&(array->rules[i]), file);
	}
}
