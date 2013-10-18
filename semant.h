#ifndef SEMANT_H_
#define SEMANT_H_

#include "absyn.h"
#include "hash.h"

/*void transProg(struct A_prog *p);
void transValDefs(struct A_prog *p, struct hash_table *venv, struct hash_table *fenv);
void transExp(struct A_prog *p);
void transStmt(struct A_prog *p);
*/


void sem_trans_prog(struct A_program *prog);

#endif /* end of include guard: SEMANT_H_ */

