#ifndef SYMBOL_H_
#define SYMBOL_H_

struct S_symbol;

struct S_symbol* S_Symbol(char *name);

char *S_name(struct S_symbol *sym);

struct hash_table *S_empty(void);

void S_enter(struct hash_table *t, struct S_symbol *sym, void *value);

void *S_look(struct hash_table *t, struct S_symbol *sym);

void S_beginScope(struct hash_table *t);

void S_endScope(struct hash_table *t);

#endif /* end of include guard: SYMBOL_H_ */

