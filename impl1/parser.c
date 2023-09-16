#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "ast.h"
#include "parser.h"

enum ERRORS {
	INVALID_AST = 1,
};

static int error = 0;
static char *error_messages[] = {
	"No errors found.",
	"Invalid AST.\n"
};

static int   token_position;
static Token *tokens;

static Expression *expression();
static Expression *addition();
static Expression *multiplication();
static Expression *unary();
static Expression *primary();

static inline int __type() {
	return tokens[token_position].tok;
}

bool Parser_hasError() {
	return error;
}

char *Parser_errorMessage() {
	return error_messages[error];
}

Expression *Parser_generateAST(Token *s_tokens) {
	token_position = 0;
	tokens = s_tokens;
	Expression *expr = expression();
	return expr;
}

Expression *expression() {
	return addition();
}

Expression *addition() {
	Expression *expr = multiplication();
	while (__type() == PLUS || __type() == MINUS) {
		Expression *n_expr = malloc(sizeof(BinaryExpression));
		n_expr->bin.type = BINARY_EXPRESSION;
		n_expr->bin.op = tokens[token_position];
		n_expr->bin.left = expr;
		token_position += 1;
		n_expr->bin.right = multiplication();
		expr = n_expr;
	}
	return expr;
}

Expression *multiplication() {
	Expression *expr = unary();
	while (__type() == STAR || __type() == SLASH || __type() == PERCENT) {
		Expression *n_expr = malloc(sizeof(BinaryExpression));
		n_expr->bin.type = BINARY_EXPRESSION;
		n_expr->bin.op = tokens[token_position];
		n_expr->bin.left = expr;
		token_position += 1;
		n_expr->bin.right = unary();
		expr = n_expr;
	}

	return expr;
}

Expression *unary() {
	int type = tokens[token_position].tok;
	while (__type() == BANG || __type() == MINUS || __type() == STAR) {
		Expression *n_expr = malloc(sizeof(UnaryExpression));
		n_expr->un.type = UNARY_EXPRESSION;
		n_expr->un.op = tokens[token_position];
		token_position += 1;
		n_expr->un.right = unary();
	}
	return primary();
}

Expression *primary() {
	Expression *expr;
	if (__type() == LEFT_PAREN) {
		expr = malloc(sizeof(GroupExpression));
		expr->group.type = GROUP_EXPRESSION;
		token_position += 1;
		expr->group.expr = expression();
		if (tokens[token_position].tok != RIGHT_PAREN) {
			fprintf(stderr, "Expected ) after expression in line %d.\n", tokens[token_position].line);
			error = INVALID_AST;
			free(expr);
			expr = NULL;
		}
		token_position += 1;
	} else {
		expr = malloc(sizeof(Literal));
		expr->lit.type = LITERAL;
		expr->lit.value = tokens[token_position];
		token_position += 1;
	}
	return expr;
}

void Parser_expressionResursiveFree(Expression *expr) {
	switch (expr->un.type) {
	case LITERAL:
		free(expr);
		break;
	case UNARY_EXPRESSION:
		Parser_expressionResursiveFree(expr->un.right);
		free(expr);
		break;
	case BINARY_EXPRESSION:
		Parser_expressionResursiveFree(expr->bin.right);
		Parser_expressionResursiveFree(expr->bin.left);
		free(expr);
		break;
	case GROUP_EXPRESSION:
		Parser_expressionResursiveFree(expr->group.expr);
		free(expr);
		break;
	}
}
