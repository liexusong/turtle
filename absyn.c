#include "global.h"
#include "absyn.h"
#include "dbg.h"
#include "parser.h"
#include "lexer.h"

struct A_program *
A_Program(char *program_name, struct A_vardecList *global_var_def_list,
          struct A_fundecList *func_def_list, struct A_stmtList *body)
{
    struct A_program *p = malloc(sizeof *p);
    check_mem(p);

    p->name = program_name;
    p->global_var_def_list = global_var_def_list;
    p->func_def_list = func_def_list;
    p->body = body;

    return p;

error:
    return NULL;
}

/*
struct A_var *
A_Var(A_pos pos, struct S_symbol *sym)
{
    struct A_var *p = malloc(sizeof *p);
    check_mem(p);

    p->pos = pos;
    p->sym = sym;

    return p;

error:
    return NULL;
}
*/

struct A_vardec *
A_Vardec(A_pos pos, struct S_symbol *name, struct A_exp *init)
{
    struct A_vardec *p = malloc(sizeof *p);
    check_mem(p);

    p->pos = pos;
    p->name = name;
    p->init = init;

    return p;

error:
    return NULL;
}

struct A_vardecList*
A_VardecList(struct A_vardec *head, struct A_vardecList *tail)
{
    struct A_vardecList *p = malloc(sizeof *p);
    check_mem(p);

    p->head = head;
    p->tail = tail;

    return p;

error:
    return NULL;
}

struct A_fundec *
A_Fundec(A_pos pos, struct S_symbol *name, struct A_fieldList *params, struct A_vardecList *var, struct A_stmtList *body)
{
    struct A_fundec *p = malloc(sizeof *p);
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

struct A_fundecList *
A_FundecList(struct A_fundec *head, struct A_fundecList *tail)
{
    struct A_fundecList *p = malloc(sizeof *p);
    check_mem(p);

    p->head = head;
    p->tail = tail;

    return p;

error:
    return NULL;
}

struct A_exp *
A_VarExp(A_pos pos, struct S_symbol *var)
{
    struct A_exp *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = A_varExp;
    p->pos = pos;
    p->u.var = var;

    return p;

error:
    return NULL;
}

struct A_exp *
A_IntExp(A_pos pos, int i)
{
    struct A_exp *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = A_intExp;
    p->pos = pos;
    p->u.intt = i;

    return p;

error:
    return NULL;
}

struct A_exp *
A_CallExp(A_pos pos, struct S_symbol *func, struct A_expList *args)
{
    struct A_exp *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = A_callExp;
    p->pos = pos;
    p->u.call.func = func;
    p->u.call.args = args;

    return p;

error:
    return NULL;
}

struct A_exp *
A_OpExp(A_pos pos, enum A_oper oper, struct A_exp *left, struct A_exp *right)
{
    struct A_exp *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = A_opExp;
    p->pos = pos;
    p->u.op.oper = oper;
    p->u.op.left = left;
    p->u.op.right = right;

    return p;

error:
    return NULL;
}


struct A_stmt *
A_UpStmt(A_pos pos)
{
    struct A_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = A_upStmt;
    p->pos = pos;

    return p;

error:
    return NULL;
}

struct A_stmt *
A_DownStmt(A_pos pos)
{
    struct A_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = A_downStmt;
    p->pos = pos;

    return p;

error:
    return NULL;
}


struct A_stmt *
A_MoveStmt(A_pos pos, struct A_exp *exp1, struct A_exp *exp2)
{
    struct A_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = A_moveStmt;
    p->pos = pos;
    p->u.move.exp1 = exp1;
    p->u.move.exp2 = exp2;

    return p;

error:
    return NULL;
}

struct A_stmt *
A_ReadStmt(A_pos pos, struct S_symbol *var)
{
    struct A_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = A_readStmt;
    p->pos = pos;
    p->u.read.var = var;
    return p;

error:
    return NULL;
}

struct A_stmt *
A_AssignStmt(A_pos pos, struct S_symbol *var, struct A_exp *exp)
{
    struct A_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = A_assignStmt;
    p->pos = pos;
    p->u.assign.var = var;
    p->u.assign.exp = exp;

    return p;

error:
    return NULL;
}

struct A_stmt *
A_IfStmt(A_pos pos, struct A_exp *test, struct A_stmtList *then, struct A_stmtList *elsee)
{
    struct A_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = A_ifStmt;
    p->pos = pos;
    p->u.iff.test = test;
    p->u.iff.then = then;
    p->u.iff.elsee = elsee;

    return p;

error:
    return NULL;
}

struct A_stmt *
A_WhileStmt(A_pos pos, struct A_exp *test, struct A_stmtList *body)
{
    struct A_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = A_whileStmt;
    p->pos = pos;
    p->u.whilee.test = test;
    p->u.whilee.body = body;

    return p;

error:
    return NULL;
}

struct A_stmt *
A_ReturnStmt(A_pos pos, struct A_exp *exp)
{
    struct A_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = A_returnStmt;
    p->pos = pos;
    p->u.returnn.exp = exp;

    return p;

error:
    return NULL;
}

struct A_stmt *
A_CallStmt(A_pos pos, struct S_symbol *func, struct A_expList *args)
{
    struct A_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = A_callStmt;
    p->pos = pos;
    p->u.call.func = func;
    p->u.call.args = args;

    return p;

error:
    return NULL;
}

struct A_stmt *
A_ExpListStmt(A_pos pos, struct A_expList *list)
{
    struct A_stmt *p = malloc(sizeof *p);
    check_mem(p);

    p->kind = A_expListStmt;
    p-> pos = pos;
    p->u.seq = list;

    return p;

error:
    return NULL;
}

struct A_expList *
A_ExpList(struct A_exp *head, struct A_expList *tail)
{
    struct A_expList *p = malloc(sizeof *p);
    check_mem(p);

    p->head = head;
    p->tail = tail;

    return p;

error:
    return NULL;

}

struct A_stmtList *
A_StmtList(struct A_stmt *head, struct A_stmtList *tail)
{
    struct A_stmtList *p = malloc(sizeof *p);
    check_mem(p);

    p->head = head;
    p->tail = tail;

    return p;

error:
    return NULL;

}

struct A_field *
A_Field(A_pos pos, struct S_symbol *name)
{
    struct A_field *p = malloc(sizeof *p);
    check_mem(p);

    p->pos = pos;
    p->name = name;

    return p;

error:
    return NULL;
}

struct A_fieldList *
A_FieldList(struct A_field *head, struct A_fieldList *tail)
{
    struct A_fieldList *p = malloc(sizeof *p);
    check_mem(p);

    p->head = head;
    p->tail = tail;

    return p;

error:
    return NULL;
}
