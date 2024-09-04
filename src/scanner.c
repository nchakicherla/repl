#include "chunk.h"
#include "scanner.h"

#include <stdio.h>

char *__tk_literals[] = {
	"TK_LPAREN", "TK_RPAREN", "TK_LBRACE", "TK_RBRACE", "TK_LSQUARE", "TK_RSQUARE",
	"TK_COMMA", "TK_DOT", "TK_MINUS", "TK_PLUS", "TK_MOD", "TK_SEMICOLON", "TK_FSLASH", "TK_STAR", "TK_COLON",

	// 1- or 2-character
	"TK_BANG", "TK_BANG_EQUAL",
	"TK_EQUAL", "TK_EQUAL_EQUAL",
	"TK_GREATER", "TK_GREATER_EQUAL",
	"TK_LESS", "TK_LESS_EQUAL",

	// literals
	"TK_IDENTIFIER", "TK_STRING", "TK_NUM",

	// keywords
	"TK_AND", "TK_BOOL", "TK_BREAK", "TK_CLASS", "TK_FLT", "TK_ELSE", "TK_EXIT", "TK_FALSE", "TK_FN", "TK_FOR", "TK_IF", "TK_INT", "TK_NIL", 
	"TK_OR", "TK_RETURN", "TK_STR", "TK_THIS", "TK_TRUE", "TK_WHILE",

	"TK_ERROR", "TK_EOF"
};

Scanner scanner;

void initScanner(char *source) {
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;
}

bool isAtEnd(void) {
	return *scanner.current == '\0';
}

char advance(void) {
	scanner.current++;
	return scanner.current[-1];
}

char peek(void) {
	return *scanner.current;
}

char peekNext(void) {
	if (isAtEnd()) {
		return '\0';
	}
	return scanner.current[1];
}

bool match(char expect) {
	if (isAtEnd())  {
		return false;
	}
	if (*scanner.current != expect)  {
		return false;
	}
	
	scanner.current++;
	return true;
}

Token makeToken(TOKEN_TYPE type) {
	Token token;
	token.type = type;
	token.start = scanner.start;
	token.len = (size_t)(scanner.current - scanner.start);
	token.line = scanner.line;
	return token;
}

Token makeTokenWPool(TOKEN_TYPE type) {
	Token token;
	token.type = type;
	token.start = scanner.start;
	token.len = (size_t)(scanner.current - scanner.start);
	token.line = scanner.line;
	return token;
}

Token makeErrorToken(char *message) {
	Token token;
	token.type = TK_ERROR;
	token.start = message;
	token.len = strlen(message);
	token.line = scanner.line;
	return token;
}

void skipWhitespace(void) {
	while (true) {
		char c = peek();
		switch(c) {
			case ' ':
			case '\r':
			case '\t':
				advance();
				break;
			case '\n':
				scanner.line++;
				advance();
				break;
			case '/':
				if (peekNext() == '/') {
					while (peek() != '\n' && !isAtEnd()) {
						advance();
					}
				} else {
					return;
				}
				break;
			default:
				return;
		}
	}
}

Token string(void) {
	while (peek() != '"' && !isAtEnd()) {
		if (peek() == '\n') { // strings can be multi-line
			// scanner.line++;
			return makeErrorToken("unterminated string");
		}
		advance();
	}

	if (isAtEnd()) {
		return makeErrorToken("unterminated string");
	}

	advance();
	return makeToken(TK_STRING);
}

bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

Token number(void) {
	while (isDigit(peek())) {
		advance();
	}

	if (peek() == '.' && isDigit(peekNext())) {
		advance();

		while (isDigit(peek())) {
			advance();
		}
	}

	return makeToken(TK_NUM);
}

