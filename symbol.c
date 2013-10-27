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
#include "table.h"
#include "symbol.h"
#include "dbg.h"

static int      nested_level = 0;

struct s_symbol {
    char           *name;
    struct s_symbol *next;
};

static struct s_symbol *
mksymbol(char *name, struct s_symbol *next)
{
    struct s_symbol *s = malloc(sizeof(*s));
    check_mem(s);
    s->name = strdup(name);
    s->next = next;
    return s;
error:
    return NULL;
}

#define SIZE 109

static struct s_symbol *hashtable[SIZE];

static unsigned int
hash(char *s0)
{
    unsigned int    h = 0;
    char           *s;

    for (s = s0; *s; ++s) {
        h = h * 65599 + (unsigned int)(*s);
    }

    return h;
}

struct s_symbol *
s_new_symbol(char *name)
{
    int             index = hash(name) % SIZE;
    struct s_symbol *syms = hashtable[index];
    struct s_symbol *sym;

    for (sym = syms; sym; sym = sym->next) {
        if (strcmp(sym->name, name) == 0) {
            return sym;
        }
    }

    sym = mksymbol(name, syms);
    hashtable[index] = sym;
    return sym;
}

char           *
s_name(struct s_symbol *sym)
{
    return sym->name;
}

struct table   *
s_new_empty(void)
{
    return table_new_empty();
}

void
s_enter(struct table *t, struct s_symbol *sym, void *value)
{
    table_insert(t, sym, value);
}

void           *
s_find(struct table *t, struct s_symbol *sym)
{
    return table_find(t, sym);
}

struct s_symbol marksym = { "<mark>", NULL };

void
s_enter_scope(struct table *t)
{
    s_enter(t, &marksym, NULL);
    nested_level += 1;
}

void
s_leave_scope(struct table *t)
{
    struct s_symbol *s;

    do {
        s = table_pop(t);
    } while (s != &marksym);

    nested_level -= 1;
}

int
s_in_scope(void)
{
    return nested_level >= 1;
}

void
s_clear(void)
{
    int index;
    for (index = 0; index < SIZE; ++index) {
        struct s_symbol *syms = hashtable[index];
        struct s_symbol *sym;
        struct s_symbol *tmp;
        for (sym = syms; sym; sym = tmp) {
            log_info("free(name = %s)", sym->name);
            free(sym->name);
            tmp = sym->next;
            free(sym);
        }
    }
}
