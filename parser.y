%{
#include <stdint.h>
#include <stdio.h>
%}

%token NUMBER

%union {
    int16_t val;
    struct *fun f;
    struct *var v;
    char *str;
    int b;
}

%token turtle
%token var
%token fun
%token up
%token down
%token moveto
%token read
%token IF
%token ELSE
%token WHILE
%token RETURN

%token EQ
%token LT
%token GT
%token LE
%token GE

%token ident

%nonassoc EQ
%left '-' '+'
%left '*'
%left NEG

%%

program
    : turtle ident var_decls func_decls compound_statement
    ;

var_decls
    : /* empty */
    | var_decls var_decl
    ;

var_decl
    : var ident
    | var ident '=' expression
    ;

func_decls
    : /* empty */
    | func_decls func_decl
    ;

func_decl
    : fun ident '(' parameters ')' var_decls compound_statement
    ;

parameters
    : idents
    ;

idents
    : /* empty */
    | ident
    | idents ',' ident
    ;

compound_statement
    : '{' statement_list '}'
    ;

statement_list
    : /* empty */
    | statement_list statement
    ;

statement
    : up
    | down
    | moveto '(' expression ',' expression ')'
    | read '(' ident ')'
    | ident '=' expression
    | IF '(' comparison ')' compound_statement
    | IF '(' comparison ')' compound_statement ELSE compound_statement
    | WHILE '(' comparison ')' compound_statement
    | RETURN expression
    | ident '(' arguments ')'
    | '{' expression_list '}'
    ;

arguments
    : idents
    ;

expression_list
    : /* empty */
    | expression
    | expression_list ',' expression
    ;

expression
    : expression '+' expression
    | expression '-' expression
    | expression '*' expression
    | '-' expression %prec NEG
    | ident
    | ident '(' arguments ')'
    | '(' expression ')'
    ;

comparison
    : expression EQ expression
    | expression LT expression
    | expression GT expression
    | expression LE expression
    | expression GE expression
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
