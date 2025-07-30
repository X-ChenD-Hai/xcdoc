%{
#include "Source.tab.h"
#include <stdio.h>
#include <stdlib.h>
void yyerror(YYLTYPE *loc,  const char *s);
%}
%union {
    int number;
   const char *word;
}
%locations
%token <number> NUMBER
%token <word> WORD
%token WHITESPACE

%%
program: /* empty */
        | program statement
        ;

statement: NUMBER { printf("Number: %d\n", $1); }
          | WORD { printf("Word: "); }
          | WHITESPACE {  }
          | error {  YYERROR_CALL("Syntax error"); yyclearin; yyerrok; }
          ;
%%
void yyerror(YYLTYPE *loc, const char *s) {
    printf("Error: %s\n", s);
}