#include <string>

#include "PreCompiledLexer.h"
#include "utils.h"

int main() {
    std::string ss{"12"};
    auto s = ss.begin();

#ifdef __xcdoc_debug__
    using namespace std;
    system("cls");
    auto sorce = new std::string(
        utiles::read_file(XCDOC_TESTS_RESOURCES_DIR "testPreCompileLexer.cc"));
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
        OUT NV(header.start) NV(header.length) NV(header.include_path)
            NV(header.start_line) NV(header.end_line) ENDL;
    }
    OUT VV("------ Macro define blocks:") ENDL;
    for (auto block : lexer.macro_define_blocks()) {
        OUT NV(block.start) NV(block.length) NV(block.ident_start)
            NV(block.ident_length) NV(block.end_line) SV(f, block.is_function)
                ENDL;
        if (block.is_function) {
            for (auto& p : block.params) {
                OUT VV("\t")
                    SV(pname, lexer.content().substr(p.start, p.length)) ENDL;
                for (auto& r : p.refs) {
                    OUT VV("\t\tref: ") NV(r.start) NV(r.length) SV(
                        type,
                        r.type == PreCompiledLexer::MacroParamRefType::Normal
                            ? "Normal"
                        : r.type == PreCompiledLexer::MacroParamRefType::Concat
                            ? "Concat"
                            : "ToString") ENDL;
                }
            }
        }
    }
    OUT VV("------ String blocks:") ENDL;
    for (auto block : lexer.string_blocks()) {
        OUT NV(block.start) NV(block.length) NV(block.start_line)
            NV(block.end_line) ENDL;
    }
    OUT VV("------ Line comment blocks:") ENDL;
    for (auto block : lexer.line_comment_blocks()) {
        OUT NV(block.start) NV(block.length) NV(block.start_line)
            NV(block.end_line) ENDL;
    }
    OUT VV("------ Block comment blocks:") ENDL;
    for (auto block : lexer.block_comment_blocks()) {
        OUT NV(block.start) NV(block.length) NV(block.start_line)
            NV(block.end_line) ENDL;
    }
    OUT VV("------ Macro define map:") ENDL;
    for (auto& block : lexer.macro_define_map()) {
        OUT NV(block.first) NV(block.second) ENDL;
    }
    OUT VV("------ Macro Identifiers:") ENDL;
    for (auto& i : lexer.macro_idents()) {
        OUT NV(i.start) NV(i.length) NV(i.line) NV(i.macro_id) ENDL;
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

#endif
    return 0;
}