bool isAlpha(char c) {
	return (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		c == '_';
}

TOKEN_TYPE checkKeyword(size_t start, size_t len, char *rest, TOKEN_TYPE type) {
	if (scanner.current - scanner.start == start + len &&
		memcmp(scanner.start + start, rest, len) == 0) {
		return type;
	}

	return TK_IDENTIFIER;
}

TOKEN_TYPE identifierType(void) {

	switch (scanner.start[0]) {
		case 'a': return checkKeyword(1, 2, "nd", TK_AND);
		// case 'b': return checkKeyword(1, 3, "ool", TK_BOOL);
		case 'b':
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'o': return checkKeyword(2, 2, "ol", TK_BOOL);
					case 'r': return checkKeyword(2, 3, "eak", TK_BREAK);
				}
			}
		case 'c': return checkKeyword(1, 4, "lass", TK_CLASS);
		// case 'e': return checkKeyword(1, 3, "lse", TK_ELSE);
		case 'e':
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'l': return checkKeyword(2, 2, "se", TK_ELSE);
					case 'x': return checkKeyword(2, 2, "it", TK_EXIT); 
				}
			}
		case 'f': {
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'a': return checkKeyword(2, 3, "lse", TK_FALSE);
					case 'l': return checkKeyword(2, 1, "t", TK_FLT);
					case 'n': return TK_FN;
					case 'o': return checkKeyword(2, 1, "r", TK_FOR);
				}
			}
		}
		case 'i': {
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'f': return TK_IF;
					case 'n': return checkKeyword(2, 1, "t", TK_INT);
				}
			}
		}
		case 'n': return checkKeyword(1, 2, "il", TK_NIL);
		case 'o': return checkKeyword(1, 2, "r", TK_OR);
		//case 'p': return checkKeyword(1, 2, "rint", TK_PRINT);
		case 'r': return checkKeyword(1, 2, "eturn", TK_RETURN);
		case 's': return checkKeyword(1, 2, "tr", TK_STR);
		case 't': {
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'h': return checkKeyword(2, 2, "is", TK_THIS);
					case 'r': return checkKeyword(2, 2, "ue", TK_TRUE);
				}
			}
		}
		//case 'v': return checkKeyword(1, 2, "ar", TK_VAR);
		case 'w': return checkKeyword(1, 2, "hile", TK_AND);
	}
	return TK_IDENTIFIER;
}

Token identifier(void) {
	while (isAlpha(peek()) || isDigit(peek())) {
		advance();
	}
	return makeToken(identifierType());
}

Token scanToken(void) {

	skipWhitespace();

	scanner.start = scanner.current;

	if (isAtEnd()) return makeToken(TK_EOF);

	char c = advance();

	if (isAlpha(c)) {
		return identifier();
	}
	if (isDigit(c)) {
		return number();
	}

	switch(c) {
		case '(': return makeToken(TK_LPAREN);
		case ')': return makeToken(TK_RPAREN);
		case '{': return makeToken(TK_LBRACE);
		case '}': return makeToken(TK_RBRACE);
		case '[': return makeToken(TK_LSQUARE);
		case ']': return makeToken(TK_RSQUARE);
		case ',': return makeToken(TK_COMMA);
		case '.': return makeToken(TK_DOT);
		case '-': return makeToken(TK_MINUS);
		case '+': return makeToken(TK_PLUS);
		case ';': return makeToken(TK_SEMICOLON);
		case '/': return makeToken(TK_FSLASH);
		case '*': return makeToken(TK_STAR);
		case '%': return makeToken(TK_MOD);
		case ':': return makeToken(TK_COLON);

		case '!':
			return makeToken (
				match('=') ? TK_BANG_EQUAL : TK_BANG);
		case '=':
			return makeToken (
				match('=') ? TK_EQUAL_EQUAL : TK_EQUAL);
		case '<':
			return makeToken (
				match('=') ? TK_LESS_EQUAL : TK_LESS);
		case '>':
			return makeToken (
				match('=') ? TK_GREATER_EQUAL : TK_GREATER);
		case '"': 
			return string();
	}
	return makeErrorToken("unexpected character");
}

char *getTKTypeLiteral(TOKEN_TYPE type) {
	return __tk_literals[type];
}
