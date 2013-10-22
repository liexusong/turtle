#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "symbol.h"
#include "dbg.h"
#include "parser.h"

void lyyerror(YYLTYPE t, char *s, ...);
void yyerror(char *s, ...);

extern FILE *out_fd;

#endif /* end of include guard: GLOBAL_H_ */

