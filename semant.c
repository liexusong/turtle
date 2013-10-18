#include "semant.h"
#include "hash.h"

static hash_table *_venv;
static hash_table *_fenv;


static int
defined(struct hash_table *env, struct S_symbol *sym)
{
    return S_look(env, sym) != NULL;
}

void
trans_vardecList(struct A_vardecList *list)
{
    for (; list; list = list->tail) {
        trans_GVar_dec(list->head);
    }
}

void
trans_vardec(struct A_vardec *dec)
{
    if (!list) {
        return;
    }
    struct env_entry *entry = S_look(_venv, dec->sym);
    if (entry) {
        log_err("Redefining %s", S_name(dec->sym));
    }

    sym_enter(_venv, dec->u.sym, E_VarEntry(dec->u.sym));

error:
}

int
count_fieldList(struct A_fieldList *list)
{
    int count = 0;
    struct A_fieldList *p;
    for (p = list; p; p = list->tail) {
        if (p->head) {
            count += 1;
        }
    }
    return count;
}

void
trans_func_def_list(struct A_fundecList *list)
{
    if (!list) {
        return;
    }

    struct A_fundec *p, *q;
    for (p = list; p; p = p->tail) {
        for (q = p->tail; q; q = q->tail) {
            if (p->head->name == q->head->name) {
                log_err("Redefining function");
            }
        }
    }

    for (p = list; p; p = p->next) {
        sym_enter(_fenv, p->head->name, E_FunEntry(count_fieldList(p->head->params)));
    }


    for (p = list; p; p = p->next) {
        struct A_fieldList *q;

        S_beginScope(_venv);
        for (; q; q  = q->tail) {
            sym_enter(_venv, q->name, NULL);
        }
        trans_vardecList(p->head->var);
        trans_stmt_list(p->head->body);
        S_endScope(_venv);
    }

error:
}

void
trans_stmt_list(struct A_stmtList *list)
{
    for (; list; list = list->tail) {
        trans_stmt(list->head);
    }
}

void
transStmt(struct A_stmt *stmt)
{
    if (!stmt) {
        return;
    }

    switch (stmt->kind) {
        case A_upStmt:
        case A_downStmt:
        case A_moveStmt:
            trans_exp(stmt->move->exp1);
            trans_exp(stmt->move->exp2);
            break;

        case A_readStmt:
            if (!s_look(_venv, stmt->u.read.var))
            {
                log_err("Cannot read to some");
            }
            break;

        case A_assignStmt:
            if (!s_look(_venv, stmt->u.assign.var))
            {
                log_err("Cannot read to some");
            }
            trans_exp(stmt->assign->exp);

            break;

        case A_ifStmt:

            trans_exp(stmt->iff->test);
            trans_stmt_list(stmt->iff->then);
            trans_stmt_list(stmt->iff->elsee);
            break;

        case A_whileStmt:

            trans_exp(stmt->whilee->test);
            trans_stmt_list(stmt->whilee->body);
            break;

        case A_returnStmt:

            trans_exp(stmt->assign->exp);
            break;

        case A_callStmt:
            if (!s_look(_fenv, stmt->u.call.func))
            {
                log_err("Cannot call undefined");
            }

            trans_exp_list(stmt->call->args);
            break;

        case A_expListStmt:
            trans_exp_list(stmt->u.seq);
            break;
    }

    // non-reacheable

error:
    return;
}

void sem_trans_prog(struct A_program *prog)
{
    if (!prog) {
        return;
    }

    _venv = E_base_venv();
    _fenv = E_base_fenv();

    trans_vardecList(prog->global_var_def_list);
    trans_func_def_list(prog->func_def_list);
    trans_stmt_list(prog->body);

    return;
}

void
trans_exp(struct A_exp *exp)
{
    if (!exp) {
        return;
    }
    switch (exp->kind) {
        case A_varExp:
            if (!defined(_venv, exp->u.var)) {
                log_err("error");
            }

            break;

        case A_intExp:
            break;

        case A_callExp:
            if (!defined(_fenv, exp->u.call.func)) {
                log_err("error");
            }
            trans_exp_list(exp-u.call.args);

            break;

        case A_opExp:
            trans_exp(exp->u.op.left);
            trans_exp(exp->u.op.right);
            break;
    }
}

void
trans_exp_list(struct A_expList  *list)
{
    if (list) {
        return;
    }

    for (; list; list = list->tail) {
        trans_exp(list->head);
    }
}
