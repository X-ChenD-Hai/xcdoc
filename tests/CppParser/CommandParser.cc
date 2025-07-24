#include <CppParser/CommandParser.h>
#include <gtest/gtest.h>

TEST(CommandParser, ClangCl) {
    const std::string command =
        R"(D:\software\develop\LLVM\bin\clang-cl.exe  /nologo -TP -DXCDOC_TESTS_RESOURCES_DIR=\"D:/31003/Desktop/code/C++/xc/xcdoc/tests/res/\" -D__xcdoc_debug__ -ID:\31003\Desktop\code\C++\xc\xcdoc\src -imsvcD:\vcpkg\installed\x64-win-llvm\include /DWIN32 /D_WINDOWS /GR /EHsc /Zi /Ob0 /Od /RTC1 -clang:-std=c++23 -MDd /Fosrc\CMakeFiles\xcdoc_lib.dir\CppParser\PreCompiledLexer.re.cc.obj /Fdsrc\CMakeFiles\xcdoc_lib.dir\xcdoc_lib.pdb -c -- D:\31003\Desktop\code\C++\xc\xcdoc\build\clang-cl-debug\src\CppParser\PreCompiledLexer.re.cc)";

    auto parser = CommandParser::parse(command);
    ASSERT_NE(parser, nullptr);
    EXPECT_EQ(parser->command(), command);

    // --------- macros ---------
    auto macros = parser->macros();
    ASSERT_EQ(macros.size(), 2);
    EXPECT_EQ(macros[0].first, "XCDOC_TESTS_RESOURCES_DIR");
    EXPECT_EQ(macros[0].second,
              R"("D:/31003/Desktop/code/C++/xc/xcdoc/tests/res/")");
    EXPECT_EQ(macros[1].first, "__xcdoc_debug__");
    EXPECT_EQ(macros[1].second, "");

    // --------- include_paths ---------
    auto incs = parser->include_paths();
    ASSERT_EQ(incs.size(), 2);
    EXPECT_EQ(incs[0], R"(D:\31003\Desktop\code\C++\xc\xcdoc\src)");
    EXPECT_EQ(incs[1], R"(D:\vcpkg\installed\x64-win-llvm\include)");

    // --------- input_files ---------
    auto inputs = parser->input_files();
    ASSERT_EQ(inputs.size(), 1);
    EXPECT_EQ(
        inputs[0],
        R"(D:\31003\Desktop\code\C++\xc\xcdoc\build\clang-cl-debug\src\CppParser\PreCompiledLexer.re.cc)");

    // --------- output_files ---------
    auto outputs = parser->output_files();
    ASSERT_EQ(outputs.size(), 2);
    EXPECT_EQ(
        outputs[0],
        R"(src\CMakeFiles\xcdoc_lib.dir\CppParser\PreCompiledLexer.re.cc.obj)");
    EXPECT_EQ(outputs[1], R"(src\CMakeFiles\xcdoc_lib.dir\xcdoc_lib.pdb)");

    // --------- options ---------
    auto opts = parser->options();
    // 只验证存在性即可（顺序并不重要）
    auto has_opt = [&](std::string_view k, std::string_view v = "") {
        return std::find_if(opts.begin(), opts.end(), [=](const auto& p) {
                   return p.first == k && p.second == v;
               }) != opts.end();
    };
    EXPECT_TRUE(has_opt("nologo"));
    EXPECT_TRUE(has_opt("DWIN32"));
    EXPECT_TRUE(has_opt("D_WINDOWS"));
    EXPECT_TRUE(has_opt("GR"));
    EXPECT_TRUE(has_opt("EHsc"));
    EXPECT_TRUE(has_opt("Zi"));
    EXPECT_TRUE(has_opt("Ob0"));
    EXPECT_TRUE(has_opt("Od"));
    EXPECT_TRUE(has_opt("RTC1"));
    EXPECT_TRUE(has_opt("-std", "c++23"));
    EXPECT_TRUE(has_opt("MDd"));
}