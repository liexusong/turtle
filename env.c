/*-
 * Copyright (c) 2013, Meitian Huang <_@freeaddr.info>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "global.h"
#include "env.h"

struct env_entry *
env_new_var(struct s_symbol *sym, enum env_var_scope scope, int index)
{
    struct env_entry *e = malloc(sizeof(*e));
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
    struct env_entry *e = malloc(sizeof(*e));
    check_mem(e);
    e->kind = env_funEntry;
    e->sym = sym;
    e->u.func.count_params = count_params;
    e->index = 0;
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
