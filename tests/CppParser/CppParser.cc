#include <CppParser/CompileSystem.h>
#include <gtest/gtest.h>
#include <utils/public.h>

TEST(CppParser, CppParser) {
    try {
        CompileSystem system(utils::read_file(XCDOC_TESTS_RESOURCES_DIR
                                              "build/compile_commands.json"));
        for (auto& unit : system.units()) {
            for (auto& it : unit->attached_lexers()) {
                OUT SV(if, it.first) ENDL;
            }
        }
    } catch (const std::exception& e) {
        FAIL() << e.what();
    }
}