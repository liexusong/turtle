#include "global.h"
#include "hash.h"
#include "symbol.h"
#include "dbg.h"

static int nested_level = 0;

struct s_symbol {
    char *name;
    struct s_symbol *next;
};

static struct s_symbol *mksymbol(char *name, struct s_symbol *next)
{
    struct s_symbol *s = malloc(sizeof *s);
    check_mem(s);

    s->name = name;
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
    unsigned int h = 0;
    char *s;
    for (s = s0; *s; ++s) {
        h = h * 65599 + *s;
    }
    return h;
}

struct s_symbol*
s_new_symbol(char *name)
{
    int index = hash(name) % SIZE;
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

char *s_name(struct s_symbol *sym)
{
    return sym->name;
}

struct table*
s_new_empty(void)
{
    return table_new_empty();
}

void
s_enter(struct table *t, struct s_symbol *sym, void *value)
{
    table_insert(t, sym, value);
}

void *
s_find(struct table *t, struct s_symbol *sym)
{
    return table_find(t, sym);
}

struct s_symbol marksym = {"<mark>", NULL};

void s_enter_scope(struct table *t)
{
    s_enter(t, &marksym, NULL);
    nested_level += 1;
}

void s_leave_scope(struct table *t)
{
    struct s_symbol *s;
    do {
        s = table_pop(t);
    } while (s != &marksym);
    nested_level -= 1;
}

int s_in_scope(void)
{
    return nested_level >= 1;
}
