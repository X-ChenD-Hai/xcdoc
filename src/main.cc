#include "PreCompiledLexer.h"
#include "utils.h"

int main() {
    using namespace std;
    system("cls");

    PreCompiledLexer lexer(XCDOC_TESTS_RESOURCE_DIR "/test.cc");
    OUT VV("Include blocks:") ENDL;
    for (auto header : lexer.include_blocks()) {
        WOUT NV(header.start) NV(header.length) NV(header.include_path)
            NV(header.start_line) NV(header.end_line) ENDL;
    }
    OUT VV("Macro define blocks:") ENDL;
    for (auto block : lexer.macro_define_blocks()) {
        WOUT NV(block.start) NV(block.length) NV(block.ident_start)
            NV(block.ident_length) NV(block.start_line) NV(block.end_line) ENDL;
    }
    OUT VV("String blocks:") ENDL;
    for (auto block : lexer.string_blocks()) {
        WOUT NV(block.start) NV(block.length) NV(block.start_line)
            NV(block.end_line) ENDL;
    }
    OUT VV("Line comment blocks:") ENDL;
    for (auto block : lexer.line_comment_blocks()) {
        WOUT NV(block.start) NV(block.length) NV(block.start_line)
            NV(block.end_line) ENDL;
    }
    OUT VV("Block comment blocks:") ENDL;
    for (auto block : lexer.block_comment_blocks()) {
        WOUT NV(block.start) NV(block.length) NV(block.start_line)
            NV(block.end_line) ENDL;
    }
    OUT VV("Macro define map:") ENDL;
    for (auto& block : lexer.macro_define_map()) {
        WOUT NV(block.first) NV(block.second) ENDL;
    }
    OUT VV("Macro Identifiers:") ENDL;
    for (auto& i : lexer.macro_idents()) {
        WOUT NV(i.start) NV(i.length) NV(i.line) NV(i.macro_id) ENDL;
    }

    return 0;
}