#include "global.h"
#include "env.h"

struct env_entry *
env_new_var(struct s_symbol *sym, enum env_var_scope scope, int index)
{
    struct env_entry *e = malloc(sizeof *e);
    check_mem(e);

    e->kind = env_varEntry;
    e->sym = sym;
    e->u.var.scope = scope;
    e->index = index;

    return e;

  error:
    return NULL;
}

struct env_entry *
env_new_fun(struct s_symbol *sym, int count_params)
{
    struct env_entry *e = malloc(sizeof *e);
    check_mem(e);

    e->kind = env_funEntry;
    e->sym = sym;
    e->u.func.count_params = count_params;
    e->index = -1;

    return e;

  error:
    return NULL;
}

struct table   *
env_base_venv(void)
{
    return s_new_empty();
}

struct table   *
env_base_fenv(void)
{
    return s_new_empty();
}

void
env_set_addr(struct table *t, struct s_symbol *sym, int addr)
{
    struct env_entry *entry = s_find(t, sym);
    check(entry, "Trying to set a symbol that does not exits");

    entry->index = addr;
    return;

  error:
    return;
}
