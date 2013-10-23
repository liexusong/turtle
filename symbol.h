#ifndef SYMBOL_H_
#define SYMBOL_H_

struct s_symbol;

struct s_symbol* s_new_symbol(char *name);

char *s_name(struct s_symbol *sym);

struct table *s_new_empty(void);

void s_enter(struct table *t, struct s_symbol *sym, void *value);

void *s_find(struct table *t, struct s_symbol *sym);

void s_enter_scope(struct table *t);

void s_leave_scope(struct table *t);

int s_in_scope(void);

extern struct s_symbol marksym;

#endif /* end of include guard: SYMBOL_H_ */

