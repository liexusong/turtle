#ifndef ENV_H_
#define ENV_H_

#include "symbol.h"
#include "hash.h"

enum env_var_scope {
    E_Global,
    E_Local,
};

struct env_entry {
    enum {
        E_varEntry,
        E_funEntry,
    } kind;
    struct S_symbol *sym;
    int index;
    union {
        struct {
            enum env_var_scope scope;
        } var;
        struct {
            int count_params;
        } func;
    } u;
};

struct env_entry *E_VarEntry(struct S_symbol *sym, enum env_var_scope scope, int index);
struct env_entry *E_FunEntry(struct S_symbol *sym, int count_params);

void E_Set_Addr(struct hash_table *t, struct S_symbol *sym, int addr);

struct hash_table *E_base_venv(void);
struct hash_table *E_base_fenv(void);

#endif /* end of include guard: ENV_H_ */
