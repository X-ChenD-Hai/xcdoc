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

        ident {
            return IDENT;
        }
        [\n \t] { return this->yylex(); }
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