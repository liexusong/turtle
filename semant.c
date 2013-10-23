#include "global.h"
#include "semant.h"
#include "table.h"
#include "env.h"

#include "instruction.h"

struct patch {
    int lineno;
    struct env_entry *fun;
    struct patch *next;
};

static struct table *_venv;
static struct table *_fenv;

static struct patch *_patches;
static int retOffset;

static int count_fieldList(struct ast_field_list *list);
static void trans_global_vardecList(struct ast_var_dec_list *list);
static void trans_local_vardecList(struct ast_var_dec_list *list);
static void trans_func_def_list(struct ast_fun_dec_list *list);
static void trans_stmt_list(struct ast_stmt_list *list);
static void trans_stmt(struct ast_stmt *stmt);
static void trans_exp_list(struct ast_exp_list  *list);
static void trans_exp(struct ast_exp *exp);

static void link_func_calls(void);

static void
link_func_calls(void)
{
    struct patch *p;
    for (p = _patches; p != NULL; p = p->next) {
        backpatch(p->lineno, p->fun->index);
    }
}

static int
count_expList(struct ast_exp_list *list)
{
    int count = 0;
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
    int count = 0;
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
    int offset = 1;
    for (; list; list = list->tail, offset += 1) {
        struct ast_var_dec *dec = list->head;
        assert(dec != NULL);

        struct env_entry *entry = s_find(_venv, dec->sym);
        if (entry != NULL) {
            log_err("Redefining %s", s_name(dec->sym));
        }
        trans_exp(dec->init);

        s_enter(_venv, dec->sym, env_new_var(dec->sym, env_global, offset));
    }
}

