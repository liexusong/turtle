#include "global.h"
#include "semant.h"
#include "hash.h"
#include "env.h"

#include "instruction.h"

struct patch {
    int lineno;
    struct env_entry *fun;
    struct patch *next;
};

static struct hash_table *_venv;
static struct hash_table *_fenv;

static struct patch *_patches;
static int retOffset;

static int count_fieldList(struct A_fieldList *list);
static void trans_global_vardecList(struct A_vardecList *list);
static void trans_local_vardecList(struct A_vardecList *list);
static void trans_func_def_list(struct A_fundecList *list);
static void trans_stmt_list(struct A_stmtList *list);
static void trans_stmt(struct A_stmt *stmt);
static void trans_exp_list(struct A_expList  *list);
static void trans_exp(struct A_exp *exp);

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
count_expList(struct A_expList *list)
{
    int count = 0;
    struct A_expList *p;
    for (p = list; p; p = p->tail) {
        if (p->head) {
            count += 1;
        }
    }
    return count;
}

static int
count_fieldList(struct A_fieldList *list)
{
    int count = 0;
    struct A_fieldList *p;
    for (p = list; p; p = p->tail) {
        if (p->head) {
            count += 1;
        }
    }
    return count;
}

static void
trans_global_vardecList(struct A_vardecList *list)
{
    int offset = 1;
    for (; list; list = list->tail, offset += 1) {
        struct A_vardec *dec = list->head;
        assert(dec != NULL);

        struct env_entry *entry = S_look(_venv, dec->sym);
        if (entry != NULL) {
            log_err("Redefining %s", S_name(dec->sym));
        }
        trans_exp(dec->init);

        S_enter(_venv, dec->sym, E_VarEntry(dec->sym, E_Global, offset));
    }
}

static void
trans_local_vardecList(struct A_vardecList *list)
{
    int offset = 1;
    for (; list; list = list->tail, offset += 1) {
        struct A_vardec *dec = list->head;
        assert(dec != NULL);

        struct env_entry *entry = S_look(_venv, dec->sym);
        if (entry != NULL && entry->u.var.scope == E_Local) {
            log_err("Redefining %s", S_name(dec->sym));
        }
        trans_exp(dec->init);

        S_enter(_venv, dec->sym, E_VarEntry(dec->sym, E_Local, offset));
    }
}

static void
trans_func_def_list(struct A_fundecList *list)
{
    if (!list) {
        return;
    }

    struct A_fundecList *p, *q;
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
        S_enter(_fenv, p->head->name,
                E_FunEntry(p->head->name, count_fieldList(p->head->params)));
        log_info("Added function %s with %d parameters", S_name(p->head->name), count_fieldList(p->head->params));
    }

    log_info("Finished adding symbol table entries");

    for (p = list; p; p = p->tail) {
        struct A_fieldList *params;
        int offset = -count_fieldList(p->head->params) - 1;
        retOffset = -count_fieldList(p->head->params) - 2;
        S_beginScope(_venv);
        for (params = p->head->params; params;  params = params->tail, offset += 1) {
            struct env_entry *entry = S_look(_venv, params->head->name);
            if (entry != NULL && entry->u.var.scope == E_Local) {
                log_err("Redefine");
                S_endScope(_venv);
                return;
            }
            S_enter(_venv, params->head->name, E_VarEntry(params->head->name, E_Local, offset));
        }
        int addr = get_next_code_index();
        // TODO check this
        E_Set_Addr(_fenv, p->head->name, addr);
        trans_local_vardecList(p->head->var);
        trans_stmt_list(p->head->body);
        gen_Rts_Opt();
        S_endScope(_venv);
        retOffset = 0;
    }
}

static void
trans_stmt_list(struct A_stmtList *list)
{
    for (; list; list = list->tail) {
        trans_stmt(list->head);
    }
}

