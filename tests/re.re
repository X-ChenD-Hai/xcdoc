// re2c $INPUT -o $OUTPUT
#include <assert.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#define __debug__
#ifdef __debug__
#define ERR std::cerr
#define WERR std::wcerr
#define OUT std::cout
#define WOUT std::wcout
#define NV(v) << #v << ": " << (v) << ", "
#define SV(n, v) << #n << ": " << (v) << ", "
#define VV(v) << (v) << " "
#define ENDL << std::endl
#define SHOW_STATE(msg)                                                       \
    show_state(std::string(__FILE__) + ":" + std::to_string(__LINE__) + " " + \
               msg);
#else
#define ERR
#define OUT
#define OUT
#define ERR
#define NV(v)
#define SV(n, v)
#define VV(v)
#define ENDL
#define SHOW_STATE(msg)
#endif
namespace utiles {
std::string read_file(const std::string &filename) {
    std::ifstream ifs(filename);
    if (!ifs) {
        ERR VV("Failed to open file: ") VV(filename) ENDL;
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
    return content;
}
}  // namespace utiles

class PreCompiledLexer {
   public:
    struct PreCompiledBlock {
        size_t start;
        size_t length;
        size_t start_line;
        size_t end_line;
    };

    struct MacroDefineBlock : public PreCompiledBlock {
        size_t ident_start;
        size_t ident_length;
    };
    struct IncludeBlock : public PreCompiledBlock {
        std::string include_path;
    };

   private:
    std::string path;
    std::string content;
    std::unordered_map<std::string, size_t> __macro_define_map;
    std::vector<IncludeBlock> __include_blocks;
    std::vector<MacroDefineBlock> __macro_define_blocks;
    std::vector<PreCompiledBlock> __string_blocks;
    std::vector<PreCompiledBlock> __line_comment_blocks;
    std::vector<PreCompiledBlock> __block_comment_blocks;

   public:
    PreCompiledLexer(const std::string &path)
        : path(path), content(utiles::read_file(path)) {
        parse();
    }

