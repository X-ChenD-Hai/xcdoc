#include "./CppSymbol.h"

CppSymbol::CppSymbol(Kind kind, string_slice_view identifier)
    : kind_(kind), identifier_(identifier) {}