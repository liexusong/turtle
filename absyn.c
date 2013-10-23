#include "global.h"
#include "absyn.h"
#include "dbg.h"
#include "parser.h"
#include "lexer.h"

struct ast_program *
ast_new_program(char *program_name, struct ast_var_dec_list *global_var_def_list,
          struct ast_fun_dec_list *func_def_list, struct ast_stmt_list *body)
{
    struct ast_program *p = malloc(sizeof *p);
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
ast_new_var_dec(ast_pos pos, struct s_symbol *sym, struct ast_exp *init)
{
    struct ast_var_dec *p = malloc(sizeof *p);
    check_mem(p);

    p->pos = pos;
    p->sym = sym;
    p->init = init;
    log_info("ast_var_dec(%s)\n", s_name(sym));

    return p;

error:
    return NULL;
}

struct ast_var_dec_list*
ast_new_var_dec_list(struct ast_var_dec *head, struct ast_var_dec_list *tail)
{
    struct ast_var_dec_list *p = malloc(sizeof *p);
    check_mem(p);

    p->head = head;
    p->tail = tail;

    return p;

error:
    return NULL;
}

struct ast_fun_dec *
ast_new_fundec(ast_pos pos, struct s_symbol *name, struct ast_field_list *params,
               struct ast_var_dec_list *var, struct ast_stmt_list *body)
{
    struct ast_fun_dec *p = malloc(sizeof *p);
    check_mem(p);

    p->pos = pos;
    p->name = name;
    p->params = params;
    p->var = var;
    p->body = body;

    return p;

error:
    return NULL;
}

struct ast_fun_dec_list *
ast_new_fundec_list(struct ast_fun_dec *head, struct ast_fun_dec_list *tail)
{
    struct ast_fun_dec_list *p = malloc(sizeof *p);
    check_mem(p);

    p->head = head;
    p->tail = tail;

    return p;

error:
    return NULL;
}

struct ast_exp *
ast_new_var_exp(ast_pos pos, struct s_symbol *var)
{
    struct ast_exp *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = ast_varExp;
    p->pos = pos;
    p->u.var = var;

    return p;

error:
    return NULL;
}

struct ast_exp *
ast_int_exp(ast_pos pos, int i)
{
    struct ast_exp *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = ast_intExp;
    p->pos = pos;
    p->u.intt = i;

    return p;

error:
    return NULL;
}

struct ast_exp *
ast_new_call_exp(ast_pos pos, struct s_symbol *func, struct ast_exp_list *args)
{
    struct ast_exp *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = ast_callExp;
    p->pos = pos;
    p->u.call.func = func;
    p->u.call.args = args;

    return p;

error:
    return NULL;
}

struct ast_exp *
ast_new_op_exp(ast_pos pos, enum ast_oper oper, struct ast_exp *left, struct ast_exp *right)
{
    struct ast_exp *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = ast_opExp;
    p->pos = pos;
    p->u.op.oper = oper;
    p->u.op.left = left;
    p->u.op.right = right;

    return p;

error:
    return NULL;
}


struct ast_stmt *
ast_new_up_stmt(ast_pos pos)
{
    struct ast_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = ast_upStmt;
    p->pos = pos;

    return p;

error:
    return NULL;
}

struct ast_stmt *
ast_new_down_stmt(ast_pos pos)
{
    struct ast_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = ast_downStmt;
    p->pos = pos;

    return p;

error:
    return NULL;
}


struct ast_stmt *
ast_new_move_stmt(ast_pos pos, struct ast_exp *exp1, struct ast_exp *exp2)
{
    struct ast_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = ast_moveStmt;
    p->pos = pos;
    p->u.move.exp1 = exp1;
    p->u.move.exp2 = exp2;

    return p;

error:
    return NULL;
}

struct ast_stmt *
ast_new_read_stmt(ast_pos pos, struct s_symbol *var)
{
    struct ast_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = ast_readStmt;
    p->pos = pos;
    p->u.read.var = var;
    return p;

error:
    return NULL;
}

struct ast_stmt *
ast_new_assign_stmt(ast_pos pos, struct s_symbol *var, struct ast_exp *exp)
{
    struct ast_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = ast_assignStmt;
    p->pos = pos;
    p->u.assign.var = var;
    p->u.assign.exp = exp;

    return p;

error:
    return NULL;
}

struct ast_stmt *
ast_new_if_stmt(ast_pos pos, struct ast_exp *test, struct ast_stmt_list *then, struct ast_stmt_list *elsee)
{
    struct ast_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = ast_ifStmt;
    p->pos = pos;
    p->u.iff.test = test;
    p->u.iff.then = then;
    p->u.iff.elsee = elsee;

    return p;

error:
    return NULL;
}

struct ast_stmt *
ast_new_while_stmt(ast_pos pos, struct ast_exp *test, struct ast_stmt_list *body)
{
    struct ast_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = ast_whileStmt;
    p->pos = pos;
    p->u.whilee.test = test;
    p->u.whilee.body = body;

    return p;

error:
    return NULL;
}

struct ast_stmt *
ast_new_return_stmt(ast_pos pos, struct ast_exp *exp)
{
    struct ast_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = ast_returnStmt;
    p->pos = pos;
    p->u.returnn.exp = exp;

    return p;

error:
    return NULL;
}

struct ast_stmt *
ast_new_call_stmt(ast_pos pos, struct s_symbol *func, struct ast_exp_list *args)
{
    struct ast_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = ast_callStmt;
    p->pos = pos;
    p->u.call.func = func;
    p->u.call.args = args;

    return p;

error:
    return NULL;
}

struct ast_stmt *
ast_new_exp_list_stmt(ast_pos pos, struct ast_exp_list *list)
{
    struct ast_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = ast_exp_listStmt;
    p-> pos = pos;
    p->u.seq = list;

    return p;

error:
    return NULL;
}

struct ast_exp_list *
ast_new_exp_list(struct ast_exp *head, struct ast_exp_list *tail)
{
    struct ast_exp_list *p = malloc(sizeof *p);
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
    struct ast_stmt_list *p = malloc(sizeof *p);
    check_mem(p);

    p->head = head;
    p->tail = tail;

    return p;

error:
    return NULL;

}

struct ast_field *
ast_new_field(ast_pos pos, struct s_symbol *name)
{
    struct ast_field *p = malloc(sizeof *p);
    check_mem(p);

    p->pos = pos;
    p->name = name;

    return p;

error:
    return NULL;
}

struct ast_field_list *
ast_new_field_list(struct ast_field *head, struct ast_field_list *tail)
{
    struct ast_field_list *p = malloc(sizeof *p);
    check_mem(p);

    p->head = head;
    p->tail = tail;

    return p;

error:
    return NULL;
}

