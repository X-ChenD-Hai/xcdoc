#pragma once
#include <linux/limits.h>

#include "CppParser/PreCompiledLexer.h"
#include "CppParser/symbolsystem/CppSymbol.h"
#include "utils/public.h"
#include "utils/string_slice_view.h"

// using symbol_list_t = std::vector<std::unique_ptr<CppSymbol>>;
using symbol_list_t = std::vector<std::shared_ptr<CppSymbol>>;
class SourceLexer {
    FRINED_LEXERA
   private:
    PreCompiledLexer* __pre_compiled_lexer;
    symbol_list_t __synbols;
    string_slice_view::iterator YYCURSOR;
    string_slice_view::iterator YYMARKER;
    string_slice_view::iterator last_cursor;
    const string_slice_view* __content;

   private:
    template <CppSymbol::Kind k>
    CppSymbolImpl<k>* __add_symbol(const string_slice_view& symbol);
    template <CppSymbol::Kind k>
    CppSymbolImpl<k>* __add_symbol(const string_slice_view* symbol) {
        return __add_symbol<k>(*symbol);
    }

   public:
    SourceLexer(PreCompiledLexer* pre_compiled_lexer);

    symbol_list_t synbols();
    int yylex();
};