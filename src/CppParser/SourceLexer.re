// re2c --lang c++
#include <magic_enum/magic_enum.hpp>

#include "CppParser/SourceLexer.h"
#include "CppParser/symbolsystem/ClassSymbolImpl.h"
#include "Source.tab.h"
#include "utils/public.h"
#include "utils/string_slice_view.h"

/*!include:re2c "def.re" */
int yyparse(SourceLexer* lexer);
int yylex(SourceLexer* lexer) { return lexer->yylex(); }

SourceLexer::SourceLexer(PreCompiledLexer* pre_compiled_lexer)
    : __pre_compiled_lexer(pre_compiled_lexer) {
    __content = &__pre_compiled_lexer->source();
    YYCURSOR = __content->begin();
    last_cursor = YYCURSOR;
    YYMARKER = YYCURSOR;
    OUT NV(*__content) ENDL;
    yyparse(this);
}
symbol_list_t SourceLexer::synbols() { return __synbols; };
using K = CppSymbol::Kind;
template <K k>
using S = CppSymbolImpl<k>;
int SourceLexer::yylex() {
    last_cursor = YYCURSOR;
    /*!re2c
        re2c:define:YYCTYPE = char;
        re2c:yyfill:enable = 0;

        "class"  { return CLASS_;}
        "public" { return PUBLIC_; }
        "private" { return PRIVATE_; }
        "protected" { return PROTECTED_; }
        "static" { return STATIC_; }
        "const" { return CONST_; }
        "virtual" { return VIRTUAL_; }
        "friend" { return FRIEND_; }
        "typedef" { return TYPEDEF_; }
        "using" { return USING_; }
        "namespace" { return NAMESPACE_; }
        "template" { return TEMPLATE_; }
        "typename" { return TYPENAME_; }
        "struct" { return STRUCT_; }
        "enum" { return ENUM_; }
        "union" { return UNION_; }
        "operator" { return OPERATOR_; }
        "new" { return NEW_; }
        "delete" { return DELETE_; }
        "this" { return THIS_; }
        "true" { return TRUE_; }
        "false" { return FALSE_; }
        "nullptr" { return NULLPTR_; }
        "sizeof" { return SIZEOF_; }
        "alignof" { return ALIGNOF_; }
        "and" { return AND_; }
        "or" { return OR_; }
        "not" { return NOT_; }
        "xor" { return XOR_; }
        "and_eq" { return AND_EQ_; }
        "or_eq" { return OR_EQ_; }
        "xor_eq" { return XOR_EQ_; }
        "not_eq" { return NOT_EQ_; }
        "greater" { return GREATER_; }
        "greater_eq" { return GREATER_EQ_; }
        "less" { return LESS_; }
        "less_eq" { return LESS_EQ_; }
        "typeid" { return TYPEID_; }
        "static_cast" { return STATIC_CAST_; }
        "dynamic_cast" { return DYNAMIC_CAST_; }
        "const_cast" { return CONST_CAST_; }
        "reinterpret_cast" { return REINTERPRET_CAST_; }
        "throw" { return THROW_; }
        "try" { return TRY_; }
        "catch" { return CATCH_; }
        "finally" { return FINALLY_; }
        "if" { return IF_; }
        "else" { return ELSE_; }
        "switch" { return SWITCH_; }
        "case" { return CASE_; }
        "default" { return DEFAULT_; }
        "for" { return FOR_; }
        "while" { return WHILE_; }
        "do" { return DO_; }
        "break" { return BREAK_; }
        "continue" { return CONTINUE_; }
        "return" { return RETURN_; }
        "goto" { return GOTO_; }
        "asm" { return ASM_; }
        "auto" { return AUTO_; }
        "register" { return REGISTER_; }
        "==" { return OP_EQ; }
        "!=" { return OP_NE; }
        ">=" { return OP_GE; }
        "<=" { return OP_LE; }
        "++" { return OP_INC; }
        "--" { return OP_DEC; }
        "+=" { return OP_ADD_ASSIGN; }
        "-=" { return OP_SUB_ASSIGN; }
        "->" { return OP_PTR_ACCESS; }
         "::" {
            return OP_SCOPE_RESOLUTION;
        }
        ident {
            yylval.ident = new string_slice_view(last_cursor, YYCURSOR);
            return IDENT;
        }
        [\n \t]+ { return this->yylex(); }
        [\x00]{
            return EOF_;
        }
        * {
            return *(last_cursor);
         }

    */
    return EOF_;
};


template <>
CppSymbolImpl<K::CLASS>* SourceLexer::__add_symbol<K::CLASS>(
    const string_slice_view& symbol) {
    __synbols.emplace_back(new S<K::CLASS>(symbol));
    return (S<K::CLASS>*)(__synbols.back().get());
}
