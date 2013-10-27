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
"!="        { return T_NEQ; }
"<="        { return T_LEQ; }
"<"         { return T_LT; }
">="        { return T_GEQ; }
">"         { return T_GT; }
"+"         { return T_PLUS; }
"-"         { return T_MINUS; }
"*"         { return T_MULTIPLY; }

"//".*\n    { yycolumn = 1; }
"//".*      /* ignore comments */

<<EOF>>     { yyterminate(); }

{ident}     { yylval.sym = s_new_symbol(yytext); log_info("%s", s_name(yylval.sym)); return T_IDENT; }
{digit}+    { sscanf(yytext, "%d", &(yylval.val)); return T_INT_LITERAL; }
.           { return yytext[0]; }

%%
