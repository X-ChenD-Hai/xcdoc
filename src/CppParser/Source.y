%{
#include "CppParser/SourceLexer.h"
#include "Source.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
void yyerror(YYLTYPE *loc,SourceLexer* lexer,  const char *s);
%}
%union {
    int number;
   const char *word;
}
%locations
%parse-param {SourceLexer* lexer}
%lex-param {SourceLexer* lexer}
%token IDENT
%token CCLASS
%token EOF_

%%
program: /* empty */ |
        program statement |
        program EOF_ {  std::cout << "End of file" << std::endl; YYACCEPT; }
        ;

statement: CLASS IDENT ';' { std::cout << "Found class " << std::endl; }
        | error {  YYERROR_CALL("Syntax error"); yyclearin; yyerrok; }
          ;
%%
void yyerror(YYLTYPE *loc,SourceLexer* lexer,  const char *s) {
    printf("Error: %s\n", s);
}