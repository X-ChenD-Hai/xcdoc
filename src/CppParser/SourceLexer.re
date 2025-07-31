// re2c --lang c++
#include "CppParser/SourceLexer.h"
#include "Source.tab.h"
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

        [\n \t]* "class" [\n \t]* {
            return CLASS;
        }
        "public" { return PUBLIC; }
        "private" { return PRIVATE; }
        "protected" { return PROTECTED; }
        "static" { return STATIC; }
        "const" { return CONST; }
        "virtual" { return VIRTUAL; }
        "friend" { return FRIEND; }
        "typedef" { return TYPEDEF; }
        "using" { return USING; }
        "namespace" { return NAMESPACE; }
        "template" { return TEMPLATE; }
        "typename" { return TYPENAME; }
        "struct" { return STRUCT; }
        "enum" { return ENUM; }
        "union" { return UNION; }
        "operator" { return OPERATOR; }
        "new" { return NEW; }
        "delete" { return DELETE; }
        "this" { return THIS; }
        "true" { return TRUE; }
        "false" { return FALSE; }
        "nullptr" { return NULLPTR; }
        "sizeof" { return SIZEOF; }
        "alignof" { return ALIGNOF; }
        "and" { return AND; }
        "or" { return OR; }
        "not" { return NOT; }
        "xor" { return XOR; }
        "and_eq" { return AND_EQ; }
        "or_eq" { return OR_EQ; }
        "xor_eq" { return XOR_EQ; }
        "not_eq" { return NOT_EQ; }
        "greater" { return GREATER; }
        "greater_eq" { return GREATER_EQ; }
        "less" { return LESS; }
        "less_eq" { return LESS_EQ; }
        "typeid" { return TYPEID; }
        "static_cast" { return STATIC_CAST; }
        "dynamic_cast" { return DYNAMIC_CAST; }
        "const_cast" { return CONST_CAST; }
        "reinterpret_cast" { return REINTERPRET_CAST; }
        "throw" { return THROW; }
        "try" { return TRY; }
        "catch" { return CATCH; }
        "finally" { return FINALLY; }
        "if" { return IF; }
        "else" { return ELSE; }
        "switch" { return SWITCH; }
        "case" { return CASE; }
        "default" { return DEFAULT; }
        "for" { return FOR; }
        "while" { return WHILE; }
        "do" { return DO; }
        "break" { return BREAK; }
        "continue" { return CONTINUE; }
        "return" { return RETURN; }
        "goto" { return GOTO; }
        "asm" { return ASM; }
        "auto" { return AUTO; }
        "register" { return REGISTER; }
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
CppSymbol::CppSymbol(Kind kind, string_slice_view identifier)
    : __kind(kind), __identifier(identifier) {}