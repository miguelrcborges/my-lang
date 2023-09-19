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
	EMPTY_STRING = 1,
	OUT_OF_MEMORY,
	INVALID_TOKEN,
	UNCLOSED_STRING,
};

typedef struct KeywordStruct KeywordStruct;
struct KeywordStruct {
	char *string;
	size_t length:56;
	TokenType Token;
};

static KeywordStruct keywords[] = {
	{ "import", 6, IMPORT },
	{ "fun", 3, FUN },
	{ "for", 3, FOR },
	{ "if", 2, IF },
	{ "else", 4, ELSE },
	{ "return", 6, RETURN },
	{ "end", 3, END }, // MAY BE REMOVED SOON BY SWITCHING TO IDENTION BASED SCOPES
};

static int error = 0;

static char *error_messages[] = {
	"No errors found."
	"Empty string sent to the scanner.",
	"Failed to allocate memory.",
	"Found invalid token.",
	"Unclosed string.",
};

static const char *src = NULL;
static size_t *lines_start = NULL;
static size_t line;

static bool isAlpha(char c);
static bool isNumeric(char c);
static bool isAlphanumeric(char c);

bool Scanner_hasError() {
	return error;
};

char *Scanner_errorMessage() {
	return error_messages[error];
}

const char *Scanner_getSourceLine(int _line, int *len) {
	if (_line > line) {
		if (len != NULL)
			*len = 0;
		return "";
	} 

	if (len == NULL) {
		if (_line == line)
			*len = INT_MAX;
		else
			*len = lines_start[line] - lines_start[line-1] - 1;
	}
	return src + lines_start[_line-1];
}

