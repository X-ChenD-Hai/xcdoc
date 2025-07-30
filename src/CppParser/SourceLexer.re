// re2c --lang c++
#include "CppParser/SourceLexer.h"
#include "Source.tab.h"
/*!include:re2c "def.re" */

SourceLexer::SourceLexer(PreCompiledLexer* pre_compiled_lexer)
    : __pre_compiled_lexer(pre_compiled_lexer) {}
symbol_list_t SourceLexer::synbols() { return __synbols; };
using K = CppSymbol::Kind;
template <K k>
using S = CppSymbolImpl<k>;
void SourceLexer::parse() {
    auto content = __pre_compiled_lexer->source();
    OUT NV(content) ENDL;
    auto YYCURSOR = content.begin();
    auto last_cursor = YYCURSOR;
    auto YYMARKER = YYCURSOR;

    bool class_prefix = false;
    for (;;) {
        last_cursor = YYCURSOR;
        /*!re2c
            re2c:define:YYCTYPE = char;
            re2c:yyfill:enable = 0;

            [\n \t]* "class" [\n \t]* {
                class_prefix = true;
                continue;
            }

            ident {
                if (class_prefix) {
                    class_prefix = false;
                    __synbols.emplace_back(new S<K::CLASS>({last_cursor,
           YYCURSOR}));
                }
                continue;
            }

            [\x00]{
                break;
            }
            * { continue; }

        */
    }
};
CppSymbol::CppSymbol(Kind kind, string_slice_view identifier)
    : __kind(kind), __identifier(identifier) {}
