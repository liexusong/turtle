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
#include "semant.h"
#include "table.h"
#include "env.h"

#include "instruction.h"


static struct table *_venv;
static struct table *_fenv;

struct patch {
    int             lineno;
    struct env_entry *fun;
    struct patch   *next;
};
static struct patch *_patches;
static int      retOffset;

static int      count_fieldList(struct ast_field_list *list);
static void     trans_global_vardecList(struct ast_var_dec_list *list);
static void     trans_local_vardecList(struct ast_var_dec_list *list);
static void     trans_func_def_list(struct ast_fun_dec_list *list);
static void     trans_stmt_list(struct ast_stmt_list *list);
static void     trans_stmt(struct ast_stmt *stmt);
static void     trans_exp_list(struct ast_exp_list *list);
static void     trans_exp(struct ast_exp *exp);

static void     link_func_calls(void);

static void trans_ast_upStmt(struct ast_stmt *stmt);
static void trans_ast_downStmt(struct ast_stmt *stmt);
static void trans_ast_moveStmt(struct ast_stmt *stmt);
static void trans_ast_readStmt(struct ast_stmt *stmt);
static void trans_ast_assignStmt(struct ast_stmt *stmt);
static void trans_ast_iftStmt(struct ast_stmt *stmt);
static void trans_ast_ifteStmt(struct ast_stmt *stmt);
static void trans_ast_whileStmt(struct ast_stmt *stmt);
static void trans_ast_returnStmt(struct ast_stmt *stmt);
static void trans_ast_callStmt(struct ast_stmt *stmt);
static void trans_ast_exp_listStmt(struct ast_stmt *stmt);

static void (*trans_stmt_fun_list[])(struct ast_stmt *stmt) = {
    trans_ast_upStmt,
    trans_ast_downStmt,
    trans_ast_moveStmt,
    trans_ast_readStmt,
    trans_ast_assignStmt,
    trans_ast_iftStmt,
    trans_ast_ifteStmt,
    trans_ast_whileStmt,
    trans_ast_returnStmt,
    trans_ast_callStmt,
    trans_ast_exp_listStmt,
};

static void trans_stmt(struct ast_stmt *stmt);
static void trans_exp_list(struct ast_exp_list *list);
static void trans_var_exp(struct ast_exp *exp);
static void trans_int_exp(struct ast_exp *exp);
static void trans_call_exp(struct ast_exp *exp);
static void trans_op_exp(struct ast_exp *exp);

static void (*trans_exp_fun_list[])(struct ast_exp *exp) = {
    trans_var_exp,
    trans_int_exp,
    trans_call_exp,
    trans_op_exp,
};

static void
link_func_calls(void)
{
    struct patch   *p;

    for (p = _patches; p != NULL; p = p->next) {
        backpatch(p->lineno, p->fun->index);
    }
}

static int
count_expList(struct ast_exp_list *list)
{
    int             count = 0;
    struct ast_exp_list *p;

    for (p = list; p; p = p->tail) {
        if (p->head) {
            count += 1;
        }
    }

    return count;
}

static int
count_fieldList(struct ast_field_list *list)
{
    int             count = 0;
    struct ast_field_list *p;

    for (p = list; p; p = p->tail) {
        if (p->head) {
            count += 1;
        }
    }

    return count;
}

static void
trans_global_vardecList(struct ast_var_dec_list *list)
{
    int             offset = 1;

    for (; list; list = list->tail, offset += 1) {
        struct ast_var_dec *dec = list->head;
        assert(dec != NULL);
        struct env_entry *entry = s_find(_venv, dec->sym);

        if (entry != NULL) {
            log_err("Redefining %s", s_name(dec->sym));
            panic();
        }

        trans_exp(dec->init);
        s_enter(_venv, dec->sym,
                env_new_var(dec->sym, env_global, offset));
    }
}

static void
trans_local_vardecList(struct ast_var_dec_list *list)
{
    int             offset = 1;

    for (; list; list = list->tail, offset += 1) {
        struct ast_var_dec *dec = list->head;
        assert(dec != NULL);
        struct env_entry *entry = s_find(_venv, dec->sym);

        if (entry != NULL && entry->u.var.scope == env_global) {
            log_err("Redefining %s", s_name(dec->sym));
            panic();
        }

        trans_exp(dec->init);
        s_enter(_venv, dec->sym, env_new_var(dec->sym, env_local, offset));
    }
}

