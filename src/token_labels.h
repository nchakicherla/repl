#ifndef TOKEN_LABELS_H
#define TOKEN_LABELS_H

static char *__tokenNameLiterals[] = {
	"TK_LPAREN", "TK_RPAREN", "TK_LBRACE", "TK_RBRACE", "TK_LSQUARE", "TK_RSQUARE",
	"TK_COMMA", "TK_DOT", "TK_HASH", "TK_MINUS", "TK_PLUS", "TK_MOD", "TK_SEMICOLON", 
	"TK_FSLASH", "TK_STAR", "TK_COLON", "TK_PIPE",

	// 1- or 2-character
	"TK_BANG", "TK_BANG_EQUAL",
	"TK_EQUAL", "TK_EQUAL_EQUAL",
	"TK_GREATER", "TK_GREATER_EQUAL",
	"TK_LESS", "TK_LESS_EQUAL",

	// literals
	"TK_IDENTIFIER", "TK_CHARS", "TK_NUM",

	// keywords
	"TK_AND", "TK_BOOL", "TK_BREAK", "TK_CASE", "TK_CLASS", "TK_CONST",
	"TK_FLT", "TK_ELSE", "TK_EXIT", "TK_FALSE", "TK_FN", "TK_FOR", "TK_IF",
	"TK_IN", 

	"TK_I64", "TK_I32", "TK_I16", "TK_I8", "TK_U64", "TK_U32", "TK_U16", "TK_U8",

	"TK_NIL", "TK_OR", "TK_RETURN", "TK_SCOPE", "TK_STR", 
	"TK_SWITCH", "TK_THIS", "TK_TRUE", "TK_WHILE",

	"TK_EOF", "TK_ERROR"
};

#endif // TOKEN_LABELS_H