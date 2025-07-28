#pragma once
#include "CppParser/PreCompiledLexer.h"
#include "utils/public.h"

class CppSymbol {
   public:
    enum class Kind {
        UNKNOWN,
        CLASS,
    };

   protected:
    template <Kind k>
    friend class CppSymbolImpl;
    Kind __kind = Kind::UNKNOWN;
    CppSymbol() = default;
};
template <CppSymbol::Kind k = CppSymbol::Kind::UNKNOWN>
class CppSymbolImpl;

template <>
class CppSymbolImpl<CppSymbol::Kind::UNKNOWN> : public CppSymbol {
   private:
   public:
    std::string name() const { TODO; };
    const std::string& kind() const { TODO; };
    template <Kind _k>
    auto& __as() {
        return static_cast<CppSymbolImpl<_k>>(*this);
    }
    template <class T>
    bool is() const noexcept {
        TODO;
    }
};

class SourceLexer {
    // PreCompiledLexer __pre_compiled_lexer;
    PreCompiledLexer* __pre_compiled_lexer;

   public:
    SourceLexer(PreCompiledLexer* pre_compiled_lexer);
};