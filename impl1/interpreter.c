#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "interpreter.h"

typedef struct IntegerValue IntegerValue;
typedef struct FloatValue FloatValue;
typedef struct StringValue StringValue;
typedef union Value Value;

struct IntegerValue {
	unsigned char type;
	long v;
};

struct FloatValue {
	unsigned char type;
	double v;
};

struct StringValue {
	unsigned char type;
	int len;
	char *p;
};

union Value {
	IntegerValue i;
	FloatValue f;
	StringValue s;
};

static Value parseExpr(Expression *expr);

void Interpreter_interpretAST(Expression *expr) {
	Value v = parseExpr(expr);
	switch (v.i.type) {
	case INTEGER:
		printf("%ld\n", v.i.v);
		break;
	case FLOAT:
		printf("%lf\n", v.f.v);
		break;
	case STRING:
		printf("\"%s\"\n", v.s.p);
		free(v.s.p);
		break;
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
		v.i.type = expr->lit.value.tok;
		switch (v.i.type) {
		case INTEGER:
			v.i.v = strtol(start + pos, NULL, 10);
			break;
		case FLOAT:
			v.f.v = strtod(start + pos, NULL);
			break;
		case STRING:
			v.s.len = expr->lit.value.length - 2;
			v.s.p = malloc(v.s.len + 1);
			memcpy(v.s.p, start + pos + 1, v.s.len);
			v.s.p[v.s.len] = '\0';
			break;
		}
		break;
	case BINARY_EXPRESSION:
		v = parseExpr(expr->bin.left);
		Value v2 = parseExpr(expr->bin.right);
		if (
			v.s.type == STRING && (v2.i.type == INTEGER || v2.f.type == FLOAT) ||
			v2.s.type == STRING && (v.i.type == INTEGER || v.f.type == FLOAT)
		) {
			fprintf(stderr, 
				"[Semantic Error] Invalid operation between a number and a string.\n"
				"\t\%.*s"
				"\t\%*s^\n\n",
				len, start, pos, "" 
			);
			v.i.type = -1;
			return v;
		}
		if (v.s.type == STRING && v2.s.type == STRING) {
			unsigned char op = expr->bin.op.tok;
			if (op == '+') {
				char *tmp = malloc(v.s.len + v.s.len + 1);
				sprintf(tmp, "%s%s", v.s.p, v2.s.p);
				free(v.s.p);
				free(v2.s.p);
				v.s.p = tmp;
			} else if (op == '>') {
				int cmp = strcmp(v.s.p, v2.s.p);
				free(v.s.p);
				free(v2.s.p);
				v.i.type = INTEGER;
				v.i.v = cmp > 0;
			} else if (op == GREATER_EQUAL) {
				int cmp = strcmp(v.s.p, v2.s.p);
				free(v.s.p);
				free(v2.s.p);
				v.i.type = INTEGER;
				v.i.v = cmp >= 0;
			} else if (op == '<') {
				int cmp = strcmp(v.s.p, v2.s.p);
				free(v.s.p);
				free(v2.s.p);
				v.i.type = INTEGER;
				v.i.v = cmp < 0;
			} else if (op == LOWER_EQUAL) {
				int cmp = strcmp(v.s.p, v2.s.p);
				free(v.s.p);
				free(v2.s.p);
				v.i.type = INTEGER;
				v.i.v = cmp <= 0;
			} else if (op == EQUAL_EQUAL) {
				int cmp = strcmp(v.s.p, v2.s.p);
				free(v.s.p);
				free(v2.s.p);
				v.i.type = INTEGER;
				v.i.v = cmp == 0;
			} else if (op == BANG_EQUAL) {
				int cmp = strcmp(v.s.p, v2.s.p);
				free(v.s.p);
				free(v2.s.p);
				v.i.type = INTEGER;
				v.i.v = cmp != 0;
			} else {
				fprintf(stderr, 
					"[Semantic Error] Invalid operation between strings.\n"
					"\t\%.*s"
					"\t\%*s^\n\n",
					len, start, pos, "" 
				);
				free(v.s.p);
				free(v2.s.p);
				v.i.type = -1;
			}
			return v;
		}
		if (v.i.type == INTEGER && v2.i.type == INTEGER) {
			switch (expr->bin.op.tok) {
			case PLUS:
				v.i.v += v2.i.v;
				break;
			case MINUS:
				v.i.v -= v2.i.v;
				break;
			case STAR:
				v.i.v *= v2.i.v;
				break;
			case SLASH:
				v.i.v /= v2.i.v;
				break;
			case PERCENT:
				v.i.v %= v2.i.v;
				break;
			case AND:
				v.i.v &= v2.i.v;
				break;
			case PIPE:
				v.i.v |= v2.i.v;
				break;
			case CARET:
				v.i.v ^= v2.i.v;
				break;
			case GREATER:
				v.i.v = v.i.v > v2.i.v;
				break;
			case GREATER_EQUAL:
				v.i.v = v.i.v >= v2.i.v;
				break;
			case LOWER:
				v.i.v = v.i.v < v2.i.v;
				break;
			case LOWER_EQUAL:
				v.i.v = v.i.v <= v2.i.v;
				break;
			case EQUAL_EQUAL:
				v.i.v = v.i.v == v2.i.v;
				break;
			case BANG_EQUAL:
				v.i.v = v.i.v != v2.i.v;
				break;
			default:
				fprintf(stderr, 
					"[Semantic Error] Invalid operation between two integers.\n"
					"\t\%.*s"
					"\t\%*s^\n\n",
					len, start, pos, "" 
				);
				v.i.type = -1;
			} 
			return v;
		}
		if (v.i.type == INTEGER) {
			v.f.v = (double) v.i.v;
			v.f.type = FLOAT;
		} else if (v2.i.type == INTEGER) {
			v2.f.v = (double) v2.i.v;
			v2.f.type = FLOAT;
		}
		switch (expr->bin.op.tok) {
		case PLUS:
			v.f.v += v2.f.v;
			break;
		case MINUS:
			v.f.v -= v2.f.v;
			break;
		case STAR:
			v.f.v *= v2.f.v;
			break;
		case SLASH:
			v.f.v /= v2.f.v;
			break;
		case GREATER:
			v.i.v = v.f.v > v2.f.v;
			v.i.type = INTEGER;
			break;
		case GREATER_EQUAL:
			v.i.v = v.f.v >= v2.f.v;
			v.i.type = INTEGER;
			break;
		case LOWER:
			v.i.v = v.f.v < v2.f.v;
			v.i.type = INTEGER;
			break;
		case LOWER_EQUAL:
			v.i.v = v.f.v <= v2.f.v;
			v.i.type = INTEGER;
			break;
		case EQUAL_EQUAL:
			v.i.v = v.f.v == v2.f.v;
			v.i.type = INTEGER;
			break;
		case BANG_EQUAL:
			v.i.v = v.f.v != v2.f.v;
			v.i.type = INTEGER;
			break;
		default:
			fprintf(stderr, 
				"[Semantic Error] Invalid operation between two floats.\n"
				"\t\%.*s"
				"\t\%*s^\n\n",
				len, start, pos, "" 
			);
			v.i.type = -1;
		} 
		return v;

	case UNARY_EXPRESSION:
		if (expr->un.right->lit.type == STRING) {
			fprintf(stderr, 
				"[Semantic Error] Invalid operation on a string.\n"
				"\t\%.*s"
				"\t\%*s^\n\n",
				len, start, pos, "" 
			);
			v.i.type = -1;
			return v;
		}
		v = parseExpr(expr->un.right);
		switch (expr->un.op.tok) {
		case MINUS:
			if (v.f.type == FLOAT) {
				v.f.v = -v.f.v;
			} else if (v.i.type == INTEGER) {
				v.i.v = -v.i.v;
			}
			return v;
		case STAR: 
			fprintf(stderr, 
				"[Semantic Error] Invalid operation on a string.\n"
				"\t\%.*s"
				"\t\%*s^\n\n",
				len, start, pos, "" 
			);
			v.i.type = -1;
			return v;
		}
	case GROUP_EXPRESSION:
		return parseExpr(expr->group.expr);
	}
	
	return v;
}
