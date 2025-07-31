#pragma once
#include "CppParser/PreCompiledLexer.h"
#include "utils/public.h"
#include "utils/string_slice_view.h"

class CppSymbol {
   public:
    enum class Kind {
        UNKNOWN,
        CLASS,
    };
    friend class SourceLexer;
    template <Kind k>
    friend class CppSymbolImpl;

   private:
    template <Kind k>
    friend class CppSymbolImpl;
    Kind __kind = Kind::UNKNOWN;
    string_slice_view __identifier;
    CppSymbol(Kind kind, string_slice_view identifier);

   public:
    CppSymbol() = delete;
    string_slice_view identifier() const { return __identifier; }
    Kind kind() const { return __kind; }
};
inline std::ostream& operator<<(std::ostream& os, const CppSymbol::Kind& kind) {
    static std::unordered_map<CppSymbol::Kind, std::string> kind_to_string = {
        {CppSymbol::Kind::UNKNOWN, "UNKNOWN"},
        {CppSymbol::Kind::CLASS, "CLASS"},
    };
    os << kind_to_string[kind];
    return os;
}

template <CppSymbol::Kind k>
class CppSymbolImpl;

template <>
class CppSymbolImpl<CppSymbol::Kind::UNKNOWN> : public CppSymbol {
   public:
    CppSymbolImpl(string_slice_view identifier)
        : CppSymbol(CppSymbol::Kind::UNKNOWN, identifier) {}
};
template <>
class CppSymbolImpl<CppSymbol::Kind::CLASS> : public CppSymbol {
   public:
    CppSymbolImpl(string_slice_view identifier)
        : CppSymbol(CppSymbol::Kind::CLASS, identifier) {}
};

// using symbol_list_t = std::vector<std::unique_ptr<CppSymbol>>;
using symbol_list_t = std::vector<std::shared_ptr<CppSymbol>>;
class SourceLexer {
    // PreCompiledLexer __pre_compiled_lexer;
    PreCompiledLexer* __pre_compiled_lexer;
    symbol_list_t __synbols;
    string_slice_view::iterator YYCURSOR;
    string_slice_view::iterator YYMARKER;
    string_slice_view::iterator last_cursor;
    const string_slice_view* __content;

   public:
    SourceLexer(PreCompiledLexer* pre_compiled_lexer);

    symbol_list_t synbols();
    int yylex();
};
