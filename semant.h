#ifndef SEMANT_H_
#define SEMANT_H_

#include "absyn.h"
#include "instruction.h"

#define OUTPUT_SIZE

void sem_trans_prog(struct ast_program *prog);

#endif /* end of include guard: SEMANT_H_ */