Token *Scanner_tokenize(const char *str) {
	if (str[0] == '\0') {
		error = EMPTY_STRING;
		return NULL;
	}
	src = str;

	if (lines_start != NULL) {
		free(lines_start);
	}

	line = 1;
	Token *p = malloc(INITIAL_N_TOKENS_ALLOCATED * sizeof(*p));
	lines_start = malloc(INITIAL_LINE_START_LOOKUP_ALLOCATED * sizeof(*lines_start));
	if (p == NULL || lines_start == NULL) {
		error = OUT_OF_MEMORY;
		return NULL;
	}
	
	int ntok = 0;
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
		case ',':
			continue;

		case '"':
			;
			size_t start = c;
			do {
				c += 1;
			} while (str[c] != '"' && str[c+1] != '\r' && str[c+1] != '\n' && str[c+1] != '\0');

			if (str[c] != '"' && (str[c+1] == '\r' || str[c+1] == '\n' || str[c+1] == '\0')) {
				char *str_end = strstr(str + lines_start[line-1], "\n");
				int line_len;
				if (str_end == NULL) {
					line_len = INT_MAX;
				} else {
					line_len = str_end - (str + lines_start[line-1]);
				}
				line_pos += c - start + 1;

				fprintf(stderr, 
					"[Syntax Error] Unclosed String\n"
					"\t%d | %.*s\n"
					"\t    %*s^\n\n",
					(int) line, line_len, str + lines_start[line-1], line_pos, "" 
				);
				error = UNCLOSED_STRING;
				continue;
			}

			p[ntok] = TOKEN(STRING, c - start + 1);
			line_pos += c - start;
			ntok += 1;
			break;


		case LEFT_BRACE:
		case RIGHT_BRACE:
		case LEFT_PAREN:
		case RIGHT_PAREN:
		case LEFT_BRACKET:
		case RIGHT_BRACKET:
		case COLON:
			p[ntok] = TOKEN(str[c], 1);
			ntok++;
			continue;

		case AND:
			if (str[c + 1] == '&') {
				p[ntok] = TOKEN(AND_AND, 2);
				c += 1;
				line_pos += 1;
			} else {
				p[ntok] = TOKEN(AND, 1);
			}
			ntok++;
			continue;
		case PIPE:
			if (str[c + 1] == '|') {
				p[ntok] = TOKEN(PIPE_PIPE, 2);
				c += 1;
				line_pos += 1;
			} else {
				p[ntok] = TOKEN(PIPE, 1);
			}
			ntok += 1;
			continue;

		case SLASH:
			if (str[c+1] == '=') {
				p[ntok] = TOKEN(SLASH_EQUAL, 2);
				c += 1;
				line_pos += 1;
				ntok += 1;
			} else if (str[c+1] == '/') {
				do {
					c += 1;
				} while (str[c+1] != '\n' && str[c+1] != '\r');
			} else if (str[c+1] == '*') {
				int nest = 1;
				c += 2;
				line_pos += 2;
				while (str[c] == '\0' && nest > 0) {
					bool is_newline = false;
					if (str[c] == '/' && str[c+1] == '*') {
						nest += 1;
						c += 2;
						line_pos += 2;
					} else if (str[c] == '*' && str[c+1] == '/') {
						nest -= 1;
						c += 2;
						line_pos += 2;
					} else if (str[c] == '\r') {
						is_newline = true;
					} else if (str[c] == '\n' || is_newline) {
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
					} else {
						c += 1;
						line_pos += 1;
					}
				}
				c -= 1;
			} else {
				p[ntok] = TOKEN(SLASH, 1);
				ntok += 1;
			}


#define HANDLE_EQUAL_TOKEN(__eq_t, __t) \
case __t: \
	if (str[c + 1] == '=' ) { \
		p[ntok] = TOKEN(__eq_t, 2); \
		c += 1; \
		line_pos += 1; \
	} else { \
		p[ntok] = TOKEN(__t, 1); \
	} \
	ntok += 1; \
	continue;

		HANDLE_EQUAL_TOKEN(PLUS_EQUAL, PLUS);
		HANDLE_EQUAL_TOKEN(MINUS_EQUAL, MINUS);
		HANDLE_EQUAL_TOKEN(STAR_EQUAL, STAR);
		HANDLE_EQUAL_TOKEN(PERCENT_EQUAL, PERCENT);
		HANDLE_EQUAL_TOKEN(BANG_EQUAL, BANG);
		HANDLE_EQUAL_TOKEN(EQUAL_EQUAL, EQUAL);
		HANDLE_EQUAL_TOKEN(GREATER_EQUAL, GREATER);
		HANDLE_EQUAL_TOKEN(LOWER_EQUAL, LOWER);
#undef HANDLE_EQUAL_TOKEN

		default:
			if (isNumeric(str[c])) {
				int token_type = INTEGER;
				size_t start_pos = c;
				while (isNumeric(str[c+1]))
					c += 1;
				if (str[c+1] == '.') {
					token_type = FLOAT;
					c += 1;
					while (isNumeric(str[c+1]))
						c += 1;
				}
				p[ntok] = TOKEN(token_type, c - start_pos + 1);
				line_pos += c - start_pos;
				ntok += 1;
				continue;
			} else if (isAlpha(str[c])) {
				size_t start_pos = c;
				while (isAlphanumeric(str[c+1]))
					c += 1;

				size_t len = c - start_pos + 1;
				bool is_identifier = true;
				for (size_t i = 0; i < sizeof(keywords)/sizeof(keywords[0]); i += 1) {
					if (len == keywords[i].length && strncmp(&(src[start_pos]), keywords[i].string, len) == 0) {
						is_identifier = false;
						p[ntok] = TOKEN(keywords[i].Token, len);
						break;
					}
				}

				if (is_identifier)
					p[ntok] = TOKEN(IDENTIFIER, len);
				
				line_pos += c - start_pos;
				ntok += 1;
				continue;
			}


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
				(int) line, line_len, str + lines_start[line-1], line_pos, "" 
			);
		}
	}
	p[ntok] = TOKEN(EOF_TOK, 1);
#undef TOKEN

	return p;
}

inline bool isAlpha(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

inline bool isNumeric(char c) {
	return c >= '0' && c <= '9';
}

inline bool isAlphanumeric(char c) {
	return isAlpha(c) || isNumeric(c);
}