    void parse() {
        using namespace std;
        auto start = content.c_str();
        auto YYCURSOR = content.c_str();
        auto YYMARKER = YYCURSOR;
        auto limit = YYCURSOR + content.size();
        bool in_include_block = false;
        bool in_macro_define = false;
        bool in_line_comment = false;
        bool in_block_comment = false;
        bool in_string = false;
        bool in_translation_unit = false;
        bool in_name_force_string = false;
        size_t line = 1;
#ifdef __debug__
        auto show_state = [&](const std::string &msg) {
            ERR VV(msg) ENDL;
            ERR NV(*(YYCURSOR - 1)) ENDL;
            ERR NV(YYCURSOR) ENDL;
            ERR NV(YYMARKER) ENDL;
            ERR NV(in_macro_define) ENDL;
            ERR NV(in_line_comment) ENDL;
            ERR NV(in_block_comment) ENDL;
            ERR NV(in_string) ENDL;
            ERR NV(in_translation_unit) ENDL;
        };
#endif
        for (;;) {
            if (in_include_block) {
                const char *en = nullptr;
                if (YYCURSOR < limit) {
                    if (*(YYCURSOR - 1) == '"')
                        en = std::find(YYCURSOR, limit, '"');
                    else if (*(YYCURSOR - 1) == '<')
                        en = std::find(YYCURSOR, limit, '>');
                    else
                        assert(false && "Invalid include path");
                    if (en == limit) {
                        ERR VV("Invalid include path") ENDL;
                        break;
                    }
                    __include_blocks.back().include_path = string(YYCURSOR, en);
                    __include_blocks.back().length =
                        (size_t)(en - start - __include_blocks.back().start);
                    YYCURSOR = en + 1;
                    in_include_block = false;
                } else {
                    ERR VV("Invalid include path") ENDL;
                    break;
                }
            }
            if (in_name_force_string) {
                auto &ss = __string_blocks.back();
                auto ns =
                    ")" +
                    (content.substr(ss.start + 1, content.find('(', ss.start) -
                                                      ss.start - 1)) +
                    "\"";
                auto pos = content.find(ns, ss.start) + ns.size();
                line += std::count(start + ss.start, start + pos, '\n');

                OUT NV(ns) NV(line) ENDL;
                OUT NV(pos) ENDL;
                OUT NV(content.find(ns, ss.start)) ENDL;
                OUT NV(content[pos - 1]) ENDL;
                OUT NV(content[pos]) ENDL;
                OUT NV(content[pos + 1]) ENDL;
                YYCURSOR = start + pos;
                OUT NV(YYCURSOR) ENDL;
                ss.length = pos - ss.start;
                in_name_force_string = false;
                if (in_macro_define) {
                    __string_blocks.pop_back();
                }
            }

            /*!re2c

                re2c:define:YYCTYPE = char;
                re2c:yyfill:enable = 0;
                re2c:flags:utf-8 = 1;
                whitespace = [ \t]*;
                ZH = [\x4e][\x2d];
                IH = (ZH|[a-zA-Z_]);
                IT = (IH|[0-9]);
                ident     = IH IT*;

                "#" whitespace "include" whitespace [<"] {
                    if(in_block_comment||in_line_comment||in_string) continue;
                    in_include_block = true;
                    __include_blocks.emplace_back(IncludeBlock{});
                    __include_blocks.back().start = (size_t)(YYMARKER-start);
                    __include_blocks.back().start_line = line;
                    __include_blocks.back().end_line = line;
                    continue; }
                "#" whitespace "define" [ \t]+  {
                    if(in_block_comment||in_line_comment||in_string) continue;
                    ERR VV("Start macro define") ENDL;
                    in_macro_define = true;
                    __macro_define_blocks.emplace_back(MacroDefineBlock{});
                    __macro_define_blocks.back().start =
                        (size_t)(YYMARKER-start);
                    __macro_define_blocks.back().ident_start =
                        (size_t)(YYCURSOR-start);
                    __macro_define_blocks.back().start_line = line;
                    continue; }
                ident {
                    if(in_block_comment||in_line_comment||in_string) continue;
                    if(in_macro_define&&__macro_define_blocks.back().ident_length==0){
                        OUT NV(YYCURSOR) ENDL;
                        __macro_define_blocks.back().ident_length =
                            (size_t)(YYCURSOR-start)-__macro_define_blocks.back().ident_start;
                        __macro_define_map[content.substr(__macro_define_blocks.back().ident_start,
                            __macro_define_blocks.back().ident_length)]=
                            __macro_define_blocks.size()-1;
                    }
                    continue; }
                [/][*] {
                    if(in_block_comment||in_line_comment||in_string) continue;
                    __block_comment_blocks.emplace_back(PreCompiledBlock{});
                    __block_comment_blocks.back().start =
                        (size_t)(YYMARKER-start);
                    __block_comment_blocks.back().start_line = line;
                    in_block_comment = true; continue;
                 }
                [*][/] {
                    if(in_block_comment){
                        in_block_comment = false;
                        __block_comment_blocks.back().length =
                            (size_t)(YYCURSOR-start)-__block_comment_blocks.back().start;
                        __block_comment_blocks.back().end_line = line;
                }
                continue;
                 }
                 "R"["] [^\x00\t\n\r \\(]* "(" {
                    ERR VV("-------------Nwe Block--------") ENDL;
                     if(in_block_comment||in_line_comment||in_string) continue;
                     __string_blocks.emplace_back(PreCompiledBlock{});
                     __string_blocks.back().start = (size_t)(YYMARKER-start);
                     __string_blocks.back().start_line = line;
                     in_name_force_string = true;
                    continue;
                 }
                ["] {
                    SHOW_STATE(" Start string");
                    if(in_translation_unit){
                        in_translation_unit = false;
                        ERR VV("End translation unit") ENDL;
                        continue;
                    }
                    if(in_block_comment||in_line_comment||in_macro_define)continue;
                    in_string = !in_string;
                    if(in_string){
                        __string_blocks.emplace_back(PreCompiledBlock{});
                        __string_blocks.back().start = (size_t)(YYMARKER-start);
                        __string_blocks.back().start_line = line;
                    }else{
                        __string_blocks.back().length =
                        (size_t)(YYCURSOR-start)-__string_blocks.back().start;
                        __string_blocks.back().end_line = line;
                    }
                    continue;
             }
                "\\" {  in_translation_unit = !in_translation_unit; continue; }
                "//" {
                    if(in_block_comment||in_line_comment||in_string||in_macro_define)
                        continue; in_line_comment = true;
                    __line_comment_blocks.emplace_back(PreCompiledBlock{});
                    __line_comment_blocks.back().start =
                        (size_t)(YYMARKER-start);
                    __line_comment_blocks.back().start_line = line;
                    continue;
                 }
                "\n" {
                    line++;
                    if(in_translation_unit){
                        in_translation_unit = false;
                        continue; }
                    if(in_string){
                        SHOW_STATE("End string With newline");
                       in_string=false; }
                    if(in_line_comment){
                        in_line_comment = false;
                        __line_comment_blocks.back().length =
                            (size_t)(YYCURSOR-start)-__line_comment_blocks.back().start;
                        __line_comment_blocks.back().end_line = line-1;
                    }
                    if(in_macro_define){
                        ERR VV("End macro define") ENDL;
                        __macro_define_blocks.back().length =
                            (size_t)(YYCURSOR-start)-__macro_define_blocks.back().start;
                        __macro_define_blocks.back().end_line = line-1;
                        in_macro_define = false;
                    }
                    continue;
                  }
                [ \t\r]+  { continue; }
                [\u0000] { break; }
                *      { continue; }
            */
        }
    }

