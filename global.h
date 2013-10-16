#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


#ifndef YYSTYPE
#undef YYSTYPE
#endif
#define YYSTYPE struct ast_node *

#endif /* end of include guard: GLOBAL_H_ */