static void
trans_func_def_list(struct ast_fun_dec_list *list)
{
    if (!list) {
        return;
    }

    struct ast_fun_dec_list *p,
            *q;
    log_info("trans_func_def_list() starts");

    /**
     * Pass 1: check for duplicate definitions
     */
    for (p = list; p; p = p->tail) {
        for (q = p->tail; q; q = q->tail) {
            if (p->head->name == q->head->name) {
                log_err("Redefining function");
                panic();
            }
        }
    }

    log_info("trans_func_def_list() pass 1 finished");

    /**
     * Pass 2: insert the symbol table entry for functions to the symbol table
     *
     * Only name and the number of parameters are filled.
     */
    for (p = list; p; p = p->tail) {
        s_enter(_fenv, p->head->name,
                env_new_fun(p->head->name,
                            count_fieldList(p->head->params)));
        log_info("Added function %s with %d parameters",
                 s_name(p->head->name), count_fieldList(p->head->params));
    }

    /**
     * Pass 3: parse the function body and fill the address in the symbol table
     */
    for (p = list; p; p = p->tail) {
        struct ast_field_list *params;
        int             offset = -count_fieldList(p->head->params) - 1;
        retOffset = -count_fieldList(p->head->params) - 2;
        s_enter_scope(_venv);

        for (params = p->head->params; params;
                params = params->tail, offset += 1) {
            struct env_entry *entry = s_find(_venv, params->head->name);

            if (entry != NULL && entry->u.var.scope == env_local) {
                log_err("Redefine");
                panic();
            }

            s_enter(_venv, params->head->name,
                    env_new_var(params->head->name, env_local, offset));
        }

        int             addr = get_next_code_index();
        // TODO check this
        env_set_addr(_fenv, p->head->name, addr);
        trans_local_vardecList(p->head->var);
        trans_stmt_list(p->head->body);
        gen_Rts();
        s_leave_scope(_venv);
        retOffset = 0;
    }
}

static void
trans_stmt_list(struct ast_stmt_list *list)
{
    for (; list; list = list->tail) {
        trans_stmt(list->head);
    }
}

static void
trans_ast_upStmt(struct ast_stmt *stmt)
{
    assert(stmt && stmt->kind == ast_upStmt);
    gen_Up();
}

static void
trans_ast_downStmt(struct ast_stmt *stmt)
{
    assert(stmt && stmt->kind == ast_downStmt);
    gen_Down();
}


static void
trans_ast_moveStmt(struct ast_stmt *stmt)
{
    assert(stmt && stmt->kind == ast_moveStmt);
    trans_exp(stmt->u.move.exp1);
    trans_exp(stmt->u.move.exp2);
    gen_Move();
}

static void
trans_ast_readStmt(struct ast_stmt *stmt)
{
    assert(stmt && stmt->kind == ast_readStmt);
    struct env_entry *p = s_find(_venv, stmt->u.read.var);

    if (!p) {
        log_err("Cannot read to undefined variable");
        panic();
    }

    switch (p->u.var.scope) {
    case env_global:
        gen_Read_GP(p->index);
        break;

    case env_local:
        gen_Read_FP(p->index);
        break;

    default:
        log_err("Unkown type");
        panic();
    }
}

static void
trans_ast_assignStmt(struct ast_stmt *stmt)
{
    assert(stmt && stmt->kind == ast_assignStmt);
    struct env_entry *p = s_find(_venv, stmt->u.assign.var);

    if (!p) {
        log_err("Cannot assign a value to the undefined variable");
        panic();
    }

    trans_exp(stmt->u.assign.exp);

    switch (p->u.var.scope) {
    case env_global:
        gen_Store_GP(p->index);
        break;

    case env_local:
        gen_Store_FP(p->index);
        break;

    default:
        log_err("Unkown");
        panic();
    }
}