static void
trans_local_vardecList(struct ast_var_dec_list *list)
{
    int offset = 1;
    for (; list; list = list->tail, offset += 1) {
        struct ast_var_dec *dec = list->head;
        assert(dec != NULL);

        struct env_entry *entry = s_find(_venv, dec->sym);
        if (entry != NULL && entry->u.var.scope == env_global) {
            log_err("Redefining %s", s_name(dec->sym));
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

    struct ast_fun_dec_list *p, *q;
    /**
     * Check for duplicate definitions
     */
    for (p = list; p; p = p->tail) {
        for (q = p->tail; q; q = q->tail) {
            if (p->head->name == q->head->name) {
                log_err("Redefining function");
            }
        }
    }

    log_info("Finished checking");

    for (p = list; p; p = p->tail) {
        s_enter(_fenv, p->head->name,
                env_new_fun(p->head->name, count_fieldList(p->head->params)));
        log_info("Added function %s with %d parameters", s_name(p->head->name), count_fieldList(p->head->params));
    }

    log_info("Finished adding symbol table entries");

    for (p = list; p; p = p->tail) {
        struct ast_field_list *params;
        int offset = -count_fieldList(p->head->params) - 1;
        retOffset = -count_fieldList(p->head->params) - 2;
        s_enter_scope(_venv);
        for (params = p->head->params; params;  params = params->tail, offset += 1) {
            struct env_entry *entry = s_find(_venv, params->head->name);
            if (entry != NULL && entry->u.var.scope == env_local) {
                log_err("Redefine");
                s_leave_scope(_venv);
                return;
            }
            s_enter(_venv, params->head->name, env_new_var(params->head->name, env_local, offset));
        }
        int addr = get_next_code_index();
        // TODO check this
        env_set_addr(_fenv, p->head->name, addr);
        trans_local_vardecList(p->head->var);
        trans_stmt_list(p->head->body);
        gen_Rts_Opt();
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
trans_stmt(struct ast_stmt *stmt)
{
    if (!stmt) {
        return;
    }

    struct env_entry *p;
    int l_then, l_else;
    int j_then, j_else;

    int l_begin, l_end;
    int j_begin, j_end;
    int j_test, l_test;

    int i;

    switch (stmt->kind) {
        case ast_upStmt:
            gen_Up();
            break;

        case ast_downStmt:
            gen_Down();
            break;

        case ast_moveStmt:
            trans_exp(stmt->u.move.exp1);
            trans_exp(stmt->u.move.exp2);
            gen_Move();
            break;

        case ast_readStmt:
            p = s_find(_venv, stmt->u.read.var);
            if (p == NULL)
            {
                log_err("Cannot read to undefined variable");
                break;
            }
            switch (p->u.var.scope) {
                case env_global:
                    gen_Read_GP(p->index);
                    break;

                case env_local:
                    gen_Read_FP(p->index);
                    break;
            }
            break;

        case ast_assignStmt:
            p = s_find(_venv, stmt->u.assign.var);
            if (p == NULL)
            {
                log_err("Cannot assign a value to the undefined variable");
                break;
            }
            trans_exp(stmt->u.assign.exp);
            switch (p->u.var.scope) {
                case env_global:
                    gen_Store_GP(p->index);
                    break;

                case env_local:
                    gen_Store_FP(p->index);
                    break;
            }

            break;

        case ast_ifStmt:

            if (stmt->u.iff.test->kind != ast_opExp ||
                stmt->u.iff.test->u.op.oper < ast_EQ) {
                log_err("error");
            }
            switch (stmt->u.iff.test->u.op.oper) {
                case ast_EQ:
                case ast_LT:
                    trans_exp(stmt->u.iff.test->u.op.left);
                    trans_exp(stmt->u.iff.test->u.op.right);
                    break;
                default:
                    log_err("Non");
                    break;
            }

            gen_Sub();
            gen_Test();
            gen_Pop(1);

            j_then = get_next_code_index();

            switch (stmt->u.iff.test->u.op.oper) {
                case ast_EQ:
                    gen_Jeq(0);
                    break;
                case ast_LT:
                    gen_Jlt(0);
                    break;
                default:
                    log_err("Non");
                    break;
            }
            j_else = get_next_code_index();
            gen_Jump(0);

            l_then = get_next_code_index();
            trans_stmt_list(stmt->u.iff.then);

            j_end = get_next_code_index();
            gen_Jump(0);

            l_else = get_next_code_index();
            trans_stmt_list(stmt->u.iff.elsee);

            l_end = get_next_code_index();

            backpatch(j_then, l_then);
            backpatch(j_else, l_else);
            backpatch(j_end, l_end);

            break;

        case ast_whileStmt:

            if (stmt->u.whilee.test->kind != ast_opExp ||
                stmt->u.whilee.test->u.op.oper < ast_EQ) {
                log_err("error");
                break;
            }

            l_test = get_next_code_index();
            switch (stmt->u.whilee.test->u.op.oper) {
                case ast_EQ:
                case ast_LT:
                    trans_exp(stmt->u.whilee.test->u.op.left);
                    trans_exp(stmt->u.whilee.test->u.op.right);
                    break;
                default:
                    log_err("Non");
                    break;
            }
            gen_Sub();
            gen_Test();
            gen_Pop(1);

            j_begin = get_next_code_index();
            switch (stmt->u.whilee.test->u.op.oper) {
                case ast_EQ:
                    gen_Jeq(0);
                    break;
                case ast_LT:
                    gen_Jlt(0);
                    break;
                default:
                    log_err("Non");
                    break;
            }

            j_end = get_next_code_index();
            gen_Jump(0);

            l_begin = get_next_code_index();
            trans_stmt_list(stmt->u.whilee.body);
            j_test = get_next_code_index();
            gen_Jump(0);
            l_end = get_next_code_index();

            backpatch(j_begin, l_begin);
            backpatch(j_end, l_end);
            backpatch(j_test, l_test);
            break;

        case ast_returnStmt:
            if (!s_in_scope()) {
                log_err("Not in frame");
            }
            trans_exp(stmt->u.returnn.exp);
            check(retOffset < 0, "Return from main");
            gen_Store_FP(retOffset);
            gen_Rts();
            break;

        case ast_callStmt:
            p = s_find(_fenv, stmt->u.call.func);
            if (p == NULL) {
                log_err("Cannot call undefined function");
                break;
            }

            if (count_expList(stmt->u.call.args) != p->u.func.count_params) {
                log_err("Mismatched number of parameters");
                break;
            }

            gen_Loadi(0);
            trans_exp_list(stmt->u.call.args);
            if (p->index != -1) {
                gen_Jsr(p->index);
            } else {
                i = get_next_code_index();
                gen_Jsr(0);

                struct patch *patch = malloc(sizeof *patch);
                check_mem(patch);
                patch->lineno = i;
                patch->fun = p;
                patch->next = _patches;
                _patches = patch;
            }
            gen_Pop(p->u.func.count_params + 1);

            break;

            // TODO doublecheck this one
        case ast_exp_listStmt:
            trans_exp_list(stmt->u.seq);
            break;
        default:
            assert(0);
    }

    // non-reacheable
error:
    return;
}

static void
trans_exp_list(struct ast_exp_list  *list)
{
    if (list == NULL) {
        log_info("exp_list returns");
        return;
    }

    for (; list; list = list->tail) {
        log_info("exp_list");
        trans_exp(list->head);
    }
}

static void
trans_exp(struct ast_exp *exp)
{
    if (!exp) {
        return;
    }
    struct env_entry *p;
    int i;
    switch (exp->kind) {
        case ast_varExp:
            p = s_find(_venv, exp->u.var);
            if (p == NULL) {
                log_err("Use of undefined varaible");
                break;
            }
            switch (p->u.var.scope) {
                case env_global:
                    gen_Load_GP(p->index);
                    break;
                case env_local:
                    gen_Load_FP(p->index);
                    break;
            }
            break;

        case ast_intExp:
            log_info("ast_IntExp: %d", exp->u.intt);
            gen_Loadi(exp->u.intt);
            break;

        case ast_callExp:
            p = s_find(_fenv, exp->u.call.func);
            if (p == NULL) {
                log_err("error");
            }

            if (count_expList(exp->u.call.args) != p->u.func.count_params) {
                log_err("Mismatched number of parameters");
                break;
            }

            gen_Loadi(0);
            trans_exp_list(exp->u.call.args);
            if (p->index != -1) {
                gen_Jsr(p->index);
            } else {
                i = get_next_code_index();
                gen_Jsr(0);

                struct patch *patch = malloc(sizeof *patch);
                check_mem(patch);
                patch->lineno = i;
                patch->fun = p;
                patch->next = _patches;
                _patches = patch;
            }
            gen_Pop(p->u.func.count_params);

            break;

        case ast_opExp:
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
                    log_err("error");
                    break;
            }
            break;

        default:
            assert(0);
    }

error:
    return;
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
    int j_jump = get_next_code_index();
    gen_Jump(0);

    log_info("trans_func_def_list() begins");
    trans_func_def_list(prog->func_def_list);
    log_info("trans_func_def_list() ends");
    link_func_calls();

    int l_jump = get_next_code_index();
    log_info("trans_stmt_list() begins");
    trans_stmt_list(prog->body);
    log_info("trans_stmt_list() ends");

    backpatch(j_jump, l_jump);
    gen_Halt();

    return;
}

