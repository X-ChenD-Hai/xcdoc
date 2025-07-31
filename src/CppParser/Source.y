%{
#include "CppParser/SourceLexer.h"
#include "Source.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
void yyerror(YYLTYPE *loc,SourceLexer* lexer,  const char *s);
%}
%union {
   string_slice_view* ident;
}
%locations
%parse-param {SourceLexer* lexer}
%lex-param {SourceLexer* lexer}
%token<ident> IDENT
%token OP_SCOPE_RESOLUTION OP_EQ OP_NE OP_GE OP_LE OP_INC OP_DEC OP_ADD_ASSIGN OP_SUB_ASSIGN OP_PTR_ACCESS
%token PUBLIC PRIVATE PROTECTED STATIC CONST VIRTUAL FRIEND TYPEDEF USING NAMESPACE TEMPLATE TYPENAME STRUCT ENUM UNION OPERATOR NEW DELETE THIS TRUE FALSE NULLPTR SIZEOF ALIGNOF AND OR NOT XOR AND_EQ OR_EQ XOR_EQ NOT_EQ GREATER GREATER_EQ LESS LESS_EQ TYPEID STATIC_CAST DYNAMIC_CAST CONST_CAST REINTERPRET_CAST THROW TRY CATCH FINALLY IF ELSE SWITCH CASE DEFAULT FOR WHILE DO BREAK CONTINUE RETURN RETURN GOTO ASM AUTO REGISTER
%token EOF_
%%
program: /* empty */ |
        program statement |
        program ident_parser |
        program EOF_ {  std::cout << "End of file" << std::endl; YYACCEPT; }
        ;
statement: CLASS IDENT ';' { std::cout << "Found class "<< *$2 << std::endl; }
        | CLASS IDENT  class_define ';' { std::cout << "Found class define "<< std::endl; }
        | error {  YYERROR_CALL("Syntax error"); yyclearin; yyerrok; }

class_define : '{' class_inner_statement_seq '}'  { std::cout << "Found class define "<< std::endl; }
class_inner_statement: IDENT IDENT ';' { std::cout << "Found inner statement "<< *$1 << " " << *$2 << std::endl; }        ;
                       | error {  YYERROR_CALL("Syntax error"); yyclearin; yyerrok; }
class_inner_statement_seq: |
    class_inner_statement_seq class_inner_statement { std::cout << "Found inner statement seq "<< std::endl; }

ident_parser: IDENT OP_SCOPE_RESOLUTION IDENT { std::cout << "Found ident "<< *$1 << "::" << *$3 << std::endl; }
%%
void yyerror(YYLTYPE *loc,SourceLexer* lexer,  const char *s) {
    printf("Error: %s\n", s);
}