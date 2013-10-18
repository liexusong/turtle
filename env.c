#include "global"
#include "env.h"

struct env_entry *
E_VarEntry(void)
{
    struct env_entry *e = malloc(sizeof *e);
    check_mem(e);

    e->kind = E_varEntry;

    return e;

error:
    return NULL;
}

struct env_entry *
E_FunEntry(int count_params)
{
    struct env_entry *e = malloc(sizeof *e);
    check_mem(e);

    e->kind = E_funEntry;
    e->u.func.count_params = count_params;

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
