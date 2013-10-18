%{
#include "global.h"
#include "absyn.h"
#include "parser.h"

int yycolumn = 1;

#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
                       yylloc.first_column = yycolumn; \
                       yylloc.last_column = yycolumn + yyleng - 1; \
                       yycolumn += yyleng;
%}

%option outfile="lexer.c"
%option header-file="lexer.h"
%option nodefault
%option yylineno
%option noyywrap

whitespace  [ \t]
digit       [0-9]
ident       [a-zA-Z][a-zA-Z0-9\_\']*

%%

{whitespace} /* ignore */
"\n"        { yycolumn  = 1; }
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

<<EOF>>     { yyterminate(); }

{ident}     { yylval.sym = S_Symbol(strdup(yytext)); log_info("%s", S_name(yylval.sym)); return T_IDENT; }
{digit}+    { sscanf(yytext, "%d", &(yylval.val)); return T_INT_LITERAL; }
.           { return yytext[0]; }

%%
