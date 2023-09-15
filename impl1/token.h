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
	LOWER   = '<',

	/* Others */
	COLON       = ':',
	LEFT_PAREN  = '(',
	RIGHT_PAREN = ')',
	LEFT_BRACE  = '{',
	RIGHT_BRACE = '}',


	/* Non Single Characters */
	PLUS_EQUAL = 128,
	MINUS_EQUAL,
	STAR_EQUAL,
	SLASH_EQUAL,
	PERCENT_EQUAL,
	BANG_EQUAL,
	EQUAL_EQUAL,
	GREATER_EQUAL,
	LOWER_EQUAL,

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
	END,

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