static void
trans_stmt(struct A_stmt *stmt)
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
        case A_upStmt:
            gen_Up();
            break;

        case A_downStmt:
            gen_Down();
            break;

        case A_moveStmt:
            trans_exp(stmt->u.move.exp1);
            trans_exp(stmt->u.move.exp2);
            gen_Move();
            break;

        case A_readStmt:
            p = S_look(_venv, stmt->u.read.var);
            if (p == NULL)
            {
                log_err("Cannot read to undefined variable");
                break;
            }
            switch (p->u.var.scope) {
                case E_Global:
                    gen_Read_GP(p->index);
                    break;

                case E_Local:
                    gen_Read_FP(p->index);
                    break;
            }
            break;

        case A_assignStmt:
            p = S_look(_venv, stmt->u.assign.var);
            if (p == NULL)
            {
                log_err("Cannot assign a value to the undefined variable");
                break;
            }
            trans_exp(stmt->u.assign.exp);
            switch (p->u.var.scope) {
                case E_Global:
                    gen_Store_GP(p->index);
                    break;

                case E_Local:
                    gen_Store_FP(p->index);
                    break;
            }

            break;

        case A_ifStmt:

            if (stmt->u.iff.test->kind != A_opExp ||
                stmt->u.iff.test->u.op.oper < A_EQ) {
                log_err("error");
            }
            switch (stmt->u.iff.test->u.op.oper) {
                case A_EQ:
                case A_LT:
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

            l_then = get_next_code_index() + 4;

            switch (stmt->u.iff.test->u.op.oper) {
                case A_EQ:
                    gen_Jeq(l_then);
                    break;
                case A_LT:
                    gen_Jlt(l_then);
                    break;
                default:
                    log_err("Non");
                    break;
            }
            j_else = get_next_code_index();
            gen_Jump(0);
            trans_stmt_list(stmt->u.iff.then);

            if (stmt->u.iff.elsee) {
                j_end = get_next_code_index();
                gen_Jump(0);
            }

            l_else = get_next_code_index();
            trans_stmt_list(stmt->u.iff.elsee);

            l_end = get_next_code_index();

            backpatch(j_else, l_else);
            if (stmt->u.iff.elsee) {
                backpatch(j_end, l_end);
            }

            break;

        case A_whileStmt:

            if (stmt->u.whilee.test->kind != A_opExp ||
                stmt->u.whilee.test->u.op.oper < A_EQ) {
                log_err("error");
                break;
            }

            l_test = get_next_code_index();
            switch (stmt->u.whilee.test->u.op.oper) {
                case A_EQ:
                case A_LT:
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
                case A_EQ:
                    gen_Jeq(0);
                    break;
                case A_LT:
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

        case A_returnStmt:
            if (!in_frame) {
                log_err("Not in frame");
            }
            trans_exp(stmt->u.returnn.exp);
            if (retOffset < 0) {
                yyerror("Return from main");
                break;
            }
            gen_Store_FP(retOffset);
            gen_Rts();
            break;

        case A_callStmt:
            p = S_look(_fenv, stmt->u.call.func);
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
        case A_expListStmt:
            trans_exp_list(stmt->u.seq);
            break;
    }

    // non-reacheable
error:
    return;
}

static void
trans_exp_list(struct A_expList  *list)
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
trans_exp(struct A_exp *exp)
{
    if (!exp) {
        return;
    }
    struct env_entry *p;
    int i;
    switch (exp->kind) {
        case A_varExp:
            p = S_look(_venv, exp->u.var);
            if (p == NULL) {
                log_err("Use of undefined varaible");
                break;
            }
            switch (p->u.var.scope) {
                case E_Global:
                    gen_Load_GP(p->index);
                    break;
                case E_Local:
                    gen_Load_FP(p->index);
                    break;
            }
            break;

        case A_intExp:
            log_info("A_IntExp: %d", exp->u.intt);
            gen_Loadi(exp->u.intt);
            break;

        case A_callExp:
            p = S_look(_fenv, exp->u.call.func);
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

        case A_opExp:
            trans_exp(exp->u.op.left);
            trans_exp(exp->u.op.right);
            switch (exp->u.op.oper) {
                case A_plusOp:
                    gen_Add();
                    break;
                case A_minusOp:
                    gen_Sub();
                    break;
                case A_timesOp:
                    gen_Mul();
                    break;
                case A_negOp:
                    gen_Neg();
                    break;
                default:
                    log_err("error");
                    break;
            }
            break;
    }

error:
    return;
}


void
sem_trans_prog(struct A_program *prog)
{
    if (!prog) {
        return;
    }

    _venv = E_base_venv();
    _fenv = E_base_fenv();
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

