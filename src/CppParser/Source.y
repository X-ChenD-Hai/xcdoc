%{
#include "CppParser/SourceLexer.h"
#include "CppParser/symbolsystem/ClassSymbolImpl.h"
#include "Source.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using CISK = ClassInnerStatement::Kind;
template<CISK k>
using CISI = ClassInnerStatementImpl<k>;
using CISS = ClassInnerStatementsSequence;
using CISSS = ClassInnerSubStatementsSequence;
using CISSS_AP = ClassInnerSubStatementsSequence::AccessPolicy;
void yyerror(YYLTYPE *loc,SourceLexer* lexer,  const char *s);
%}
%union {
   string_slice_view* ident;
   ClassInnerStatement* class_inner_statement;
   ClassInnerSubStatementsSequence* class_inner_sub_statements_sequence;
   ClassInnerStatementsSequence* class_inner_statements_sequence;
   ClassInheriteItem* class_inherit_item;
   ClassInheriteList_ptr class_inherit_list;
}
%locations
%parse-param {SourceLexer* lexer}
%lex-param {SourceLexer* lexer}
%token<ident> IDENT
%token OP_SCOPE_RESOLUTION OP_EQ OP_NE OP_GE OP_LE OP_INC OP_DEC OP_ADD_ASSIGN OP_SUB_ASSIGN OP_PTR_ACCESS
%token PUBLIC_ PRIVATE_ PROTECTED_ STATIC_ CONST_ VIRTUAL_ FRIEND_ TYPEDEF_ USING_ NAMESPACE_ TEMPLATE_ TYPENAME_ STRUCT_ ENUM_ UNION_ OPERATOR_ NEW_ DELETE_ THIS_ TRUE_ FALSE_ NULLPTR_ SIZEOF_ ALIGNOF_ AND_ OR_ NOT_ XOR_ AND_EQ_ OR_EQ_ XOR_EQ_ NOT_EQ_ GREATER_ GREATER_EQ_ LESS_ LESS_EQ_ TYPEID_ STATIC_CAST_ DYNAMIC_CAST_ CONST_CAST_ REINTERPRET_CAST_ THROW_ TRY_ CATCH_ FINALLY_ IF_ ELSE_ SWITCH_ CASE_ DEFAULT_ FOR_ WHILE_ DO_ BREAK_ CONTINUE_ RETURN_ RETURN_ GOTO_ ASM_ AUTO_ REGISTER_
%token EOF_
%type <class_inner_sub_statements_sequence> class_inner_sub_statements_seq
%type <class_inner_statements_sequence> class_inner_statement_seq
%type <class_inner_statement> class_inner_statement
%type <class_inherit_item> class_inherit_item
%type <class_inherit_list> class_inherit_list
%%
program: /* empty */ |
        program statement 
        | program ident_parser 
        | program EOF_ {  
                std::cout << "End of file" << std::endl;
                YYACCEPT; }
        ;
statement:
        CLASS_ IDENT ';' {
                lexer->__add_symbol<CppSymbol::Kind::CLASS>($2); 
                std::cout << "Found class "<< *$2 << std::endl; }
        | CLASS_ IDENT class_inherit_list '{' class_inner_statement_seq '}' ';' {  
                lexer->__add_symbol<CppSymbol::Kind::CLASS>($2)->__set_inherite_list($3)->__set_statements_sequence($5);
                 std::cout << "Found class define "<< std::endl; }
        | error {  YYERROR_CALL("Statement error"); yyclearin; yyerrok; }
class_inherit_item: 
        IDENT {
                $$ = new ClassInheriteItem($1);
                std::cout << "Found class inherit "<< *$1 << std::endl; }
        | PUBLIC_ IDENT { 
                $$ = new ClassInheriteItem($2, ClassInheriteItem::InheritePolicy::PUBLIC);
                std::cout << "Found class inherit "<< *$2 << std::endl; }
        | PROTECTED_ IDENT {
                $$ = new ClassInheriteItem($2, ClassInheriteItem::InheritePolicy::PROTECTED);
                std::cout << "Found class inherit "<< *$2 << std::endl; }
        | PRIVATE_ IDENT {
                $$ = new ClassInheriteItem($2, ClassInheriteItem::InheritePolicy::PRIVATE); 
                std::cout << "Found class inherit "<< *$2 << std::endl; }
        | error { 
                YYERROR_CALL("class_inherit_item error"); yyclearin;
                yyerrok; }
class_inherit_list: /* empty */ { $$ = nullptr; } 
        | ':' class_inherit_item { 
                std::cout<<"insert " << $2 <<std::endl; 
                $$ = new ClassInheriteList(); 
                $$->emplace_back($2); 
                std::cout << "Found class inherit list "<< std::endl; }
        | class_inherit_list ',' class_inherit_item {
                std::cout<<"insert " << $3 << std::endl; 
                $1->emplace_back($3); $$ = $1; 
                std::cout << "Found class inherit list "<< std::endl; }
        | error {
                if($$->size() > 0) std::cout << $$ <<std::endl;
                YYERROR_CALL("class_inherit_list error"); 
                yyclearin; yyerrok; }
class_inner_statement: 
        IDENT IDENT ';' { 
                $$ = new CISI<CISK::MEMBER_VARIABLE>($1,$2); 
                std::cout << "Found inner statement "<< *$1 << " " << *$2 << std::endl; }        ;
        | error {  YYERROR_CALL("class_inner_statement error"); yyclearin; yyerrok; }
                       ;
class_inner_sub_statements_seq: 
        class_inner_statement { 
                $$ = new CISSS(); 
                $$->__statements.emplace_back($1); 
                std::cout << "Found inner sub statement seq "<< std::endl; }
        | PUBLIC_ ':' class_inner_statement { 
                $$ = new CISSS(); 
                $$->__access_policy = CISSS_AP::PUBLIC; 
                $$->__statements.emplace_back($3); 
                std::cout << "Found inner sub statement seq "<< std::endl; }
        | PROTECTED_ ':' class_inner_statement { 
                $$ = new CISSS(); 
                $$->__access_policy = CISSS_AP::PROTECTED; 
                $$->__statements.emplace_back($3); 
                std::cout << "Found inner sub statement seq "<< std::endl; }
        | PRIVATE_ ':' class_inner_statement { 
                $$ = new CISSS(); 
                $$->__access_policy = CISSS_AP::PRIVATE; 
                $$->__statements.emplace_back($3); 
                std::cout << "Found inner sub statement seq "<< std::endl; }
        | class_inner_sub_statements_seq class_inner_statement{ 
                $1->__statements.emplace_back($2); 
                $$ = $1; 
                std::cout << "Found inner sub statement seq "<< std::endl; }
        | error {
                YYERROR_CALL("class_inner_sub_statements_seq error"); 
                yyclearin; yyerrok; }
        ;
class_inner_statement_seq: { 
                $$ = new CISS(); }
        | class_inner_statement_seq class_inner_sub_statements_seq {
                $$ = $1; 
                $1->__sub_sequences.emplace_back($2); 
                std::cout << "Found inner statement seq "<< std::endl; }
        | error {  
                YYERROR_CALL("class_inner_statement_seq error");
                yyclearin; yyerrok; }
        ;
ident_parser: 
        IDENT OP_SCOPE_RESOLUTION IDENT { 
                std::cout << "Found ident "<< *$1 << "::" << *$3 << std::endl; }
        | error {  YYERROR_CALL("ident_parser error"); yyclearin; yyerrok; }
        ;
%%
void yyerror(YYLTYPE *loc,SourceLexer* lexer,  const char *s) {
    printf("Error: %s\n", s);
}