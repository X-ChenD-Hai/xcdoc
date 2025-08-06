#pragma once
#include "utils/string_slice_view.h"
#define FRINED_LEXERA                       \
    friend int yyparse(SourceLexer* lexer); \
    friend class SourceLexer;               \
    template <CppSymbol::Kind k>            \
    friend class CppSymbolImpl;

class SourceLexer;
class CppSymbol {
   public:
    enum class Kind {
        UNKNOWN,
        FUNCTION,
        VARIABLE,
        NAMESPACE,
        CLASS,
        STRUCT,
        ENUM,
        UNION,
        ALiAS,
        MEMBER_FUNCTION,
        MEMBER_VARIABLE,
    };
    friend class SourceLexer;
    template <Kind k>
    friend class CppSymbolImpl;

   private:
    template <Kind k>
    friend class CppSymbolImpl;
    Kind kind_ = Kind::UNKNOWN;
    string_slice_view identifier_;
    CppSymbol(Kind kind, string_slice_view identifier);

   public:
    CppSymbol() = delete;
    inline string_slice_view identifier() const { return identifier_; }
    inline Kind kind() const { return kind_; }
};

template <CppSymbol::Kind k>
class CppSymbolImpl;
template <>
class CppSymbolImpl<CppSymbol::Kind::UNKNOWN> : public CppSymbol {
   public:
    CppSymbolImpl(string_slice_view identifier)
        : CppSymbol(CppSymbol::Kind::UNKNOWN, identifier) {}
};