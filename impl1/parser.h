/* 
 * Header dependencies:
 * - <stdbool.h>
 * - "token.h"
 * - "ast.h"
 */

#ifndef PARSER_H
#define PARSER_H

extern Expression *Parser_generateAST(Token *tokens);
extern bool       Parser_hasError();
extern char       *Parser_errorMessage();

#endif /* PARSER_H */
