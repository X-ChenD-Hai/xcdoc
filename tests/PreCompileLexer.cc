#include <string>

#include "PreCompiledLexer.h"
#include "utils.h"

int main() {
    std::wstring ss{L"12"};
    auto s = ss.begin();

#ifdef __xcdoc_debug__
    using namespace std;
    system("cls");
    auto sorce = new std::wstring(
        utiles::read_file(XCDOC_TESTS_RESOURCES_DIR "testPreCompileLexer.cc"));
    PreCompiledLexer lexer(sorce);
    OUT VV("------ Include blocks:") ENDL;
    for (auto header : lexer.include_blocks()) {
        WOUT NV(header.start) NV(header.length) NV(header.include_path)
            NV(header.start_line) NV(header.end_line) ENDL;
    }
    OUT VV("------ Macro define blocks:") ENDL;
    for (auto block : lexer.macro_define_blocks()) {
        WOUT NV(block.start) NV(block.length) NV(block.ident_start)
            NV(block.ident_length) NV(block.start_line) NV(block.end_line) ENDL;
    }
    OUT VV("------ String blocks:") ENDL;
    for (auto block : lexer.string_blocks()) {
        WOUT NV(block.start) NV(block.length) NV(block.start_line)
            NV(block.end_line) ENDL;
    }
    OUT VV("------ Line comment blocks:") ENDL;
    for (auto block : lexer.line_comment_blocks()) {
        WOUT NV(block.start) NV(block.length) NV(block.start_line)
            NV(block.end_line) ENDL;
    }
    OUT VV("------ Block comment blocks:") ENDL;
    for (auto block : lexer.block_comment_blocks()) {
        WOUT NV(block.start) NV(block.length) NV(block.start_line)
            NV(block.end_line) ENDL;
    }
    OUT VV("------ Macro define map:") ENDL;
    for (auto& block : lexer.macro_define_map()) {
        WOUT NV(block.first) NV(block.second) ENDL;
    }
    OUT VV("------ Macro Identifiers:") ENDL;
    for (auto& i : lexer.macro_idents()) {
        WOUT NV(i.start) NV(i.length) NV(i.line) NV(i.macro_id) ENDL;
    }
    OUT VV("------ Macro Conditions:") ENDL;
    for (auto& i : lexer.condition_blocks()) {
        [](this auto&& self, const PreCompiledLexer::ConditonBlock& i,
           const std::wstring& s, const std::wstring& s1) -> void {
            WOUT VV(s) SV(s, i.start) SV(l, i.length) SV(sl, i.start_line)
                SV(el, i.end_line) ENDL;
            for (auto& j : i.blocks) {
                WOUT VV(s1) VV("-- block: ") SV(s, j.start) SV(l, j.length)
                    SV(sl, j.start_line) SV(el, j.end_line)
                        SV(cl, j.condition_length) ENDL;
                for (auto& k : j.sub_blocks) {
                    self(k, s + L"\t ", s1 + L"\t  |");
                }
            }
        }(i, L"", L" |");
    }

#endif
    return 0;
}