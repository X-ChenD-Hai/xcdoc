#pragma once
#include <memory>
#include <vector>

#include "./CppSymbol.h"
#include "utils/public.h"
class ClassInnerStatementsSequence;

class ClassInnerStatement {
    FRINED_LEXERA
   public:
    enum class Kind {
        CLASS,
        STRUCT,
        ENUM,
        UNION,
        ALiAS,
        MEMBER_FUNCTION,
        MEMBER_VARIABLE,
    };

   private:
    utils::ssv_uptr __identifier;
    Kind __kind;

   protected:
    ClassInnerStatement(Kind kind, utils::ssv_ptr identifier)
        : __kind(kind), __identifier(identifier) {}
};
template <ClassInnerStatement::Kind k>
class ClassInnerStatementImpl;
template <>
class ClassInnerStatementImpl<ClassInnerStatement::Kind::MEMBER_VARIABLE>
    : public ClassInnerStatement {
    FRINED_LEXERA
   private:
    utils::ssv_uptr __type{nullptr};

   public:
    ClassInnerStatementImpl(utils::ssv_ptr type, utils::ssv_ptr identifier)
        : ClassInnerStatement(ClassInnerStatement::Kind::MEMBER_VARIABLE,
                              identifier),
          __type(type) {}
};
class ClassInnerSubStatementsSequence {
    FRINED_LEXERA
   public:
    enum class AccessPolicy {
        PUBLIC,
        PRIVATE,
        PROTECTED,
    };

   private:
    std::vector<std::unique_ptr<ClassInnerStatement>> __statements;
    AccessPolicy __access_policy = AccessPolicy::PRIVATE;

    void __add_statement(ClassInnerStatement* statement);
    void __set_access_policy(AccessPolicy access_policy) {
        __access_policy = access_policy;
    }
};
class ClassInnerStatementsSequence {
    FRINED_LEXERA
    template <ClassInnerStatement::Kind k>
    class CppSymbolImpl;
    std::vector<std::unique_ptr<ClassInnerSubStatementsSequence>>
        __sub_sequences;
};
class ClassInheriteItem {
    FRINED_LEXERA
   public:
    enum class InheritePolicy { PUBLIC, PROTECTED, PRIVATE, DEFAULT };

   private:
    utils::ssv_uptr __base_class{nullptr};
    InheritePolicy __inherite_policy{InheritePolicy::DEFAULT};
    bool __is_virtual{false};

   public:
    ClassInheriteItem(utils::ssv_ptr base_classes,
                      InheritePolicy inherite_policy = InheritePolicy::DEFAULT,
                      bool is_virtual = false)
        : __base_class(base_classes),
          __inherite_policy(inherite_policy),
          __is_virtual(is_virtual) {}
};
using ClassInheriteList = std::vector<std::unique_ptr<ClassInheriteItem>>;
using ClassInheriteList_ptr = ClassInheriteList*;
template <>
class CppSymbolImpl<CppSymbol::Kind::CLASS> : public CppSymbol {
    FRINED_LEXERA
   public:
   public:
    CppSymbolImpl(string_slice_view identifier,
                  ClassInnerStatementsSequence* statements_sequence = nullptr);
    utils::TodoType methods();
    utils::TodoType fields();
    utils::TodoType symbols();

   private:
    CppSymbolImpl<CppSymbol::Kind::CLASS>* __set_statements_sequence(
        ClassInnerStatementsSequence* statements_sequence);
    CppSymbolImpl<CppSymbol::Kind::CLASS>* __set_inherite_list(
        ClassInheriteList_ptr inherite_list);
};