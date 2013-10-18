#include "global.h"
#include "hash.h"
#include "dbg.h"

#define HASH_TBL_SIZE 109

struct binder {
    void *key;
    void *value;
    struct binder *next;
    void *prevtop;
};

struct hash_table {
    struct binder *table[HASH_TBL_SIZE];
    void *top;
};


struct binder *Binder(void *key, void *value, struct binder *next, void *prevtop)
{
    struct binder *b = malloc(sizeof *b);
    check_mem(b);

    b->key = key;
    b->value = value;
    b->next = next;
    b->prevtop = prevtop;
    return b;

error:
    return NULL;
}

struct hash_table*
hash_table_empty(void)
{
    struct hash_table *t = malloc(sizeof *t);
    check_mem(t);

    t->top = NULL;

    int i;
    for (i = 0; i < HASH_TBL_SIZE; ++i)
    {
        t->table[i] = NULL;
    }

    return t;

error:
    return NULL;
}

void
hash_insert(struct hash_table *t, void *key, void *value)
{
    check(t && key, "NULL pointer...");
    int index;
    index = ((unsigned) key) % HASH_TBL_SIZE;
    t->table[index] = Binder(key, value, t->table[index], t->top);
    t->top = key;

error:
    return;
}

void *hash_lookup(struct hash_table *t, void *key)
{
    check(t && key, "NULL pointer...");

    int index;
    struct binder *b;

    index = ((unsigned) key) % HASH_TBL_SIZE;
    for (b = t->table[index]; b; b = b->next)
    {
        if (b->key == &marksym) {
            return NULL;
        }
        if (b->key == key) {
            return b->value;
        }
    }

error:
    return NULL;
}

void *hash_pop(struct hash_table *t)
{
    check(t, "NULL pointer...");

    void *k;
    struct binder *b;
    int index;

    k = t->top;
    check(k, "Something is wrong...");

    index = ((unsigned)k) % HASH_TBL_SIZE;
    b = t->table[index];
    t->table[index] = b->next;
    t->top = b->prevtop;
    return b->key;

error:
    return NULL;
}

