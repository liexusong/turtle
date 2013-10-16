%{
#include "parser.h"

enum { LINECMT, NEWLINE }

static int in_line_cmt = 0;
static int proc(int token);
%}

%option outfile="lexer.c"
%option header-file="lexer.h"
%option nodefault
%option yylineno
%option noyywrap

whitespace  [ \t]
digit       [0-9]
ident       [a-zA-Z][a-zA-Z0-9_']*
line_cmt    \/\/

%%

{whitespace}
"\n"        { proc(NEWLINE); }
{line_cmt}  { int r = proc(LINE_CMT);   if (r != COMMENT)  return r; }
"turtle"    { int r = proc(TURTLE);     if (r != COMMENT) return r; }
"var"       { int r = proc(VAR);        if (r != COMMENT) return r; }
"fun"       { int r = proc(FUN);        if (r != COMMENT) return r; }
"up"        { int r = proc(UP);         if (r != COMMENT) return r; }
"down"      { int r = proc(DOWN);       if (r != COMMENT) return r; }
"moveto"    { int r = proc(MOVETO);     if (r != COMMENT) return r; }
"read"      { int r = proc(READ);       if (r != COMMENT) return r; }
"if"        { int r = proc(IF);         if (r != COMMENT) return r; }
"else"      { int r = proc(ELSE);       if (r != COMMENT) return r; }
"while"     { int r = proc(WHILE);      if (r != COMMENT) return r; }
"return"    { int r = proc(RETURN);     if (r != COMMENT) return r; }

"=="        { int r = proc(EQ);         if (r != COMMENT) return r; }
"<"         { int r = proc{LT};         if (r != COMMENT) return r; }

{ident}     { int r = proc(IDENT);      if (r != COMMENT) return r; }
{digit}+    { int r = proc(INTEGER);    if (r != COMMENT) return r; }
.           { int r = proc(CHAR);       if (r != COMMENT) return r; }

%%

int yyerror(const char *msg)
{
    fprintf(stderr, "Error: %s\n", msg);
    return 0;
}

static int proc(int token)
{
    if (in_line_cmt && token != NEWLINE) {
        return COMMENT;
    }

    switch (token) {
    case LINE_CMT:
        in_line_cmt = 1;
        return COMMENT;
    case NEWLINE:
        yyset_lineno(yyget_lineno() + 1);
        if (in_line_cmt) {
            in_line_cmt = 0;
            return COMMENT;
        }
    case IDENT:
        if (prev_sym) {
            free(prev_sym);
        }
        prev_sym = last_sym;
        last_sym = strdup(yytext);
        return IDENT;
    case INTEGER:
        sscanf(yytext, "%d", &lastval);
        return INTEGER;
    case CHAR:
        return yytext[0];
    default:
        return token;
    }
}
