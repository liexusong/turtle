%{
#include "absyn.h"
#include "global.h"
#include "semant.h"
#include "lexer.h"
#define TODO_NUM 0
%}

%output "parser.c"
%defines "parser.h"
%locations

%union{
    struct A_program *a_program;
    struct A_VAR *a_var;

    struct A_vardec *a_vardec;
    struct A_vardecList *a_vardeclist;

    struct A_fundec *a_fundec;
    struct A_fundecList *a_fundeclist;

    struct A_exp *a_exp;
    struct A_expList *a_explist;

    struct A_stmt *a_stmt;
    struct A_stmtList *a_stmtlist;

    struct A_field *a_field;
    struct A_fieldList *a_fieldlist;

    int val;

    struct S_symbol *sym;
}

%token T_TURTLE
%token T_VAR
%token T_FUN
%token T_UP
%token T_DOWN
%token T_MOVETO
%token T_READ
%token T_IF
%token T_ELSE
%token T_WHILE
%token T_RETURN
%token T_MINUS
%token T_PLUS
%token T_MULTIPLY
%token T_EQ
%token T_LT

%token <sym> T_IDENT
%token <val> T_INT_LITERAL

%type <a_program> program
%type <a_vardec> var_decl
%type <a_vardeclist> var_decls

%type <a_fundec> func_decl
%type <a_fundeclist> func_decls

%type <a_fieldlist> idents
%type <a_fieldlist> idents_list

%type <a_stmt> statement
%type <a_stmtlist> compound_statement
%type <a_stmtlist> statement_list


%type <a_exp> expression
%type <a_explist> expression_list
%type <a_explist> expressions

%type <a_exp> comparison

%nonassoc T_EQ T_LT
%left T_MINUS T_PLUS
%left T_MULTIPLY
%left T_NEG

%start program

%%
program
    : T_TURTLE T_IDENT var_decls func_decls compound_statement
        {
            $$ = A_Program(S_name($2), $3, $4, $5);
            sem_trans_prog($$);
            //free_ast_program($$);
            if (sflag) {
                gen_debug();
            } else {
                translate_to_binary();
            }
        }
    ;

var_decls
    : /* empty */           { $$ = NULL; }
    | var_decl var_decls    { $$ = A_VardecList($1, $2); }
    ;

var_decl
    : T_VAR T_IDENT
        { $$ = A_Vardec(TODO_NUM, $2, A_IntExp(TODO_NUM, 0)); }
    | T_VAR T_IDENT '=' expression  { $$ = A_Vardec(TODO_NUM, $2, $4); }
    ;

func_decls
    : /* empty */           { $$ = NULL; }
    | func_decl func_decls  { $$ = A_FundecList($1, $2); }
    ;

func_decl
    : T_FUN T_IDENT '(' idents_list ')' var_decls compound_statement
        { $$ = A_Fundec(TODO_NUM, $2, $4, $6, $7); }
    ;

idents_list
    : /* empty */   { $$ = NULL; }
    | idents        { $$ = $1; }
    ;

idents
    : T_IDENT               { $$ = A_FieldList(A_Field(TODO_NUM, $1), NULL); }
    | T_IDENT ',' idents    { $$ = A_FieldList(A_Field(TODO_NUM, $1), $3); }
    ;

compound_statement
    : '{' statement_list '}'    { $$ = $2; }
    ;

statement_list
    : /* empty */               { $$ = NULL; }
    | statement statement_list  { $$ = A_StmtList($1, $2); }
    ;

statement
    : T_UP
        { $$ = A_UpStmt(TODO_NUM); }
    | T_DOWN
        { $$ = A_DownStmt(TODO_NUM); }
    | T_MOVETO '(' expression ',' expression ')'
        { $$ = A_MoveStmt(TODO_NUM, $3, $5); }
    | T_READ '(' T_IDENT ')'
        { $$ = A_ReadStmt(TODO_NUM, $3); }
    | T_IDENT '=' expression
        { $$ = A_AssignStmt(TODO_NUM, $1, $3); }
    | T_IF '(' comparison ')' compound_statement
        { $$ = A_IfStmt(TODO_NUM, $3, $5, NULL); }
    | T_IF '(' comparison ')' compound_statement T_ELSE compound_statement
        { $$ = A_IfStmt(TODO_NUM, $3, $5, $7); }
    | T_WHILE '(' comparison ')' compound_statement
        { $$ = A_WhileStmt(TODO_NUM, $3, $5); }
    | T_RETURN expression
        { $$ = A_ReturnStmt(TODO_NUM, $2); }
    | T_IDENT '(' expression_list ')'
        { $$ = A_CallStmt(TODO_NUM, $1, $3); }
    | '{' expression_list '}'
        { $$ = A_ExpListStmt(TODO_NUM, $2); }
    ;

expression_list
    : /* empty */   { $$ = NULL; }
    | expressions   { $$ = $1; }
    ;

expressions
    : expression                    { $$ = A_ExpList($1, NULL); }
    | expression ',' expressions    { $$ = A_ExpList($1, $3); }
    ;

expression
    : expression T_PLUS expression
        { $$ = A_OpExp(TODO_NUM, A_plusOp, $1, $3); }
    | expression T_MINUS expression
        { $$ = A_OpExp(TODO_NUM, A_minusOp, $1, $3); }
    | expression T_MULTIPLY expression
        { $$ = A_OpExp(TODO_NUM, A_timesOp, $1, $3); }
    | T_MINUS expression %prec T_NEG
        { $$ = A_OpExp(TODO_NUM, A_negOp, $2, NULL); }
    | T_IDENT
        { $$ = A_VarExp(TODO_NUM, $1); }
    | T_IDENT '(' expression_list ')'
        { $$ = A_CallExp(TODO_NUM, $1, $3); }
    | '(' expression ')'
        { $$ = $2; }
    | T_INT_LITERAL
        { $$ = A_IntExp(TODO_NUM, $1); }
    ;

comparison
    : expression T_EQ expression
        { $$ = A_OpExp(TODO_NUM, A_EQ, $1, $3); }
    | expression T_LT expression
        { $$ = A_OpExp(TODO_NUM, A_LT, $1, $3); }
    ;
%%

void
yyerror(char *s, ...)
{
    va_list ap;
    va_start(ap, s);
    if (yylloc.first_line) {
        fprintf(stderr, "%d.%d-%d.%d: error: ", yylloc.first_line,
                yylloc.first_column, yylloc.last_line, yylloc.last_column);
    }
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}

void
lyyerror(YYLTYPE t, char *s, ...)
{
    va_list ap;
    va_start(ap, s);
    if (t.first_line) {
        fprintf(stderr, "%d.%d-%d.%d: error: ", t.first_line, t.first_column,
        t.last_line, t.last_column);
    }
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}

