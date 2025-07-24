#include <CppParser/CompileSystem.h>
#include <gtest/gtest.h>
#include <utils/public.h>

TEST(CppParser, CppParser) {
    try {
        CompileSystem system(utils::read_file(CLANG_CL_COMPILE_DATABASE));
        for (auto& unit : system.units()) {
            for (auto& it : unit->attached_lexers()) {
                OUT SV(if, it.first) ENDL;
            }
            // OUT VV(unit->pre_lexer()->source()) ENDL;
        }
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
}