#include <CppParser/CompileSystem.h>
#include <gtest/gtest.h>
#include <utils/public.h>

TEST(CppParser, CppParser) {
#ifdef CLANG_CL_COMPILE_DATABASE
    try {
        CompileSystem clang_cl_system(
            utils::read_file(CLANG_CL_COMPILE_DATABASE));
        for (auto& unit : clang_cl_system.units()) {
            for (auto& it : unit->attached_lexers()) {
                OUT SV(if, it.first) ENDL;
            }
            OUT VV(unit->pre_lexer()->source()) ENDL;
        }
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
#endif
#ifdef GCC_COMPILE_DATABASE
    try {
        CompileSystem gcc_system(utils::read_file(GCC_COMPILE_DATABASE));
        for (auto& unit : gcc_system.units()) {
            for (auto& it : unit->attached_lexers()) {
                OUT SV(if, it.first) ENDL;
            }
            OUT VV(unit->pre_lexer()->source()) ENDL;
        }
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
#endif
#ifdef CLANG_COMPILE_DATABASE
    try {
        CompileSystem clang_system(utils::read_file(CLANG_COMPILE_DATABASE));
        for (auto& unit : clang_system.units()) {
            for (auto& it : unit->attached_lexers()) {
                OUT SV(if, it.first) ENDL;
            }
            OUT VV(unit->pre_lexer()->source()) ENDL;
        }
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
#endif
}