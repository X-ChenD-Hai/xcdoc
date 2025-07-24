#include <CppParser/CompileSystem.h>
#include <gtest/gtest.h>
#include <utils/public.h>

TEST(CppParser, CppParser) {
    CompileSystem system(
        utils::read_file(XCDOC_TESTS_RESOURCES_DIR "compile_commands.json"));
}