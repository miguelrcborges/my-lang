/* 
 * Header dependencies:
 * - <stdbool.h>
 * - "token.h"
 */

#ifndef SCANNER_H
#define SCANNER_H

extern Token *Tokenizer_tokenize(const char *str);
extern bool  Tokenizer_hasError();
extern char  *Tokenizer_errorMessage();

#endif