static struct ast_stmt*
transform_iftStmt(struct ast_stmt *stmt)
{
    assert(stmt && stmt->kind == ast_iftStmt);
    struct ast_exp *test = stmt->u.ift.test;
    struct ast_stmt_list *then = stmt->u.ift.then;
    struct ast_exp *left = stmt->u.ift.test->u.op.left;
    struct ast_exp *right = stmt->u.ift.test->u.op.right;
    struct ast_exp *test1, *test2;

    switch(stmt->u.ift.test->u.op.oper) {
    case ast_EQ:
    case ast_LT:
        return stmt;

    case ast_NEQ:
        test->u.op.oper = ast_EQ;
        return ast_new_ifte_stmt(test, NULL, then);

    case ast_GT:
        stmt->u.ift.test = ast_new_op_exp(ast_LT, right, left);
        return stmt;

    case ast_LEQ:
        test1 = ast_new_op_exp(ast_LT, left, right);
        test2 = ast_new_op_exp(ast_EQ, left, right);
        return ast_new_ifte_stmt(test1, then, ast_new_stmt_list(ast_new_ifte_stmt(test2, then, NULL), NULL));

    case ast_GEQ:
        stmt->u.ift.test = ast_new_op_exp(ast_LEQ, right, left);
        return transform_iftStmt(stmt);

    default:
        panic();
    }
    sentinel("Should not reach");
error:
    return NULL;
}

static void
trans_ast_iftStmt(struct ast_stmt *stmt)
{
    assert(stmt && stmt->kind == ast_iftStmt);

    if (stmt->u.ift.test->kind != ast_opExp ||
            stmt->u.ift.test->u.op.oper < ast_EQ) {
        log_err("error");
        panic();
    }

    struct ast_stmt *s = transform_iftStmt(stmt);
    if (s != stmt) {
        trans_stmt(s);
        return;
    }

    struct ast_exp *left = stmt->u.ift.test->u.op.left;
    struct ast_exp *right = stmt->u.ift.test->u.op.right;

    switch (stmt->u.ift.test->u.op.oper) {
    case ast_EQ:
    case ast_LT:
        trans_exp(left);
        trans_exp(right);
        break;

    default:
        log_err("Non supported");
        panic();
    }

    gen_Sub();
    gen_Test();
    gen_Pop(1);
    int j_then = get_next_code_index();

    switch (stmt->u.ift.test->u.op.oper) {
    case ast_EQ:
        gen_Jeq(0);
        break;

    case ast_LT:
        gen_Jlt(0);
        break;

    default:
        log_err("Non");
        panic();
        break;
    }

    int j_end = get_next_code_index();
    gen_Jump(0);
    int l_then = get_next_code_index();
    trans_stmt_list(stmt->u.ift.then);
    int l_end = get_next_code_index();
    backpatch(j_then, l_then);
    backpatch(j_end, l_end);
}

static struct ast_stmt*
transform_ifteStmt(struct ast_stmt *stmt)
{
    assert(stmt && stmt->kind == ast_ifteStmt);
    struct ast_exp *test = stmt->u.ifte.test;
    struct ast_stmt_list *then = stmt->u.ifte.then;
    struct ast_stmt_list *elsee = stmt->u.ifte.elsee;
    struct ast_exp *left = stmt->u.ifte.test->u.op.left;
    struct ast_exp *right = stmt->u.ifte.test->u.op.right;
    struct ast_exp *test1, *test2;

    switch(stmt->u.ifte.test->u.op.oper) {
    case ast_EQ:
    case ast_LT:
        return stmt;

    case ast_NEQ:
        test->u.op.oper = ast_EQ;
        return ast_new_ifte_stmt(test, elsee, then);

    case ast_GT:
        stmt->u.ifte.test = ast_new_op_exp(ast_LT, right, left);
        return stmt;

    case ast_LEQ:
        test1 = ast_new_op_exp(ast_LT, left, right);
        test2 = ast_new_op_exp(ast_EQ, left, right);
        return ast_new_ifte_stmt(test1, then, ast_new_stmt_list(ast_new_ifte_stmt(test2, then, elsee), NULL));

    case ast_GEQ:
        stmt->u.ifte.test = ast_new_op_exp(ast_LEQ, right, left);
        return transform_ifteStmt(stmt);

    default:
        panic();
    }
    sentinel("Should not reach");
error:
    return NULL;
}



