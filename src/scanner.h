#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	// 1-character
	TK_LPAREN, TK_RPAREN, TK_LBRACE, TK_RBRACE, TK_LSQUARE, TK_RSQUARE,
	TK_COMMA, TK_DOT, TK_HASH, TK_MINUS, TK_PLUS, TK_MOD, TK_SEMICOLON, 
	TK_FSLASH, TK_STAR, TK_COLON, TK_PIPE,

	// 1- or 2-character
	TK_BANG, TK_BANG_EQUAL,
	TK_EQUAL, TK_EQUAL_EQUAL,
	TK_GREATER, TK_GREATER_EQUAL,
	TK_LESS, TK_LESS_EQUAL,

	// literals
	TK_IDENTIFIER, TK_CHARS, TK_NUM,

	// keywords
	TK_AND, TK_BOOL, TK_BREAK, TK_CLASS, TK_FLT, TK_ELSE, TK_EXIT, TK_FALSE, TK_FN, TK_FOR, TK_IF, TK_INT, TK_NIL, 
	TK_OR, TK_RETURN, TK_SCOPE, TK_STR, TK_THIS, TK_TRUE, TK_WHILE,

	TK_EOF, TK_ERROR,
} TOKEN_TYPE;

typedef struct s_Scanner {
	char *start;
	char *current;
	size_t line;
} Scanner;

typedef struct s_Token {
	TOKEN_TYPE type;
	char *start;
	size_t len;
	size_t line;
} Token;

void initScanner(char *source);

bool isAtEnd(void);

char advance(void);

char peek(void);

char peekNext(void);

bool match(char expect);

Token makeToken(TOKEN_TYPE type);

Token makeErrorToken(char *message);

void skipWhitespace(void);

Token string(void);

bool isDigit(char c);

Token number(void);

bool isAlpha(char c);

TOKEN_TYPE checkKeyword(size_t start, size_t len, char *rest, TOKEN_TYPE type);

TOKEN_TYPE identifierType(void);

Token identifier(void);

Token scanToken(void);

char *tokenTypeLiteralLookup(TOKEN_TYPE type);

TOKEN_TYPE getTKTypeFromLiteral(char *str);

TOKEN_TYPE tokenTypeValFromNChars(char* str, size_t n);

void __printToken(Token *token);

#endif // SCANNER_H
