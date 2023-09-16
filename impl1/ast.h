/* 
 * Header dependencies:
 * - "token.h"
 */

#ifndef AST_H
#define AST_H

typedef struct BinaryExpression BinaryExpression;
typedef struct GroupExpression GroupExpression;
typedef struct UnaryExpression UnaryExpression;
typedef struct Literal Literal;
typedef union Expression Expression;

typedef enum EXPRESSION_TYPE {
	BINARY_EXPRESSION,
	GROUP_EXPRESSION,
	UNARY_EXPRESSION,
	LITERAL,
} EXPRESSION_TYPE;

struct BinaryExpression {
	EXPRESSION_TYPE type;
	Token op;
	Expression *left;
	Expression *right;
};

struct GroupExpression {
	EXPRESSION_TYPE type;
	Expression *expr;
};

struct UnaryExpression {
	EXPRESSION_TYPE type;
	Token op;
	Expression *right;
};

struct Literal {
	EXPRESSION_TYPE type;
	Token value;
};

union Expression {
	BinaryExpression bin;
	GroupExpression group;
	UnaryExpression un;
	Literal lit;
};

#endif /* AST_H */
