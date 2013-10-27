/*-
 * Copyright (c) 1998, Andrew W. Appel with Maia Ginsburg
 * All rights reserved.
 */

/**
 * Converts each string (char *) to a symbol, so that all the different
 * occurrences of any given string to the same symbol.
 *
 * For more information, please refer to the tiger book.
 */

#ifndef SYMBOL_H_
#define SYMBOL_H_

struct s_symbol;

/**
 * @return a point to s_symbol
 *
 * A new s_symbol will be allocated if and only we have not seen @name so far.
 */
struct s_symbol *s_new_symbol(char *name);

/**
 * @return the name of the symbol as a C string
 */
char *s_name(struct s_symbol *sym);

/**
 * @return a new (hash)table for symbols
 */
struct table *s_new_empty(void);

/**
 * Inserts an entry with @sym as key and @value into the table @t
 *
 * Wrapper of table_insert()
 */
void s_insert(struct table *t, struct s_symbol *sym, void *value);

/**
 * @return the key associated with @sym in the able @t if there is any
 *
 * Wrapper of table_find()
 */
void *s_find(struct table *t, struct s_symbol *sym);

/**
 * Stores the state of table @t so that s_leave_scope can restore the state of
 * the table to the current one.
 */
void s_enter_scope(struct table *t);

/**
 * Restores table @t to the exact state when s_enter_scope() was called last
 * time
 */
void s_leave_scope(struct table *t);

/**
 * @return a positive number if s_enter_scope() is called but s_leave_scope() is
 * not called yet
 */
int s_in_scope(void);

/**
 * 
 */
extern struct s_symbol marksym;

/**
 * Frees all dynamic memory
 */
void s_clear(void);

#endif /* end of include guard: SYMBOL_H_ */

