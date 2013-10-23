#ifndef ENV_H_
#define ENV_H_

#include "symbol.h"
#include "table.h"

enum env_var_scope {
    env_global,
    env_local,
};

struct env_entry {
    enum {
        env_varEntry,
        env_funEntry,
    } kind;
    struct s_symbol *sym;
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

struct env_entry *env_new_var(struct s_symbol *sym, enum env_var_scope scope, int index);
struct env_entry *env_new_fun(struct s_symbol *sym, int count_params);

void env_set_addr(struct table *t, struct s_symbol *sym, int addr);

struct table *env_base_venv(void);
struct table *env_base_fenv(void);

#endif /* end of include guard: ENV_H_ */