    inline const std::vector<IncludeBlock> &include_blocks() const {
        return __include_blocks;
    }
    inline const std::vector<MacroDefineBlock> &macro_define_blocks() const {
        return __macro_define_blocks;
    }
    inline const std::vector<PreCompiledBlock> &string_blocks() const {
        return __string_blocks;
    }
    inline const std::vector<PreCompiledBlock> &line_comment_blocks() const {
        return __line_comment_blocks;
    }
    inline const std::vector<PreCompiledBlock> &block_comment_blocks() const {
        return __block_comment_blocks;
    }
    inline const std::unordered_map<std::string, size_t> &macro_define_map()
        const {
        return __macro_define_map;
    }
};

int main() {
    using namespace std;
    system("cls");

    // wcout << utiles::read_file("test.cc");
    PreCompiledLexer lexer("test.cc");
    OUT VV("Include blocks:") ENDL;
    for (auto header : lexer.include_blocks()) {
        OUT NV(header.start) NV(header.length) NV(header.include_path)
            NV(header.start_line) NV(header.end_line) ENDL;
    }
    OUT VV("Macro define blocks:") ENDL;
    for (auto block : lexer.macro_define_blocks()) {
        OUT NV(block.start) NV(block.length) NV(block.ident_start)
            NV(block.ident_length) NV(block.start_line) NV(block.end_line) ENDL;
    }
    OUT VV("String blocks:") ENDL;
    for (auto block : lexer.string_blocks()) {
        OUT NV(block.start) NV(block.length) NV(block.start_line)
            NV(block.end_line) ENDL;
    }
    OUT VV("Line comment blocks:") ENDL;
    for (auto block : lexer.line_comment_blocks()) {
        OUT NV(block.start) NV(block.length) NV(block.start_line)
            NV(block.end_line) ENDL;
    }
    OUT VV("Block comment blocks:") ENDL;
    for (auto block : lexer.block_comment_blocks()) {
        OUT NV(block.start) NV(block.length) NV(block.start_line)
            NV(block.end_line) ENDL;
    }
    OUT VV("Macro define map:") ENDL;
    for (auto &block : lexer.macro_define_map()) {
        OUT NV(block.first) NV(block.second) ENDL;
    }

    return 0;
}