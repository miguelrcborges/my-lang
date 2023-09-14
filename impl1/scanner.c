#include <stdlib.h>
#include <stdbool.h>

#include "token.h"
#include "scanner.h"

enum CONSTANTS {
	INITIAL_N_TOKENS_ALLOCATED,
};

enum ERRORS {
	EMPTY_STRING,
	OUT_OF_MEMORY,
	INVALID_TOKEN,
};

bool error = 0;

char *error_messages[] = {
	"No errors found."
	"Empty string sent to the scanner.",
	"Failed to allocate memory.",
	"Found invalid token.",
};

static bool isLetter(char c);
static bool isDigit(char c);
static bool isWhiteSpace(char c);

bool tokenize_hasError() {
	return error;
};

char *Tokenizer_errorMessage() {
	return error_messages[error];
}

Token *tokenize(const char *str) {
	if (str[0] == '\0') {
		error = EMPTY_STRING;
		return NULL;
	}

	Token *p = (Token *)malloc(INITIAL_N_TOKENS_ALLOCATED * sizeof(Token));
	if (p == NULL) {
		error = OUT_OF_MEMORY;
		return NULL;
	}
	
	size_t ntok = 0;
	size_t line = 1;
	size_t line_pos = 0;
	size_t atok = INITIAL_N_TOKENS_ALLOCATED;
	for (size_t c; str[c] != '\0'; c++) {
		switch (str[c]) {
		case LEFT_BRACE:
		case RIGHT_BRACE:
		case LEFT_PAREN:
		case RIGHT_PAREN:
		case COLON:
			p[ntok] = (Token) {
				.tok = str[c],
				.line = line,
				.line_pos = line_pos,
				.length = 1,
			};
		}
	}

	return p;
}

inline bool isLetter(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

inline bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

inline bool isWhiteSpace(char c) {
	return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}
