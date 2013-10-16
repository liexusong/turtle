%{
#include "global.h"
#include "ast.h"
#include "parser.h"
%}

%option outfile="lexer.c"
%option header-file="lexer.h"
%option nodefault
%option yylineno
%option noyywrap

whitespace  [ \t]
digit       [0-9]
ident       [a-zA-Z][a-zA-Z0-9_']*

%%

{whitespace} /* ignore */
"\n"        { }
"turtle"    { return T_TURTLE; }
"var"       { return T_VAR; }
"fun"       { return T_FUN; }
"up"        { return T_UP; }
"down"      { return T_DOWN; }
"moveto"    { return T_MOVETO; }
"read"      { return T_READ; }
"if"        { return T_IF; }
"else"      { return T_ELSE; }
"while"     { return T_WHILE; }
"return"    { return T_RETURN; }

"=="        { return T_EQ; }
"<"         { return T_LT; }
"+"         { return T_PLUS; }
"-"         { return T_MINUS; }
"*"         { return T_MULTIPLY; }

"//".*      /* ignore comments */

{ident}     { yylval = ast_new_ident_node(yytext); return T_IDENT; }
{digit}+    {
                int tmp;
                sscanf(yytext, "%d", &tmp);
                yylval = ast_new_int_literal_node(tmp);
                return T_INT_LITERAL;
            }
.           { return yytext[0]; }

%%

int yyerror(const char *msg)
{
    fprintf(stderr, "Error: %s\n", msg);
    return 0;
}

