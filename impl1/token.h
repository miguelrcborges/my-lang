#ifndef TOKEN_H
#define TOKEN_H

typedef unsigned char TokenType;
enum _TokenType {
	/* Operators */
	PLUS    = '+',
	MINUS   = '-',
	STAR    = '*',
	SLASH   = '/',
	PERCENT = '%', 
	BANG    = '!',
	EQUAL   = '=',

	/* Comparators */
	GREATER = '>',
	LESS    = '<',

	/* Others */
	COLON       = ':',
	LEFT_PAREN  = '(',
	RIGHT_PAREN = ')',
	LEFT_BRACE  = '{',
	RIGHT_BRACE = '}',


	/* Non Single Characters */
	EQUAL_EQUAL = 128,
	GREATER_EQUAL,
	LESS_EQUAL,
	BANG_EQUAL,
	PLUS_PLUS,
	MINUS_MINUS,


	IDENTIFIER,
	NUMBER,
	STRING,
	TRUE,
	FALSE,

	/* Keywords */
	IMPORT,
	FUN,
	IF,
	ELSE,
	RETURN,

	NEW_SCOPE,
	END_SCOPE,
	EOF_TOK,
};

typedef struct Token Token;
struct Token {
	unsigned short line;
	unsigned short line_pos;
	unsigned char  length;
	TokenType tok;
};

#endif
