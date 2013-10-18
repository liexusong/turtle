#include "global.h"
#include "env.h"

struct env_entry *
E_VarEntry(struct S_symbol *sym, enum env_var_scope scope, int index)
{
    struct env_entry *e = malloc(sizeof *e);
    check_mem(e);

    e->kind = E_varEntry;
    e->sym = sym;
    e->u.var.scope = scope;
    e->index = index;

    return e;

error:
    return NULL;
}

struct env_entry *
E_FunEntry(struct S_symbol *sym, int count_params)
{
    struct env_entry *e = malloc(sizeof *e);
    check_mem(e);

    e->kind = E_funEntry;
    e->sym = sym;
    e->u.func.count_params = count_params;
    e->index = -1;

    return e;

error:
    return NULL;
}

struct hash_table *
E_base_venv(void)
{
    return S_empty();
}

struct hash_table *
E_base_fenv(void)
{
    return S_empty();
}

void
E_Set_Addr(struct hash_table *t, struct S_symbol *sym, int addr)
{
    struct env_entry *entry = S_look(t, sym);
    if (entry == NULL) {
        log_err("Trying to set a symbol that does not exits");
    }

    entry->index = addr;

    return;

error:
    return;
}