static void
trans_ast_ifteStmt(struct ast_stmt *stmt)
{
    assert(stmt && stmt->kind == ast_ifteStmt);

    struct ast_stmt *s = transform_ifteStmt(stmt);
    if (s != stmt) {
        trans_stmt(s);
        return;
    }

    if (stmt->u.ifte.test->kind != ast_opExp ||
            stmt->u.ifte.test->u.op.oper < ast_EQ) {
        log_err("error");
        panic();
    }

    switch (stmt->u.ifte.test->u.op.oper) {
    case ast_EQ:
    case ast_LT:
        trans_exp(stmt->u.ifte.test->u.op.left);
        trans_exp(stmt->u.ifte.test->u.op.right);
        break;

    default:
        log_err("Non");
        panic();
    }

    gen_Sub();
    gen_Test();
    gen_Pop(1);
    int j_then = get_next_code_index();

    switch (stmt->u.ifte.test->u.op.oper) {
    case ast_EQ:
        gen_Jeq(0);
        break;

    case ast_LT:
        gen_Jlt(0);
        break;

    default:
        log_err("Non");
        panic();
        break;
    }

    int j_else = get_next_code_index();
    gen_Jump(0);
    int l_then = get_next_code_index();
    trans_stmt_list(stmt->u.ifte.then);
    int j_end = get_next_code_index();
    gen_Jump(0);
    int l_else = get_next_code_index();
    trans_stmt_list(stmt->u.ifte.elsee);
    int l_end = get_next_code_index();
    backpatch(j_then, l_then);
    backpatch(j_else, l_else);
    backpatch(j_end, l_end);
}

static void
trans_ast_whileStmt(struct ast_stmt *stmt)
{
    assert(stmt && stmt->kind == ast_whileStmt);

    if (stmt->u.whilee.test->kind != ast_opExp ||
            stmt->u.whilee.test->u.op.oper < ast_EQ) {
        log_err("error");
        panic();
    }

    int l_test = get_next_code_index();

    switch (stmt->u.whilee.test->u.op.oper) {
    case ast_EQ:
    case ast_LT:
        trans_exp(stmt->u.whilee.test->u.op.left);
        trans_exp(stmt->u.whilee.test->u.op.right);
        break;

    default:
        log_err("Non");
        panic();
        break;
    }

    gen_Sub();
    gen_Test();
    gen_Pop(1);
    int j_begin = get_next_code_index();

    switch (stmt->u.whilee.test->u.op.oper) {
    case ast_EQ:
        gen_Jeq(0);
        break;

    case ast_LT:
        gen_Jlt(0);
        break;

    default:
        log_err("Non");
        panic();
        break;
    }

    int j_end = get_next_code_index();
    gen_Jump(0);
    int l_begin = get_next_code_index();
    trans_stmt_list(stmt->u.whilee.body);
    int j_test = get_next_code_index();
    gen_Jump(0);
    int l_end = get_next_code_index();
    backpatch(j_begin, l_begin);
    backpatch(j_end, l_end);
    backpatch(j_test, l_test);
}

static void
trans_ast_returnStmt(struct ast_stmt *stmt)
{
    assert(stmt && stmt->kind == ast_returnStmt);

    if (!s_in_scope()) {
        log_info("Return from the outmost scope");
        panic();
    }

    trans_exp(stmt->u.returnn.exp);
    assert(retOffset < 0);
    gen_Store_FP(retOffset);
    gen_Rts();
}

static void
trans_ast_callStmt(struct ast_stmt *stmt)
{
    assert(stmt && stmt->kind == ast_callStmt);
    struct env_entry *p = s_find(_fenv, stmt->u.call.func);

    if (!p) {
        log_err("Cannot call undefined function");
        panic();
    }

    if (count_expList(stmt->u.call.args) != p->u.func.count_params) {
        log_err("Mismatched number of parameters");
        panic();
    }

    gen_Loadi(0);
    trans_exp_list(stmt->u.call.args);

    if (p->index != 0) {
        gen_Jsr(p->index);
    } else {
        int i = get_next_code_index();
        gen_Jsr(0);
        struct patch   *patch = malloc(sizeof(*patch));
        check_mem(patch);
        patch->lineno = i;
        patch->fun = p;
        patch->next = _patches;
        _patches = patch;
    }

#ifdef SANITY
    gen_Pop(p->u.func.count_params + 1);
#else
    gen_Pop(p->u.func.count_params);
#endif
    return;
error:
    panic();
}

