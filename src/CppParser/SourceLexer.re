// re2c --lang c++
#include "CppParser/SourceLexer.h"

SourceLexer::SourceLexer(PreCompiledLexer* pre_compiled_lexer)
    : __pre_compiled_lexer(pre_compiled_lexer) {}