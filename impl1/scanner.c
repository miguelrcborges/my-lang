#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "token.h"
#include "scanner.h"

enum CONSTANTS {
	INITIAL_N_TOKENS_ALLOCATED = 20,
	INITIAL_LINE_START_LOOKUP_ALLOCATED = 50,
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

bool Tokenizer_hasError() {
	return error;
};

char *Tokenizer_errorMessage() {
	return error_messages[error];
}

Token *Tokenizer_tokenize(const char *str) {
	if (str[0] == '\0') {
		error = EMPTY_STRING;
		return NULL;
	}

	Token *p = malloc(INITIAL_N_TOKENS_ALLOCATED * sizeof(*p));
	size_t *lines_start = malloc(INITIAL_LINE_START_LOOKUP_ALLOCATED * sizeof(*lines_start));
	if (p == NULL || lines_start == NULL) {
		error = OUT_OF_MEMORY;
		return NULL;
	}
	
	int ntok = 0;
	int line = 1;
	int line_pos = 0;
	lines_start[0] = 0;
	int alines = INITIAL_LINE_START_LOOKUP_ALLOCATED;
	int atok = INITIAL_N_TOKENS_ALLOCATED;


#define TOKEN(__t, __len) (Token) { \
	.tok = __t, \
	.line = line, \
	.line_pos = line_pos, \
	.length = __len \
}

	for (int c = 0; str[c] != '\0'; line_pos++, c++) {
		if (ntok == atok) {
			Token *n_p = realloc(p, atok * (2 * sizeof(*p)));
			if (n_p == NULL) {
				error = OUT_OF_MEMORY;
				return n_p;
			}
			p = n_p;
			atok *= 2;
		}
		switch (str[c]) {
		case '\r':
			if (str[c+1] == '\n') {
				c += 1;
			}
		case '\n':
			if (line >= alines) {
				size_t *n_lines_start = realloc(lines_start, alines * (2 * sizeof(*lines_start)));
				if (n_lines_start == NULL) {
					error = OUT_OF_MEMORY;
					return NULL;
				}
				lines_start = n_lines_start;
				alines *= 2;
			}
			lines_start[line] = c + 1;
			line_pos = -1;
			line += 1;
			continue;

		case ' ':
		case '\t':
			continue;

		case LEFT_BRACE:
		case RIGHT_BRACE:
		case LEFT_PAREN:
		case RIGHT_PAREN:
		case COLON:
			p[ntok] = TOKEN(str[c], 1);
			continue;


#define HANDLE_EQUAL_TOKEN(__eq_t, __t) \
case __t: \
	if (str[c + 1] == '=' ) { \
		p[ntok] = TOKEN(__eq_t, 2); \
		c++; \
		line_pos++; \
	} else { \
		p[ntok] = TOKEN(__t, 1); \
	} \
	continue;

		HANDLE_EQUAL_TOKEN(PLUS_EQUAL, PLUS);
		HANDLE_EQUAL_TOKEN(MINUS_EQUAL, MINUS);
		HANDLE_EQUAL_TOKEN(STAR_EQUAL, STAR);
		HANDLE_EQUAL_TOKEN(SLASH_EQUAL, SLASH);
		HANDLE_EQUAL_TOKEN(PERCENT_EQUAL, PERCENT);
		HANDLE_EQUAL_TOKEN(BANG_EQUAL, BANG);
		HANDLE_EQUAL_TOKEN(EQUAL_EQUAL, EQUAL);
		HANDLE_EQUAL_TOKEN(GREATER_EQUAL, GREATER);
		HANDLE_EQUAL_TOKEN(LOWER_EQUAL, LOWER);
#undef HANDLE_EQUAL_TOKEN

		default:
			; // Especial semicolon. Did you know that you can't declare variables in the first expression
			  // of a switch/case??????????? TIL IKR
			char *str_end = strstr(str + lines_start[line-1], "\n");
			int line_len;
			if (str_end == NULL) {
				line_len = INT_MAX;
			} else {
				line_len = str_end - (str + lines_start[line-1]);
			}

			fprintf(stderr, 
				"[Syntax Error] Unexpected Token\n"
				"\t%d | %.*s\n"
				"\t    %*s^\n\n",
				line, line_len, str + lines_start[line-1], line_pos, "" 
			);
		}
	}
	p[ntok] = TOKEN(EOF_TOK, 1);
#undef TOKEN

	free(lines_start);
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
