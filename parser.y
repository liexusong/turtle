%{
#include "ast.h"
#include "global.h"
%}

%output "parser.c"
%defines "parser.h"

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
%token T_IDENT
%token T_INT_LITERAL

%nonassoc T_EQ LT
%left T_MINUS T_PLUS
%left T_MULTIPLY
%left T_NEG

%%

program
    : T_TURTLE T_IDENT var_decls func_decls compound_statement
        { $$ = ast_new_node4(N_PROG, N_PROG, $2, $3, $4, $5); }
    ;

var_decls
    : /* empty */           { $$ = NULL; }
    | var_decls var_decl    {
                                if ($1 == NULL) {
                                    $$ = $2;
                                } else {
                                    $$ = ast_insert($1, $2);
                                }
                            }
    ;

var_decl
    : T_VAR T_IDENT
        {
            struct ast_node *a = ast_new_int_literal_node(0);
            $$ = ast_new_node2(N_VAR_DEF, 0, $2, a);
        }
    | T_VAR T_IDENT '=' expression
        {
            $$ = ast_new_node2(N_VAR_DEF, 0, $2, $4);
        }
    ;

func_decls
    : /* empty */           { $$ = NULL; }
    | func_decls func_decl  {
                                if ($1 == NULL) {
                                    $$ = $2;
                                } else {
                                    $$ = ast_insert($1, $2);
                                }
                            }
    ;

func_decl
    : T_FUN T_IDENT '(' idents_list ')' var_decls compound_statement
        { $$ = ast_new_node4(N_FUNC_DEF, 0, $2, $4, $6, $7); }
    ;

idents_list
    : /* empty */   { $$ = NULL; }
    | idents        { $$ = $1; }
    ;

idents
    : T_IDENT
        { /*TODO */}
    | idents ',' T_IDENT  { $$ = ast_insert($1, $3); }
    ;

compound_statement
    : '{' statement_list '}'    { $$ = $2; }
    ;

statement_list
    : /* empty */               { $$ = NULL; }
    | statement_list statement  {
                                    if ($1 == NULL) {
                                        $$ = $2;
                                    } else {
                                        $$ = ast_insert($1, $2);
                                    }
                                }
    ;

statement
    : T_UP
        { $$ = ast_new_node0(N_STMT, STMT_UP); }
    | T_DOWN
        { $$ = ast_new_node0(N_STMT, STMT_DOWN); }
    | T_MOVETO '(' expression ',' expression ')'
        { $$ = ast_new_node2(N_STMT, STMT_MOVETO, $3, $5); }
    | T_READ '(' T_IDENT ')'
        { $$ = ast_new_node1(N_STMT, STMT_READ, $3); }
    | T_IDENT '=' expression
        { $$ = ast_new_node2(N_STMT, STMT_ASSIGN, $1, $3); }
    | T_IF '(' comparison ')' compound_statement
        { $$ = ast_new_node2(N_STMT, STMT_IFT, $3, $5); }
    | T_IF '(' comparison ')' compound_statement T_ELSE compound_statement
        { $$ = ast_new_node3(N_STMT, STMT_IFTE, $3, $5, $7); }
    | T_WHILE '(' comparison ')' compound_statement
        { $$ = ast_new_node2(N_STMT, STMT_WHILE, $3, $5); }
    | T_RETURN expression
        { $$ = ast_new_node1(N_STMT, STMT_RET, $2); }
    | T_IDENT '(' expression_list ')'
        { $$ = ast_new_node2(N_STMT, STMT_FUNC_CALL, $1, $3); }
    | '{' expression_list '}'
        { $$ = $2; }
    ;

expression_list
    : /* empty */   { $$ = NULL; }
    | expressions   { $$ = $1; }
    ;

expressions
    : expression                    { $$ = $1; }
    | expressions ',' expression    { $$ = ast_insert($1, $3); }
    ;

expression
    : expression T_PLUS expression
        { $$ = ast_new_arith_node(EXPR_OP_PLUS, $1, $3); }
    | expression T_MINUS expression
        { $$ = ast_new_arith_node(EXPR_OP_MINUS, $1, $3); }
    | expression T_MULTIPLY expression
        { $$ = ast_new_arith_node(EXPR_OP_MULTIPLY, $1, $3); }
    | T_MINUS expression %prec T_NEG
        { $$ = ast_new_arith_node(EXPR_OP_NEG, $2, NULL); }
    | T_IDENT
        { $$ = ast_new_node(N_EXPR, EXPR_IDENT, $1, NULL, NULL, NULL); }
    | T_IDENT '(' expression_list ')'
        { $$ = ast_new_node(N_EXPR, EXPR_FUNC_CALL, $1, $3, NULL, NULL); }
    | '(' expression ')'
        { $$ = $2; }
    | T_INT_LITERAL
        { $$ = $1; }
    ;

comparison
    : expression T_EQ expression
        { $$ = ast_new_comp_node(COMP_EQ, $1, $3); }
    | expression LT expression
        { $$ = ast_new_comp_node(COMP_LT, $1, $3); }
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

