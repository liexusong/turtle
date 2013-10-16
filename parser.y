%{
#include <stdint.h>
#include <stdio.h>

int prepare_func(char *str, int count);
%}

%output "parser.c"
%defines "parser.h"

%union {
    int16_t val;
//    struct *func f;
//    struct *var v;
    char *str;
    int count;
    struct *ast ast;
}

%token TURTLE
%token VAR
%token FUN
%token UP
%token DOWN
%token MOVETO
%token READ
%token IF
%token ELSE
%token WHILE
%token RETURN

%token EQ
%token LT
%token GT
%token LE
%token GE

%token <str> IDENT
%token <val> INTEGER

%type <ast> var_decls_list
%type <ast> var_decls
%type <ast> var_decl
%type <ast> func_decls
%type <ast> func_decl
%type <ast> parameters
%type <ast> idents_list
%type <ast> idents
%type <ast> compound_statement
%type <ast> statement_list
%type <ast> statement
%type <ast> arguments
%type <ast> expression_list
%type <ast> expression
%type <ast> comparison


%nonassoc EQ
%left '-' '+'
%left '*'
%left NEG

%%

program
    : TURTLE IDENT BEGIN_GLOBAL_VAR_DECLS var_decls_list END_GLOBAL_VAR_DECLS
    func_decls compound_statement
        {}
    ;

BEGIN_GLOBAL_VAR_DECLS
    : /* empty */   { is_global = 1; }
    ;

END_GLOBAL_VAR_DECLS
    : /* empty */   { is_global = 0; }
    ;

var_decls_list
    : /* empty */   { $$ = NULL; }
    | var_decls     { $$ = $1; }

var_decls
    : var_decls var_decl    { $$ = ast_insert($1, $2); }
    | var_decl              { $$ = $1; }
    ;

var_decl
    : VAR IDENT                 {}
    | VAR IDENT '=' expression  {}
    ;

func_decls
    : /* empty */           { $$ = NULL; }
    | func_decls func_decl  { $$ = ast_insert($1, $2); }
    ;

func_decl
    : FUN IDENT '(' parameters ')' var_decls_list compound_statement
                                {
                                    in_func = true;
                                }
    ;

parameters
    : idents_list            { $$ = $1; }
    ;

idents_list
    : /* empty */       { $$ = NULL; }
    | idents            { $$ = $1; }
    ;

idents
    : IDENT             { }
    | idents ',' IDENT  { ast_insert() }
    ;

compound_statement
    : '{' statement_list '}'    { $$ = $2 }
    ;

statement_list
    : /* empty */               { $$ = NULL; }
    | statement_list statement  { }
    ;

statement
    : UP
        { $$ = ast_new_node(K_STMT, UP, NULL, NULL, NULL, NULL); }
    | DOWN
        { $$ = ast_new_node(K_STMT, Down, NULL, NULL, NULL, NULL); }
    | MOVETO '(' expression ',' expression ')'
        { $$ = ast_new_node(K_STMT, MOVETO, $3, $5, NULL, NULL); }
    | READ '(' IDENT ')'
        { $$ = ast_new_node(K_STMT, READ, $3, NULL, NULL, NULL); }
    | IDENT '=' expression
        { $$ = ast_new_node(K_STMT, Assign, $3, NULL, NULL, NULL); }
    | IF '(' comparison ')' compound_statement
        { $$ = ast_new_node(K_STMT, IFT, $3, $5, NULL, NULL); }
    | IF '(' comparison ')' compound_statement ELSE compound_statement
        { $$ = ast_new_node(K_STMT, IFTE, $3, $5, $7, NULL); }
    | WHILE '(' comparison ')' compound_statement
        { $$ = ast_new_node(K_STMT, WHILE, $3, $5, NULL, NULL); }
    | RETURN expression
        { $$ = ast_new_node(K_STMT, RETURN, $2, NULL, NULL, NULL); }
    | IDENT '(' arguments ')'
        { $$ = ast_new_node(K_STMT, FUNC_CALL_STMT, $1, $3, NULL, NULL); }
    | '{' expression_list '}'
        { $$ = $2; }
    ;

arguments
    : idents_list    { $$ = $1; }
    ;

expression_list
    : /* empty */                       { $$ = NULL; }
    | expression                        { }
    | expression_list ',' expression    { }
    ;

expression
    : expression '+' expression
        { $$ = ast_new_node(2, K_EXPR, PLUS, $1, $3, NULL, NULL); }
    | expression '-' expression
        { $$ = ast_new_node(2, K_EXPR, MINUS, $1, $3, NULL, NULL); }
    | expression '*' expression
        { $$ = ast_new_node(2, K_EXPR, MULTIPLY, $1, $3, NULL, NULL); }
    | '-' expression %prec NEG
        { $$ = ast_new_node(1, K_EXPR, NEG, $2, NULL, NULL, NULL); }
    | IDENT
        { $$ = ast_new_node(2, K_EXPR, MULTIPLY, NULL, NULL, NULL, NULL,
        (void *) $1; }
    | IDENT '(' arguments ')'
        { $$ = ast_new_node(2, K_EXPR, FUNC_CALL_EXPR, $3, NULL, NULL, NULL,
        (void *) $1; }
    | '(' expression ')'
        { $$ = $2; }
    | INTEGER
        { $$ = ast_new_node(1, K_EXPR, FUNC_CALL_EXPR, $1, NULL, NULL, NULL); }
    ;

comparison
    : expression EQ expression
        { $$ = ast_new_node(2, K_COMP, EQ, $1, $3, NULL, NULL); }
    | expression GT expression
        { $$ = ast_new_node(2, K_COMP, EQ, $1, $3, NULL, NULL); }
    ;
%%

main ()
{
  yyparse ();
}

yyerror (char *s)  /* Called by yyparse on error */
{
  printf ("\terror: %s\n", s);
}


void prepare_func(char *str, int count)
{
    if (undefined) {
        // error
    }
    if (lookup(str)->count != count) {
        // error
    }
    emit_func(str);
}
