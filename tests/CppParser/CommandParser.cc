#include <CppParser/CommandParser.h>
#include <gtest/gtest.h>

#include <algorithm>

TEST(CommandParser, ClangCl) {
    const std::string command =
        R"(D:\software\develop\LLVM\bin\clang-cl.exe  /nologo -TP -DXCDOC_TESTS_RESOURCES_DIR=\"D:/31003/Desktop/code/C++/xc/xcdoc/tests/res/\" -D__xcdoc_debug__ -ID:\31003\Desktop\code\C++\xc\xcdoc\src -imsvcD:\vcpkg\installed\x64-win-llvm\include /DWIN32 /D_WINDOWS /GR /EHsc /Zi /Ob0 /Od /RTC1 -clang:-std=c++23 -MDd /Fosrc\CMakeFiles\xcdoc_lib.dir\CppParser\PreCompiledLexer.re.cc.obj /Fdsrc\CMakeFiles\xcdoc_lib.dir\xcdoc_lib.pdb -c -- src\CppParser\PreCompiledLexer.re.cc)";

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
    EXPECT_EQ(inputs[0], R"(src\CppParser\PreCompiledLexer.re.cc)");

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
TEST(CommandParser, Gcc) {
    const std::string cmd =
        "/usr/bin/g++ -std=c++20 -Wall -Wextra -DDEBUG "
        "-DVERSION_STRING=\\\"1.2.3\\\" "
        "-I/usr/local/include -isystem/opt/boost/include "
        "-o my_app main.cpp utils/log.cpp -lssl -lcrypto";

    auto parser = CommandParser::parse(cmd);
    ASSERT_NE(parser, nullptr);
    EXPECT_EQ(parser->command(), cmd);

    /* macros */
    auto macros = parser->macros();
    ASSERT_EQ(macros.size(), 2);
    EXPECT_EQ(macros[0].first, "DEBUG");
    EXPECT_EQ(macros[0].second, "");
    EXPECT_EQ(macros[1].first, "VERSION_STRING");
    EXPECT_EQ(macros[1].second, R"("1.2.3")");

    /* include paths */
    auto incs = parser->include_paths();
    ASSERT_EQ(incs.size(), 2);
    EXPECT_EQ(incs[0], "/usr/local/include");
    EXPECT_EQ(incs[1], "/opt/boost/include");

    /* input files */
    auto inputs = parser->input_files();
    ASSERT_EQ(inputs.size(), 2);
    EXPECT_EQ(inputs[0], "main.cpp");
    EXPECT_EQ(inputs[1], "utils/log.cpp");

    /* output files */
    auto outs = parser->output_files();
    ASSERT_EQ(outs.size(), 1);
    EXPECT_EQ(outs[0], "my_app");

    auto opts = parser->options();
    /* options (仅抽查) */
    auto has = [&](std::string_view k, std::string_view v = "") {
        return std::any_of(opts.begin(), opts.end(), [&](const auto& p) {
            return p.first == k && p.second == v;
        });
    };

    EXPECT_TRUE(has("Wall"));
    EXPECT_TRUE(has("Wextra"));
    EXPECT_TRUE(has("std", "c++20"));
}

/* ---------- 2. Clang (Unix 风格) ---------- */
TEST(CommandParser, ClangUnix) {
    const std::string cmd =
        "clang++ -std=c++23 -Weverything -DENABLE_TRACING "
        "-I./third_party/eigen -I./third_party/fmt/include "
        "-O2 -march=native -fuse-ld=lld "
        "-o build/bin/renderer src/main.cc src/gl/context.cc";

    auto parser = CommandParser::parse(cmd);
    ASSERT_NE(parser, nullptr);
    EXPECT_EQ(parser->command(), cmd);

    /* macros */
    auto macros = parser->macros();
    ASSERT_EQ(macros.size(), 1);
    EXPECT_EQ(macros[0].first, "ENABLE_TRACING");
    EXPECT_EQ(macros[0].second, "");

    /* include */
    auto incs = parser->include_paths();
    ASSERT_EQ(incs.size(), 2);
    EXPECT_EQ(incs[0], "./third_party/eigen");
    EXPECT_EQ(incs[1], "./third_party/fmt/include");

    /* inputs */
    auto inputs = parser->input_files();
    ASSERT_EQ(inputs.size(), 2);
    EXPECT_EQ(inputs[0], "src/main.cc");
    EXPECT_EQ(inputs[1], "src/gl/context.cc");

    /* outputs */
    auto outs = parser->output_files();
    ASSERT_EQ(outs.size(), 1);
    EXPECT_EQ(outs[0], "build/bin/renderer");
}

/* ---------- 3. MSVC (cl.exe) ---------- */
TEST(CommandParser, Msvc) {
    const std::string cmd =
        R"("C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64\cl.exe")"
        R"( /nologo /W4 /DUNICODE /D_UNICODE /D_WINDOWS /Z7 /Od /FS /EHsc /std:c++20)"
        R"( /I..\include /I"C:\vcpkg\installed\x64-windows\include" /Fobuild\obj\)"
        R"( /Febuild\bin\app.exe main.cpp file1.cpp file2.cpp)";

    auto parser = CommandParser::parse(cmd);
    ASSERT_NE(parser, nullptr);
    EXPECT_EQ(parser->command(), cmd);

    /* macros */
    auto macros = parser->macros();
    ASSERT_EQ(macros.size(), 3);
    auto has_macro = [&](std::string_view name) {
        return std::any_of(macros.begin(), macros.end(), [&](const auto& p) {
            return p.first == name && p.second.empty();
        });
    };
    EXPECT_TRUE(has_macro("UNICODE"));
    EXPECT_TRUE(has_macro("_UNICODE"));
    EXPECT_TRUE(has_macro("_WINDOWS"));

    /* include paths */
    auto incs = parser->include_paths();
    ASSERT_EQ(incs.size(), 2);
    EXPECT_EQ(incs[0], R"(..\include)");
    EXPECT_EQ(incs[1], R"(C:\vcpkg\installed\x64-windows\include)");

    /* input files */
    auto inputs = parser->input_files();
    ASSERT_EQ(inputs.size(), 3);
    EXPECT_EQ(inputs[0], "main.cpp");
    EXPECT_EQ(inputs[1], "file1.cpp");
    EXPECT_EQ(inputs[2], "file2.cpp");

    /* output files */
    auto outs = parser->output_files();
    ASSERT_EQ(outs.size(), 2);
    EXPECT_EQ(outs[0], R"(build\obj\)");
    EXPECT_EQ(outs[1], R"(build\bin\app.exe)");

    /* options */
    auto opts = parser->options();
    auto has_opt = [&](std::string_view k, std::string_view v = "") {
        return std::find_if(opts.begin(), opts.end(), [&](const auto& p) {
                   return p.first == k && p.second == v;
               }) != opts.end();
    };
    EXPECT_TRUE(has_opt("nologo"));
    EXPECT_TRUE(has_opt("W4"));
    EXPECT_TRUE(has_opt("std", "c++20"));
    EXPECT_TRUE(has_opt("Z7"));
    EXPECT_TRUE(has_opt("Od"));
    EXPECT_TRUE(has_opt("FS"));
    EXPECT_TRUE(has_opt("EHsc"));
}