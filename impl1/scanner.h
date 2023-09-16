/* 
 * Header dependencies:
 * - <stdbool.h>
 * - "token.h"
 */

#ifndef SCANNER_H
#define SCANNER_H

extern Token       *Scanner_tokenize(const char *str);
extern bool        Scanner_hasError();
extern char        *Scanner_errorMessage();
extern const char  *Scanner_getSourceLine(int line, int *len);

#endif /* SCANNER_H */
