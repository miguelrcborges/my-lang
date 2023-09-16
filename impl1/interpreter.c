#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "interpreter.h"

typedef struct Value {
	unsigned char type;
	union {
		int i;
		double f;
		char *s;
	};
} Value;

static Value parseExpr(Expression *expr);

void Interpreter_interpretAST(Expression *expr) {
	Value v = parseExpr(expr);
	switch (v.type) {
	case INTEGER:
		printf("%d\n", v.i);
		break;
	case FLOAT:
		printf("%lf\n", v.f);
	case STRING:
		printf("\"%s\"\n", v.s);
		free(v.s);
	}
}

Value parseExpr(Expression *expr) {
	Value v;
	int len;
	int pos = expr->lit.value.line_pos;
	int line = expr->bin.op.line;
	const char *start = Scanner_getSourceLine(expr->lit.value.line, &len);
	switch (expr->un.type) {
	case LITERAL:
		v.type = expr->lit.value.tok;
		switch (v.type) {
		case INTEGER:
			v.i = strtol(start + pos, NULL, 10);
			break;
		case FLOAT:
			v.f = strtod(start + pos, NULL);
			break;
		case STRING:
			;
			int s_len = expr->lit.value.length;
			v.s = malloc(s_len - 1);
			memcpy(v.s, start + pos + 1, s_len - 2);
			v.s[s_len - 2] = '\0';
			break;
		}
		break;
	case BINARY_EXPRESSION:
		v = parseExpr(expr->bin.left);
		Value v2 = parseExpr(expr->bin.right);
		if (
			v.type == STRING && (v2.type == INTEGER || v2.type == FLOAT) ||
			v2.type == STRING && (v.type == INTEGER || v.type == FLOAT)
		) {
			fprintf(stderr, 
				"[Semantic Error] Tried to add a number with a string.\n"
				"\t\%.*s"
				"\t\%*s^\n\n",
				len, start, pos, "" 
			);
			v.type = -1;
			return v;
		}
		if (v.type == STRING && v2.type == STRING) {
			if (expr->bin.op.tok != '+') {
				fprintf(stderr, 
					"[Semantic Error] Invalid operation between strings.\n"
					"\t\%.*s"
					"\t\%*s^\n\n",
					len, start, pos, "" 
				);
				free(v.s);
				free(v2.s);
				v.type = -1;
			} else {
				int l = strlen(v.s) + strlen(v2.s);
				char *tmp = malloc(l + 1);
				sprintf(tmp, "%s%s", v.s, v2.s);
				free(v.s);
				free(v2.s);
				v.s = tmp;
			}
		}
	case UNARY_EXPRESSION:
	case GROUP_EXPRESSION:
		break;
	}
	
	return v;
}
