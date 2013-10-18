#ifndef ENV_H_
#define ENV_H_

#include "symbol.h"
#include "hash.h"

struct env_entry {
    enum {
        E_varEntry,
        E_funEntry,
    } kind;
    struct S_symbol *sym;
    union {
        struct {
            int count_params;
        } func;
    } u;
};

struct env_entry *E_VarEntry(struct S_symbol *sym);
struct env_entry *E_FunEntry(struct S_symbol *sym, int count_params);

struct hash_table *E_base_venv(void);
struct hash_table *E_base_fenv(void);

#endif /* end of include guard: ENV_H_
