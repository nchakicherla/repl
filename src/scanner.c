#include "scanner.h"

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
			scanner.line++;
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
		case 'c': return checkKeyword(1, 4, "lass", TK_CLASS);
		case 'e': return checkKeyword(1, 3, "lse", TK_ELSE);
		case 'f': {
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'a': return checkKeyword(2, 3, "lse", TK_FALSE);
					case 'n': return TK_FN;
					case 'o': return checkKeyword(2, 1, "r", TK_FOR);
				}
			}
		}
		case 'i': return checkKeyword(1, 1, "f", TK_IF);
		case 'n': return checkKeyword(1, 2, "il", TK_NIL);
		case 'o': return checkKeyword(1, 2, "r", TK_OR);
		//case 'p': return checkKeyword(1, 2, "rint", TK_PRINT);
		case 'r': return checkKeyword(1, 2, "eturn", TK_RETURN);
		//case 's': return checkKeyword(1, 2, "uper", TK_SUPER);
		case 't': {
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'h': return checkKeyword(2, 2, "is", TK_THIS);
					case 'r': return checkKeyword(2, 2, "ue", TK_TRUE);
				}
			}
		}
		case 'v': return checkKeyword(1, 2, "ar", TK_AND);
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
		case ':': return makeToken(TK_COLON);

		case '1':
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
