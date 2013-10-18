#ifndef AST_H_
#define AST_H_

#include "symbol.h"

typedef int A_pos;

struct A_program {
    char *name;
    struct A_vardecList *global_var_def_list;
    struct A_fundecList *func_def_list;
    struct A_stmtList *body;
};

enum A_oper {
    A_plusOp,
    A_minusOp,
    A_timesOp,
    A_negOp,
    A_EQ,
    A_LT,
};

struct A_vardec {
    A_pos pos;
    struct S_symbol *sym;
    struct A_exp *init;
};

struct A_vardecList {
    struct A_vardec *head;
    struct A_vardecList *tail;
};

struct A_fundec {
    A_pos pos;
    struct S_symbol *name;
    struct A_fieldList *params;
    struct A_vardecList *var;
    struct A_stmtList *body;
};

struct A_fundecList {
    struct A_fundec *head;
    struct A_fundecList *tail;
};

struct A_exp {
    enum {
        A_varExp,
        A_intExp,
        A_callExp,
        A_opExp,
    } kind;
    A_pos pos;
    union {
        struct S_symbol *var;
        int intt;
        struct {
            struct S_symbol *func;
            struct A_expList *args;
        } call;
        struct {
            enum A_oper oper;
            struct A_exp *left;
            struct A_exp *right;
        } op;
    } u;
};

struct A_expList {
    struct A_exp *head;
    struct A_expList *tail;
};

struct A_stmt {
    enum {
        A_upStmt,
        A_downStmt,
        A_moveStmt,
        A_readStmt,
        A_assignStmt,
        A_ifStmt,
        A_whileStmt,
        A_returnStmt,
        A_callStmt,
        A_expListStmt,
    } kind;
    A_pos pos;
    union {
        /* up; - needs only the pos */
        /* down; - needs only the pos */
        struct {
            struct A_exp *exp1;
            struct A_exp *exp2;
        } move;
        struct {
            struct S_symbol *var;
        } read;
        struct {
            struct S_symbol *var;
            struct A_exp *exp;
        } assign;
        struct {
            struct A_exp *test;
            struct A_stmtList *then;
            struct A_stmtList *elsee;
        } iff;
        struct {
            struct A_exp *test;
            struct A_stmtList *body;
        } whilee;
        struct {
            struct A_exp *exp;
        } returnn;
        struct {
            struct S_symbol *func;
            struct A_expList *args;
        } call;
        struct A_expList *seq;
    } u;
};

struct A_stmtList {
    struct A_stmt *head;
    struct A_stmtList *tail;
};

struct A_field {
    A_pos pos;
    struct S_symbol *name;
};

struct A_fieldList {
    struct A_field *head;
    struct A_fieldList *tail;
};


struct A_program *A_Program(char *program_name, struct A_vardecList *global_var_def_list, struct A_fundecList *func_def_list, struct A_stmtList *body);

//struct A_var *A_Var(A_pos pos, struct S_symbol *sym);

struct A_vardec *A_Vardec(A_pos pos, struct S_symbol *sym, struct A_exp *init);
struct A_vardecList *A_VardecList(struct A_vardec *head, struct A_vardecList *tail);

struct A_fundec *A_Fundec(A_pos pos, struct S_symbol *name, struct A_fieldList *params, struct A_vardecList *var, struct A_stmtList *body);
struct A_fundecList *A_FundecList(struct A_fundec *head, struct A_fundecList *tail);

struct A_exp *A_VarExp(A_pos pos, struct S_symbol *var);
struct A_exp *A_IntExp(A_pos pos, int i);
struct A_exp *A_CallExp(A_pos pos, struct S_symbol *func, struct A_expList *args);
struct A_exp *A_OpExp(A_pos pos, enum A_oper oper, struct A_exp *left, struct A_exp *right);
struct A_expList *A_ExpList(struct A_exp *head, struct A_expList *tail);

struct A_stmt *A_UpStmt(A_pos pos);
struct A_stmt *A_DownStmt(A_pos pos);
struct A_stmt *A_MoveStmt(A_pos pos, struct A_exp *exp1, struct A_exp *exp2);
struct A_stmt *A_ReadStmt(A_pos pos, struct S_symbol *var);
struct A_stmt *A_AssignStmt(A_pos pos, struct S_symbol *var, struct A_exp *exp);
struct A_stmt *A_IfStmt(A_pos pos, struct A_exp *test, struct A_stmtList *then, struct A_stmtList *elsee);
struct A_stmt *A_WhileStmt(A_pos pos, struct A_exp *test, struct A_stmtList *body);
struct A_stmt *A_ReturnStmt(A_pos pos, struct A_exp *exp);
struct A_stmt *A_CallStmt(A_pos pos, struct S_symbol *func, struct A_expList *args);
struct A_stmt *A_ExpListStmt(A_pos pos, struct A_expList *list);
struct A_stmtList *A_StmtList(struct A_stmt *head, struct A_stmtList *tail);

struct A_field *A_Field(A_pos pos, struct S_symbol *name);
struct A_fieldList *A_FieldList(struct A_field *head, struct A_fieldList *tail);

#endif /* end of include guard: AST_H_ */
