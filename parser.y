%{
/*-
 * Copyright (c) 2013, Meitian Huang <_@freeaddr.info>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "absyn.h"
#include "global.h"
#include "semant.h"
#include "lexer.h"
%}

%output "parser.c"
%defines "parser.h"
%locations
%define parse.error verbose

%union{
    struct ast_program          *a_program;
    struct ast_var_dec          *a_vardec;
    struct ast_var_dec_list     *a_vardeclist;
    struct ast_fun_dec          *a_fundec;
    struct ast_fun_dec_list     *a_fundeclist;

    struct ast_exp              *a_exp;
    struct ast_exp_list         *a_explist;

    struct ast_stmt             *a_stmt;
    struct ast_stmt_list        *a_stmtlist;

    struct ast_field            *a_field;
    struct ast_field_list       *a_fieldlist;

    int                         val;
    struct s_symbol             *sym;
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
%token T_NEQ
%token T_LEQ
%token T_LT
%token T_GEQ
%token T_GT

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

%nonassoc T_EQ T_LT T_LEQ T_GT T_GEQ
%left T_MINUS T_PLUS
%left T_MULTIPLY
%left T_NEG

%start program

%%
program
    : T_TURTLE T_IDENT var_decls func_decls compound_statement
        {
            $$ = ast_new_program(s_name($2), $3, $4, $5);
            sem_trans_prog($$);
            free($$);
            if (sflag) {
                gen_debug();
            } else {
                translate_to_binary();
            }
        }
    ;

var_decls
    : /* empty */           { $$ = NULL; }
    | var_decl var_decls    { $$ = ast_new_var_dec_list($1, $2); }
    ;

var_decl
    : T_VAR T_IDENT
        { $$ = ast_new_var_dec(@2, $2, ast_int_exp(@1, 0)); }
    | T_VAR T_IDENT '=' expression  { $$ = ast_new_var_dec(@2, $2, $4); }
    ;

func_decls
    : /* empty */           { $$ = NULL; }
    | func_decl func_decls  { $$ = ast_new_fundec_list($1, $2); }
    ;

func_decl
    : T_FUN T_IDENT '(' idents_list ')' var_decls compound_statement
        { $$ = ast_new_fundec(@2, $2, $4, $6, $7); }
    ;

idents_list
    : /* empty */   { $$ = NULL; }
    | idents        { $$ = $1; }
    ;

idents
    : T_IDENT               { $$ = ast_new_field_list(ast_new_field(@1, $1), NULL); }
    | T_IDENT ',' idents    { $$ = ast_new_field_list(ast_new_field(@1, $1), $3); }
    ;

compound_statement
    : '{' statement_list '}'    { $$ = $2; }
    ;

statement_list
    : /* empty */               { $$ = NULL; }
    | statement statement_list  { $$ = ast_new_stmt_list($1, $2); }
    ;

statement
    : T_UP
        { $$ = ast_new_up_stmt(@1); }
    | T_DOWN
        { $$ = ast_new_down_stmt(@1); }
    | T_MOVETO '(' expression ',' expression ')'
        { $$ = ast_new_move_stmt(@1, $3, $5); }
    | T_READ '(' T_IDENT ')'
        { $$ = ast_new_read_stmt(@3, $3); }
    | T_IDENT '=' expression
        { $$ = ast_new_assign_stmt(@1, $1, $3); }
    | T_IF '(' comparison ')' compound_statement
        { $$ = ast_new_ift_stmt(@1, $3, $5); }
    | T_IF '(' comparison ')' compound_statement T_ELSE compound_statement
        { $$ = ast_new_ifte_stmt(@1, $3, $5, $7); }
    | T_WHILE '(' comparison ')' compound_statement
        { $$ = ast_new_while_stmt(@1, $3, $5); }
    | T_RETURN expression
        { $$ = ast_new_return_stmt(@1, $2); }
    | T_IDENT '(' expression_list ')'
        { $$ = ast_new_call_stmt(@1, $1, $3); }
    | '{' expression_list '}'
        { $$ = ast_new_exp_list_stmt(@2, $2); }
    ;

expression_list
    : /* empty */   { $$ = NULL; }
    | expressions   { $$ = $1; }
    ;

expressions
    : expression                    { $$ = ast_new_exp_list($1, NULL); }
    | expression ',' expressions    { $$ = ast_new_exp_list($1, $3); }
    ;

expression
    : expression T_PLUS expression
        { $$ = ast_new_op_exp(@2, ast_plusOp, $1, $3); }
    | expression T_MINUS expression
        { $$ = ast_new_op_exp(@2, ast_minusOp, $1, $3); }
    | expression T_MULTIPLY expression
        { $$ = ast_new_op_exp(@2, ast_timesOp, $1, $3); }
    | T_MINUS expression %prec T_NEG
        { $$ = ast_new_op_exp(@2, ast_negOp, $2, NULL); }
    | T_IDENT
        { $$ = ast_new_var_exp(@1, $1); }
    | T_IDENT '(' expression_list ')'
        { $$ = ast_new_call_exp(@1, $1, $3); }
    | '(' expression ')'
        { $$ = $2; }
    | T_INT_LITERAL
        { $$ = ast_int_exp(@1, $1); }
    ;

comparison
    : expression T_EQ expression
        { $$ = ast_new_op_exp(@2, ast_EQ, $1, $3); }
    | expression T_NEQ expression
        { $$ = ast_new_op_exp(@2, ast_NEQ, $1, $3); }
    | expression T_LT expression
        { $$ = ast_new_op_exp(@2, ast_LT, $1, $3); }
    | expression T_LEQ expression
        { $$ = ast_new_op_exp(@2, ast_LEQ, $1, $3); }
    | expression T_GT expression
        { $$ = ast_new_op_exp(@2, ast_GT, $1, $3); }
    | expression T_GEQ expression
        { $$ = ast_new_op_exp(@2, ast_GEQ, $1, $3); }
    ;
%%

/**
 * The following two functions are taken from "Flex and Bison" by John Levine
 */

/**
 * Standard yyerror() function except that this uses the information in @yylloc
 */
void
yyerror(const char *s, ...)
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

/**
 * Enhanced yyerror() function. Allows the caller to supply a YYLTYPE.
 */
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