static void
trans_ast_exp_listStmt(struct ast_stmt *stmt)
{
    assert(stmt && stmt->kind == ast_exp_listStmt);
    trans_exp_list(stmt->u.seq);
}

static void
trans_stmt(struct ast_stmt *stmt)
{
    if (!stmt) {
        return;
    } else if (stmt->kind <= ast_exp_listStmt) {
        (*trans_stmt_fun_list[stmt->kind])(stmt);
    } else {
        log_err("Unkown");
        panic();
    }
}

static void
trans_exp_list(struct ast_exp_list *list)
{
    if (!list) {
        log_info("exp_list returns");
        return;
    }

    for (; list; list = list->tail) {
        log_info("exp_list");
        trans_exp(list->head);
    }
}

static void trans_var_exp(struct ast_exp *exp)
{
    assert(exp && exp->kind == ast_varExp);
    struct env_entry *p = s_find(_venv, exp->u.var);

    if (!p) {
        log_err("Use of undefined varaible");
        panic();
    }

    switch (p->u.var.scope) {
    case env_global:
        gen_Load_GP(p->index);
        break;

    case env_local:
        gen_Load_FP(p->index);
        break;

    default:
        log_err("Unkown parameters");
        panic();
    }
}

static void
trans_int_exp(struct ast_exp *exp)
{
    assert (exp && exp->kind == ast_intExp);
    gen_Loadi(exp->u.intt);
}

static void
trans_call_exp(struct ast_exp *exp)
{
    assert(exp && exp->kind == ast_callExp);
    struct env_entry *p = s_find(_fenv, exp->u.call.func);

    if (!p) {
        log_err("Calling undefined function.");
        panic();
    }

    if (count_expList(exp->u.call.args) != p->u.func.count_params) {
        log_err("Mismatched number of parameters");
        panic();
    }

    gen_Loadi(0);
    trans_exp_list(exp->u.call.args);

    if (p->index != 0) {
        gen_Jsr(p->index);
    } else {
        int i = get_next_code_index();
        gen_Jsr(0);
        struct patch   *patch = malloc(sizeof(*patch));
        check_mem(patch);
        patch->lineno = i;
        patch->fun = p;
        patch->next = _patches;
        _patches = patch;
    }

    gen_Pop(p->u.func.count_params);
    return;
error:
    panic();
}

static void
trans_op_exp(struct ast_exp *exp)
{
    assert(exp && exp->kind == ast_opExp);
    trans_exp(exp->u.op.left);
    trans_exp(exp->u.op.right);

    switch (exp->u.op.oper) {
    case ast_plusOp:
        gen_Add();
        break;

    case ast_minusOp:
        gen_Sub();
        break;

    case ast_timesOp:
        gen_Mul();
        break;

    case ast_negOp:
        gen_Neg();
        break;

    default:
        log_err("Unkown parameter");
        panic();
    }
}

static void
trans_exp(struct ast_exp *exp)
{
    if (!exp) {
        return;
    } else if (exp->kind <= ast_opExp) {
        (*trans_exp_fun_list[exp->kind])(exp);
    } else {
        log_err("Unkown type");
        panic();
    }
}

void
sem_trans_prog(struct ast_program *prog)
{
    if (!prog) {
        return;
    }

    _venv = env_base_venv();
    _fenv = env_base_fenv();
    retOffset = 0;
    log_info("trans_global_vardecList() begins");
    trans_global_vardecList(prog->global_var_def_list);
    log_info("trans_global_vardecList() ends");
    int             j_jump = get_next_code_index();
    gen_Jump(0);
    log_info("trans_func_def_list() begins");
    trans_func_def_list(prog->func_def_list);
    log_info("trans_func_def_list() ends");
    link_func_calls();
    int             l_jump = get_next_code_index();
    log_info("trans_stmt_list() begins");
    trans_stmt_list(prog->body);
    log_info("trans_stmt_list() ends");
    backpatch(j_jump, l_jump);
    gen_Halt();
    return;
}
