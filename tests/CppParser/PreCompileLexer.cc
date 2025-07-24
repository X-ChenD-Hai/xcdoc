#include <CppParser/PreCompiledLexer.h>
#include <gtest/gtest.h>
#include <utils/public.h>

#include <string>

TEST(PreCompiledLexer, all) {
    std::string ss{"12"};
    auto s = ss.begin();
    using namespace std;
    system("cls");
    auto sorce = new std::string(
        utils::read_file(XCDOC_TESTS_RESOURCES_DIR "testPreCompileLexer.cc"));
    PreCompiledLexer lexer(sorce);
    std::string sss;
    for (;;) {
        if (auto c = lexer.next(); *c != 0) {
            sss.push_back(*c);
            continue;
        }
        break;
    }
    OUT VV("------------source") ENDL;
    OUT VV(sss) ENDL;
    OUT VV("------------source") ENDL;

    OUT VV("------ Include blocks:") ENDL;
    for (auto header : lexer.include_blocks()) {
        OUT NV(lexer.pos(header.start)) NV(header.length)
            NV(header.include_path) NV(header.start_line) NV(header.end_line)
                ENDL;
    }
    OUT VV("------ Macro define blocks:") ENDL;
    for (auto block : lexer.macro_define_blocks()) {
        OUT SV(ident,
               lexer.content().substr(block.ident_start, block.ident_length))
            SV(s, lexer.pos(block.start)) SV(l, block.length)
                SV(bs, block.body_start) SV(el, block.end_line)
                    SV(f, block.is_function) ENDL;
        if (block.is_function) {
            for (size_t i = 0; i < block.params.size(); ++i) {
                auto& p = block.params[i];
                OUT VV("\t") SV(pname, string(p.start, p.length)) ENDL;
                for (auto& r : block.params_refs) {
                    if (r.shape_param_id == i) {
                        OUT VV("\t\tref: ") NV(lexer.pos(r.start)) NV(r.length)
                            SV(type, r.type == PreCompiledLexer::
                                                   MacroParamRefType::Normal
                                         ? "Normal"
                                     : r.type == PreCompiledLexer::
                                                     MacroParamRefType::Concat
                                         ? "Concat"
                                         : "ToString") ENDL;
                    }
                }
            }
        }
    }
    OUT VV("------ String blocks:") ENDL;
    for (auto block : lexer.string_blocks()) {
        OUT NV(lexer.pos(block.start)) NV(block.length) NV(block.start_line)
            NV(block.end_line) ENDL;
    }
    OUT VV("------ Line comment blocks:") ENDL;
    for (auto block : lexer.line_comment_blocks()) {
        OUT NV(lexer.pos(block.start)) NV(block.length) NV(block.start_line)
            NV(block.end_line) ENDL;
    }
    OUT VV("------ Block comment blocks:") ENDL;
    for (auto block : lexer.block_comment_blocks()) {
        OUT NV(lexer.pos(block.start)) NV(block.length) NV(block.start_line)
            NV(block.end_line) ENDL;
    }
    OUT VV("------ Macro define map:") ENDL;
    for (auto& block : lexer.macro_define_map()) {
        OUT NV(block.first) NV(block.second) ENDL;
    }
    OUT VV("------ Macro Identifiers:") ENDL;
    for (auto& i : lexer.macro_idents()) {
        OUT NV(lexer.pos(i.start)) NV(i.length) NV(i.line) NV(i.macro_id) ENDL;
        for (auto& p : i.real_params) {
            OUT VV("\t") SV(pname, string(p.start, p.length)) ENDL;
        }
    }
    OUT VV("------ Macro Conditions:") ENDL;
    for (auto& i : lexer.condition_blocks()) {
        [](this auto&& self, const PreCompiledLexer::ConditonBlock& i,
           const std::string& s, const std::string& s1) -> void {
            OUT VV(s) SV(s, i.start) SV(l, i.length) SV(sl, i.start_line)
                SV(el, i.end_line) ENDL;
            for (auto& j : i.blocks) {
                OUT VV(s1) VV("-- block: ") SV(s, j.start) SV(l, j.length)
                    SV(sl, j.start_line) SV(el, j.end_line)
                        SV(cl, j.condition_length) ENDL;
                for (auto& k : j.sub_blocks) {
                    self(k, s + "\t ", s1 + "\t  |");
                }
            }
        }(i, "", " |");
    }
}