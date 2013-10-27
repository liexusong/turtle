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
#include "absyn.h"
#include "dbg.h"
#include "parser.h"
#include "lexer.h"

struct ast_program *
ast_new_program(char *program_name,
                struct ast_var_dec_list *global_var_def_list,
                struct ast_fun_dec_list *func_def_list,
                struct ast_stmt_list *body)
{
    struct ast_program *p = malloc(sizeof(*p));
    check_mem(p);
    p->name = program_name;
    p->global_var_def_list = global_var_def_list;
    p->func_def_list = func_def_list;
    p->body = body;
    return p;
error:
    return NULL;
}

struct ast_var_dec *
ast_new_var_dec(struct s_symbol *sym, struct ast_exp *init)
{
    struct ast_var_dec *p = malloc(sizeof(*p));
    check_mem(p);
    p->pos = yylloc;
    p->sym = sym;
    p->init = init;
    log_info("ast_var_dec(%s)\n", s_name(sym));
    return p;
error:
    return NULL;
}

struct ast_var_dec_list *
ast_new_var_dec_list(struct ast_var_dec *head,
                     struct ast_var_dec_list *tail)
{
    struct ast_var_dec_list *p = malloc(sizeof(*p));
    check_mem(p);
    p->head = head;
    p->tail = tail;
    return p;
error:
    return NULL;
}

struct ast_fun_dec *
ast_new_fundec(struct s_symbol *name,
               struct ast_field_list *params, struct ast_var_dec_list *var,
               struct ast_stmt_list *body)
{
    struct ast_fun_dec *p = malloc(sizeof(*p));
    check_mem(p);
    p->pos = yylloc;
    p->name = name;
    p->params = params;
    p->var = var;
    p->body = body;
    return p;
error:
    return NULL;
}

struct ast_fun_dec_list *
ast_new_fundec_list(struct ast_fun_dec *head,
                    struct ast_fun_dec_list *tail)
{
    struct ast_fun_dec_list *p = malloc(sizeof(*p));
    check_mem(p);
    p->head = head;
    p->tail = tail;
    return p;
error:
    return NULL;
}

struct ast_exp *
ast_new_var_exp(struct s_symbol *var)
{
    struct ast_exp *p = malloc(sizeof(*p));
    check_mem(p);
    p->kind = ast_varExp;
    p->pos = yylloc;
    p->u.var = var;
    return p;
error:
    return NULL;
}

struct ast_exp *
ast_int_exp(int i)
{
    struct ast_exp *p = malloc(sizeof(*p));
    check_mem(p);
    p->kind = ast_intExp;
    p->pos = yylloc;
    p->u.intt = i;
    return p;
error:
    return NULL;
}

struct ast_exp *
ast_new_call_exp(struct s_symbol *func,
                 struct ast_exp_list *args)
{
    struct ast_exp *p = malloc(sizeof(*p));
    check_mem(p);
    p->kind = ast_callExp;
    p->pos = yylloc;
    p->u.call.func = func;
    p->u.call.args = args;
    return p;
error:
    return NULL;
}

struct ast_exp *
ast_new_op_exp(enum ast_oper oper, struct ast_exp *left,
               struct ast_exp *right)
{
    struct ast_exp *p = malloc(sizeof(*p));
    check_mem(p);
    p->kind = ast_opExp;
    p->pos = yylloc;
    p->u.op.oper = oper;
    p->u.op.left = left;
    p->u.op.right = right;
    return p;
error:
    return NULL;
}


struct ast_stmt *
ast_new_up_stmt(void)
{
    struct ast_stmt *p = malloc(sizeof(*p));
    check_mem(p);
    p->kind = ast_upStmt;
    p->pos = yylloc;
    return p;
error:
    return NULL;
}

struct ast_stmt *
ast_new_down_stmt(void)
{
    struct ast_stmt *p = malloc(sizeof(*p));
    check_mem(p);
    p->kind = ast_downStmt;
    p->pos = yylloc;
    return p;
error:
    return NULL;
}


struct ast_stmt *
ast_new_move_stmt(struct ast_exp *exp1, struct ast_exp *exp2)
{
    struct ast_stmt *p = malloc(sizeof(*p));
    check_mem(p);
    p->kind = ast_moveStmt;
    p->pos = yylloc;
    p->u.move.exp1 = exp1;
    p->u.move.exp2 = exp2;
    return p;
error:
    return NULL;
}

