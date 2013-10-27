/*-
 * Copyright (c) 1998, Andrew W. Appel with Maia Ginsburg
 * All rights reserved.
 */

/**
 * Implements what we call symbol table in this course.
 *
 * For more information, please refer to the tiger book.
 */

#ifndef ENV_H_
#define ENV_H_

#include "symbol.h"
#include "table.h"

enum env_var_scope {
    // Variable defined/declared in the global scope
    env_global,
    // Variable defined/declared in the function scope, i.e., function
    // parameters or local variables
    env_local,
};

/**
 * Environment entry that can be either a variable or a function
 */
struct env_entry {
    enum {
        env_varEntry,
        env_funEntry,
    } kind;
    struct s_symbol *sym;
    // I know this is misleading... This should be `address' or `offset'...
    int index;
    union {
        struct {
            // The scope in which the variable is defined/declared
            enum env_var_scope scope;
        } var;
        struct {
            // No. of parameters expected
            int count_params;
        } func;
    } u;
};

/**
 * Constructs a new environment entry for the variable
 */
struct env_entry *env_new_var(struct s_symbol *sym, enum env_var_scope scope,
                              int index);

/**
 * Constructs a new environment entry for the function
 */
struct env_entry *env_new_fun(struct s_symbol *sym, int count_params);

/**
 * Set the address/offset of an environment entry
 *
 * @param t the table to which the entry belongs
 * @param sym the symbol of the entry
 * @param addr the address to which the entry with symbol @sym in table @t
 *             should be assigned
 */
void env_set_addr(struct table *t, struct s_symbol *sym, int addr);

/**
 * @return an empty variable environment table
 */
struct table *env_base_venv(void);

/**
 * @return an empty function environment table
 */
struct table *env_base_fenv(void);

#endif /* end of include guard: ENV_H_ */
