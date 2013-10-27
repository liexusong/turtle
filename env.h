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
