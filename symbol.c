#include "global.h"
#include "hash.h"
#include "symbol.h"
#include "dbg.h"

static int nested_level = 0;

struct S_symbol {
    char *name;
    struct S_symbol *next;
};

static struct S_symbol *mksymbol(char *name, struct S_symbol *next)
{
    struct S_symbol *s = malloc(sizeof *s);
    check_mem(s);

    s->name = name;
    s->next = next;

    return s;

error:
    return NULL;
}

#define SIZE 109

static struct S_symbol *hashtable[SIZE];

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

static int streq(char *a, char *b)
{
    return strcmp(a, b) == 0;
}

struct S_symbol*
S_Symbol(char *name)
{
    int index = hash(name) % SIZE;
    struct S_symbol *syms = hashtable[index];
    struct S_symbol *sym;
    for (sym = syms; sym; sym = sym->next) {
        if (streq(sym->name, name)) {
            return sym;
        }
    }
    sym = mksymbol(name, syms);
    hashtable[index] = sym;
    return sym;
}

char *S_name(struct S_symbol *sym)
{
    return sym->name;
}

struct hash_table*
S_empty(void)
{
    return hash_table_empty();
}

void
S_enter(struct hash_table *t, struct S_symbol *sym, void *value)
{
    hash_insert(t, sym, value);
}

void *
S_look(struct hash_table *t, struct S_symbol *sym)
{
    return hash_lookup(t, sym);
}

struct S_symbol marksym = {"<mark>", NULL};

void S_beginScope(struct hash_table *t)
{
    S_enter(t, &marksym, NULL);
    nested_level += 1;
}

void S_endScope(struct hash_table *t)
{
    struct S_symbol *s;
    do {
        s = hash_pop(t);
    } while (s != &marksym);
    nested_level -= 1;
}

int in_frame(void)
{
    return nested_level >= 1;
}
