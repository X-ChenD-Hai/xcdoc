#include "./ClassSymbolImpl.h"
CppSymbolImpl<CppSymbol::Kind::CLASS>*
CppSymbolImpl<CppSymbol::Kind::CLASS>::__set_statements_sequence(
    ClassInnerStatementsSequence* statements_sequence) {
#ifdef __xcdoc_debug__

    for (auto& seq : statements_sequence->__sub_sequences) {
        OUT SV(access_policy, seq->__access_policy) ENDL;
        for (auto& statement : seq->__statements) {
            OUT VV("statement: ") SV(kind, statement->__kind) ENDL;
        }
    }

#endif
    delete statements_sequence;
    return this;
}
utils::TodoType CppSymbolImpl<CppSymbol::Kind::CLASS>::methods() { TODO; }
utils::TodoType CppSymbolImpl<CppSymbol::Kind::CLASS>::fields() { TODO; };
utils::TodoType CppSymbolImpl<CppSymbol::Kind::CLASS>::symbols() { TODO; };
void ClassInnerSubStatementsSequence::__add_statement(
    ClassInnerStatement* statement) {
    __statements.emplace_back(statement);
};
CppSymbolImpl<CppSymbol::Kind::CLASS>::CppSymbolImpl(
    string_slice_view identifier,
    ClassInnerStatementsSequence* statements_sequence)
    : CppSymbol(CppSymbol::Kind::CLASS, identifier) {}
CppSymbolImpl<CppSymbol::Kind::CLASS>*
CppSymbolImpl<CppSymbol::Kind::CLASS>::__set_inherite_list(
    ClassInheriteList_ptr inherite_list) {
    if (inherite_list == nullptr) return this;
#ifdef __xcdoc_debug__
    for (auto& inherite : *inherite_list) {
        if (inherite) {
            OUT SV(inherite_item_name, *inherite->__base_class) ENDL;
            OUT SV(inherite_item_access_policy, inherite->__inherite_policy)
                ENDL;
            OUT SV(inherite_item_is_virtual, inherite->__is_virtual) ENDL;
        }
    }
#endif
    delete inherite_list;
    return this;
}
