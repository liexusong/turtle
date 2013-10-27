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
#include "dbg.h"

#define TBL_SIZE 109

static struct binder  *binder_new_binder(void *key, void *value,
        struct binder *next,
        void *prevtop);

struct table {
    struct binder  *table[TBL_SIZE];
    void           *top;
};

struct binder {
    void           *key;
    void           *value;
    struct binder  *next;
    void           *prevtop;
};

static struct binder  *
binder_new_binder(void *key, void *value, struct binder *next,
                  void *prevtop)
{
    struct binder  *b = malloc(sizeof(*b));
    check_mem(b);
    b->key = key;
    b->value = value;
    b->next = next;
    b->prevtop = prevtop;

    record_allocated(b);
    return b;
error:
    return NULL;
}

struct table   *
table_new_empty(void)
{
    struct table   *t = malloc(sizeof(*t));
    check_mem(t);
    t->top = NULL;
    int             i;

    for (i = 0; i < TBL_SIZE; ++i) {
        t->table[i] = NULL;
    }

    return t;
error:
    return NULL;
}

void
table_insert(struct table *t, void *key, void *value)
{
    check(t && key, "NULL pointer...");
    int             index;
    index = ((uintptr_t) key) % TBL_SIZE;
    t->table[index] =
        binder_new_binder(key, value, t->table[index], t->top);
    t->top = key;
    return;
error:
    return;
}

void           *
table_find(struct table *t, void *key)
{
    check(t && key, "NULL pointer...");
    int             index;
    struct binder  *b;
    index = ((uintptr_t) key) % TBL_SIZE;

    for (b = t->table[index]; b; b = b->next) {
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

void           *
table_pop(struct table *t)
{
    check(t, "NULL pointer...");
    void           *k;
    struct binder  *b;
    int             index;
    k = t->top;
    check(k, "Something is wrong...");
    index = ((uintptr_t) k) % TBL_SIZE;
    b = t->table[index];
    t->table[index] = b->next;
    t->top = b->prevtop;
    return b->key;
error:
    return NULL;
}

