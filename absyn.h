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

#ifndef AST_H_
#define AST_H_

#include "symbol.h"

typedef int ast_pos;

struct ast_program {
    char *name;
    struct ast_var_dec_list *global_var_def_list;
    struct ast_fun_dec_list *func_def_list;
    struct ast_stmt_list *body;
};

enum ast_oper {
    ast_plusOp,
    ast_minusOp,
    ast_timesOp,
    ast_negOp,
    ast_EQ,
    ast_NEQ,
    ast_LT,
    ast_LEQ,
    ast_GT,
    ast_GEQ,
};

struct ast_var_dec {
    ast_pos pos;
    struct s_symbol *sym;
    struct ast_exp *init;
};

struct ast_var_dec_list {
    struct ast_var_dec *head;
    struct ast_var_dec_list *tail;
};

struct ast_fun_dec {
    ast_pos pos;
    struct s_symbol *name;
    struct ast_field_list *params;
    struct ast_var_dec_list *var;
    struct ast_stmt_list *body;
};

struct ast_fun_dec_list {
    struct ast_fun_dec *head;
    struct ast_fun_dec_list *tail;
};

struct ast_exp {
    enum {
        ast_varExp,
        ast_intExp,
        ast_callExp,
        ast_opExp,
    } kind;
    ast_pos pos;
    union {
        struct s_symbol *var;
        int intt;
        struct {
            struct s_symbol *func;
            struct ast_exp_list *args;
        } call;
        struct {
            enum ast_oper oper;
            struct ast_exp *left;
            struct ast_exp *right;
        } op;
    } u;
};

struct ast_exp_list {
    struct ast_exp *head;
    struct ast_exp_list *tail;
};

struct ast_stmt {
    enum {
        ast_upStmt,
        ast_downStmt,
        ast_moveStmt,
        ast_readStmt,
        ast_assignStmt,
        ast_iftStmt,
        ast_ifteStmt,
        ast_whileStmt,
        ast_returnStmt,
        ast_callStmt,
        ast_exp_listStmt,
    } kind;
    ast_pos pos;
    union {
        /* up; - needs only the pos */
        /* down; - needs only the pos */
        struct {
            struct ast_exp *exp1;
            struct ast_exp *exp2;
        } move;
        struct {
            struct s_symbol *var;
        } read;
        struct {
            struct s_symbol *var;
            struct ast_exp *exp;
        } assign;
        struct {
            struct ast_exp *test;
            struct ast_stmt_list *then;
        } ift;
        struct {
            struct ast_exp *test;
            struct ast_stmt_list *then;
            struct ast_stmt_list *elsee;
        } ifte;
        struct {
            struct ast_exp *test;
            struct ast_stmt_list *body;
        } whilee;
        struct {
            struct ast_exp *exp;
        } returnn;
        struct {
            struct s_symbol *func;
            struct ast_exp_list *args;
        } call;
        struct ast_exp_list *seq;
    } u;
};

struct ast_stmt_list {
    struct ast_stmt *head;
    struct ast_stmt_list *tail;
};

struct ast_field {
    ast_pos pos;
    struct s_symbol *name;
};

struct ast_field_list {
    struct ast_field *head;
    struct ast_field_list *tail;
};


struct ast_program *ast_new_program(char *program_name, struct ast_var_dec_list *global_var_def_list, struct ast_fun_dec_list *func_def_list, struct ast_stmt_list *body);

//struct ast_var *ast_Var(struct s_symbol *sym);

struct ast_var_dec *ast_new_var_dec(struct s_symbol *sym, struct ast_exp *init);
struct ast_var_dec_list *ast_new_var_dec_list(struct ast_var_dec *head, struct ast_var_dec_list *tail);

struct ast_fun_dec *ast_new_fundec(struct s_symbol *name, struct ast_field_list *params, struct ast_var_dec_list *var, struct ast_stmt_list *body);
struct ast_fun_dec_list *ast_new_fundec_list(struct ast_fun_dec *head, struct ast_fun_dec_list *tail);

struct ast_exp *ast_new_var_exp(struct s_symbol *var);
struct ast_exp *ast_int_exp(int i);
struct ast_exp *ast_new_call_exp(struct s_symbol *func, struct ast_exp_list *args);
struct ast_exp *ast_new_op_exp(enum ast_oper oper, struct ast_exp *left, struct ast_exp *right);
struct ast_exp_list *ast_new_exp_list(struct ast_exp *head, struct ast_exp_list *tail);

struct ast_stmt *ast_new_up_stmt(void);
struct ast_stmt *ast_new_down_stmt(void);
struct ast_stmt *ast_new_move_stmt(struct ast_exp *exp1, struct ast_exp *exp2);
struct ast_stmt *ast_new_read_stmt(struct s_symbol *var);
struct ast_stmt *ast_new_assign_stmt(struct s_symbol *var, struct ast_exp *exp);
struct ast_stmt *ast_new_ift_stmt(struct ast_exp *test, struct ast_stmt_list *then);
struct ast_stmt *ast_new_ifte_stmt(struct ast_exp *test, struct ast_stmt_list *then, struct ast_stmt_list *elsee);
struct ast_stmt *ast_new_while_stmt(struct ast_exp *test, struct ast_stmt_list *body);
struct ast_stmt *ast_new_return_stmt(struct ast_exp *exp);
struct ast_stmt *ast_new_call_stmt(struct s_symbol *func, struct ast_exp_list *args);
struct ast_stmt *ast_new_exp_list_stmt(struct ast_exp_list *list);
struct ast_stmt_list *ast_new_stmt_list(struct ast_stmt *head, struct ast_stmt_list *tail);

struct ast_field *ast_new_field(struct s_symbol *name);
struct ast_field_list *ast_new_field_list(struct ast_field *head, struct ast_field_list *tail);

#endif /* end of include guard: AST_H_ */