struct ast_stmt *
ast_new_read_stmt(struct s_symbol *var)
{
    struct ast_stmt *p = malloc(sizeof(*p));
    check_mem(p);
    p->kind = ast_readStmt;
    p->pos = yylloc;
    p->u.read.var = var;
    return p;
error:
    return NULL;
}

struct ast_stmt *
ast_new_assign_stmt(struct s_symbol *var, struct ast_exp *exp)
{
    struct ast_stmt *p = malloc(sizeof(*p));
    check_mem(p);
    p->kind = ast_assignStmt;
    p->pos = yylloc;
    p->u.assign.var = var;
    p->u.assign.exp = exp;
    return p;
error:
    return NULL;
}

struct ast_stmt *
ast_new_ift_stmt(struct ast_exp *test, struct ast_stmt_list *then)
{
    struct ast_stmt *p = malloc(sizeof(*p));
    check_mem(p);
    p->kind = ast_iftStmt;
    p->pos = yylloc;
    p->u.ift.test = test;
    p->u.ift.then = then;
    return p;
error:
    return NULL;
}

struct ast_stmt *
ast_new_ifte_stmt(struct ast_exp *test, struct ast_stmt_list *then,
                  struct ast_stmt_list *elsee)
{
    struct ast_stmt *p = malloc(sizeof(*p));
    check_mem(p);
    p->kind = ast_ifteStmt;
    p->pos = yylloc;
    p->u.ifte.test = test;
    p->u.ifte.then = then;
    p->u.ifte.elsee = elsee;
    return p;
error:
    return NULL;
}

struct ast_stmt *
ast_new_while_stmt(struct ast_exp *test,
                   struct ast_stmt_list *body)
{
    struct ast_stmt *p = malloc(sizeof(*p));
    check_mem(p);
    p->kind = ast_whileStmt;
    p->pos = yylloc;
    p->u.whilee.test = test;
    p->u.whilee.body = body;
    return p;
error:
    return NULL;
}

struct ast_stmt *
ast_new_return_stmt(struct ast_exp *exp)
{
    struct ast_stmt *p = malloc(sizeof(*p));
    check_mem(p);
    p->kind = ast_returnStmt;
    p->pos = yylloc;
    p->u.returnn.exp = exp;
    return p;
error:
    return NULL;
}

struct ast_stmt *
ast_new_call_stmt(struct s_symbol *func,
                  struct ast_exp_list *args)
{
    struct ast_stmt *p = malloc(sizeof(*p));
    check_mem(p);
    p->kind = ast_callStmt;
    p->pos = yylloc;
    p->u.call.func = func;
    p->u.call.args = args;
    return p;
error:
    return NULL;
}

struct ast_stmt *
ast_new_exp_list_stmt(struct ast_exp_list *list)
{
    struct ast_stmt *p = malloc(sizeof(*p));
    check_mem(p);
    p->kind = ast_exp_listStmt;
    p->pos = yylloc;
    p->u.seq = list;
    return p;
error:
    return NULL;
}

struct ast_exp_list *
ast_new_exp_list(struct ast_exp *head, struct ast_exp_list *tail)
{
    struct ast_exp_list *p = malloc(sizeof(*p));
    check_mem(p);
    p->head = head;
    p->tail = tail;
    return p;
error:
    return NULL;
}

struct ast_stmt_list *
ast_new_stmt_list(struct ast_stmt *head, struct ast_stmt_list *tail)
{
    struct ast_stmt_list *p = malloc(sizeof(*p));
    check_mem(p);
    p->head = head;
    p->tail = tail;
    return p;
error:
    return NULL;
}

struct ast_field *
ast_new_field(struct s_symbol *name)
{
    struct ast_field *p = malloc(sizeof(*p));
    check_mem(p);
    p->pos = yylloc;
    p->name = name;
    return p;
error:
    return NULL;
}

struct ast_field_list *
ast_new_field_list(struct ast_field *head, struct ast_field_list *tail)
{
    struct ast_field_list *p = malloc(sizeof(*p));
    check_mem(p);
    p->head = head;
    p->tail = tail;
    return p;
error:
    return NULL;
}
