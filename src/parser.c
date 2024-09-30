#include "parser.h"
#include "mempool.h"

void initParser(Parser *parser) {
	parser->source = NULL;

	parser->n_tokens = 0;
	parser->tokens = NULL;

	parser->head = NULL;
	initMemPool(&(parser->pool));
	return;
}

void resetParser(Parser *parser) {
	parser->source = NULL;

	parser->n_tokens = 0;
	parser->tokens = NULL;
	
	parser->head = NULL;
	resetMemPool(&(parser->pool));
}

void termParser(Parser *parser) {
	termMemPool(&(parser->pool));
	return;
}