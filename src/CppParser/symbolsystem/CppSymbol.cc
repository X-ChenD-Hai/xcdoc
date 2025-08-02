#include "./CppSymbol.h"

CppSymbol::CppSymbol(Kind kind, string_slice_view identifier)
    : __kind(kind), __identifier(identifier) {}