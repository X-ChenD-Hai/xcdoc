// re2c --lang c++
#include <assert.h>

#include <stack>

#include "PreCompiledLexer.h"
#include "utils.h"

PreCompiledLexer::PreCompiledLexer(const std::wstring *content)
    : content(content) {
    parse();
}
void PreCompiledLexer::parse() {
    using namespace std;
    auto start = content->c_str();
    auto YYCURSOR = content->c_str();
    auto YYMARKER = YYCURSOR;
    auto last_cursor = YYCURSOR;
    auto limit = YYCURSOR + content->size();
    bool in_include_block = false;
    bool in_macro_define = false;
    bool in_line_comment = false;
    bool in_block_comment = false;
    bool in_string = false;
    bool in_translation_unit = false;
    bool in_name_force_string = false;
    bool in_condition_line = false;
    bool in_condition_endif_line = false;
    std::stack<ConditonBlock> condition_block_stack;
    size_t line = 1;
#ifdef __xcdoc_debug__
    auto show_state = [&](const std::string &msg) {
        ERR VV(msg) ENDL;
        WERR NV(*(YYCURSOR - 1)) ENDL;
        WERR NV(YYCURSOR) ENDL;
        WERR NV(YYMARKER) ENDL;
        ERR NV(in_macro_define) ENDL;
        ERR NV(in_line_comment) ENDL;
        ERR NV(in_block_comment) ENDL;
        ERR NV(in_string) ENDL;
        ERR NV(in_translation_unit) ENDL;
    };
#define SHOW_STATE(msg)                                                       \
    show_state(std::string(__FILE__) + ":" + std::to_string(__LINE__) + " " + \
               msg);
#endif
    for (;;) {
        if (in_include_block) {
            const wchar_t *en = nullptr;
            if (YYCURSOR < limit) {
                if (*(YYCURSOR - 1) == L'"')
                    en = std::find(YYCURSOR, limit, L'"');
                else if (*(YYCURSOR - 1) == L'<')
                    en = std::find(YYCURSOR, limit, L'>');
                else
                    assert(false && "Invalid include path");
                if (en == limit) {
                    ERR VV("Invalid include path") ENDL;
                    break;
                }
                __include_blocks.back().include_path = wstring(YYCURSOR, en);
                __include_blocks.back().length =
                    (size_t)(en - start - __include_blocks.back().start);
                YYCURSOR = en + 1;
                in_include_block = false;
                goto loop_continue;
            } else {
                ERR VV("Invalid include path") ENDL;
                break;
            }
        }
        if (in_name_force_string) {
            auto &ss = __string_blocks.back();
            auto ns =
                L")" +
                (content->substr(ss.start + 1,
                                 content->find('(', ss.start) - ss.start - 1)) +
                L"\"";
            auto pos = content->find(ns, ss.start) + ns.size();
            line += std::count(start + ss.start, start + pos, L'\n');

            WOUT NV(ns) NV(line) ENDL;
            WOUT NV(pos) ENDL;
            WOUT NV(content->find(ns, ss.start)) ENDL;
            WOUT NV(content->at(pos - 1)) ENDL;
            WOUT NV(content->at(pos)) ENDL;
            WOUT NV(content[pos + 1]) ENDL;
            YYCURSOR = start + pos;
            WOUT NV(YYCURSOR) ENDL;
            ss.length = pos - ss.start;
            in_name_force_string = false;
            if (in_macro_define) {
                __string_blocks.pop_back();
            }
            goto loop_continue;
        }

        /*!re2c
            re2c:define:YYCTYPE = wchar_t;
            re2c:yyfill:enable = 0;
            re2c:flags:utf-8 = 1;
            whitespace = [ \t]*;
            // ZH = [\u4e2d-\u9fa5];
            IH = [a-zA-Z_];
            IT = (IH|[0-9]);
            ident     = IH IT*;
            "#" whitespace "if"{
                if(in_block_comment||in_line_comment||in_string)
                    goto loop_continue;
                in_condition_line = true;
                ConditonBlock condition_block{};
                condition_block.start = last_cursor-start;
                condition_block.start_line = line;
                ConditonItemBlock condition_item_block{};
                condition_item_block.start = last_cursor-start;
                condition_item_block.start_line = line;
                condition_block.blocks.push_back(condition_item_block);
                condition_block_stack.push(condition_block);
                goto loop_continue;
            }
            "#" whitespace "ifdef"{
                if(in_block_comment||in_line_comment||in_string)
                    goto loop_continue;
                in_condition_line = true;
                ConditonBlock condition_block{};
                condition_block.start = last_cursor-start;
                condition_block.start_line = line;
                ConditonItemBlock condition_item_block{};
                condition_item_block.start = last_cursor-start;
                condition_item_block.start_line = line;
                condition_block.blocks.push_back(condition_item_block);
                condition_block_stack.push(condition_block);
                goto loop_continue;

            }
            "#" whitespace "elif"{
                if(in_block_comment||in_line_comment||in_string)
                    goto loop_continue;
                in_condition_line = true;
                condition_block_stack.top().blocks.back().length =
                    last_cursor-start-condition_block_stack.top().blocks.back().start;
                condition_block_stack.top().blocks.back().end_line = line-1;
                ConditonItemBlock condition_item_block{};
                condition_item_block.start = last_cursor-start;
                condition_item_block.start_line = line;
                condition_block_stack.top().blocks.push_back(condition_item_block);
                goto loop_continue;

            }
            "#" whitespace "elifdef"{
                if(in_block_comment||in_line_comment||in_string)
                    goto loop_continue;
                in_condition_line = true;
                condition_block_stack.top().blocks.back().length =
                    last_cursor-start-condition_block_stack.top().blocks.back().start;
                condition_block_stack.top().blocks.back().end_line = line-1;
                ConditonItemBlock condition_item_block{};
                condition_item_block.start = last_cursor-start;
                condition_item_block.start_line = line;
                condition_block_stack.top().blocks.push_back(condition_item_block);
                goto loop_continue;
            }
            "#" whitespace "else"{
                if(in_block_comment||in_line_comment||in_string)
                    goto loop_continue;
                in_condition_line = true;
                condition_block_stack.top().blocks.back().length =
                    last_cursor-start-condition_block_stack.top().blocks.back().start;
                condition_block_stack.top().blocks.back().end_line = line-1;
                ConditonItemBlock condition_item_block{};
                condition_item_block.start = last_cursor-start;
                condition_item_block.start_line = line;
                condition_block_stack.top().blocks.push_back(condition_item_block);
                goto loop_continue;

            }
            "#" whitespace "endif"{
                if(in_block_comment||in_line_comment||in_string)
                    goto loop_continue;
                if(!condition_block_stack.empty()){
                    condition_block_stack.top().blocks.back().length =
                        last_cursor-start-condition_block_stack.top().blocks.back().start;
                    condition_block_stack.top().blocks.back().end_line = line-1;
                }

                in_condition_endif_line = true;
                goto loop_continue;

            }
            "#" whitespace "include" whitespace [<"] {
                if(in_block_comment||in_line_comment||in_string)
                    goto loop_continue;
                in_include_block = true;
                __include_blocks.emplace_back(IncludeBlock{});
                __include_blocks.back().start = (size_t)(YYMARKER-start);
                __include_blocks.back().start_line = line;
                __include_blocks.back().end_line = line;
                goto loop_continue; }
            "#" whitespace "define" [ \t]+  {
                if(in_block_comment||in_line_comment||in_string)
                    goto loop_continue;
                ERR VV("Start macro define") ENDL;
                 in_macro_define = true;
                __macro_define_blocks.emplace_back(MacroDefineBlock{});
                __macro_define_blocks.back().start =
                    (size_t)(YYMARKER-start);
                __macro_define_blocks.back().ident_start =
                    (size_t)(YYCURSOR-start);
                __macro_define_blocks.back().start_line = line;
                goto loop_continue; }
            ident {
                if(in_block_comment||in_line_comment||in_string) goto
                    loop_continue;
                if(in_macro_define&&__macro_define_blocks.back().ident_length==0){
                    __macro_define_blocks.back().ident_length =
                        (size_t)(YYCURSOR-start)-__macro_define_blocks.back().ident_start;
                    __macro_define_map[content->substr(__macro_define_blocks.back().ident_start,
                        __macro_define_blocks.back().ident_length)]=
                        __macro_define_blocks.size()-1;
                }else {
                    std::wstring ident =
                        content->substr(last_cursor-start,YYCURSOR-last_cursor);
                        if(auto it=__macro_define_map.find(ident);
                            it!=__macro_define_map.end()){
                            __macro_idents.emplace_back(
                                last_cursor-start,
                                YYCURSOR-last_cursor,
                                line,
                                it->second
                            );
                            WOUT NV(ident) ENDL;
                        }
                }
                goto loop_continue; }
            [/][*] {
                if(in_block_comment||in_line_comment||in_string)
                    goto loop_continue;
                __block_comment_blocks.emplace_back(PreCompiledBlock{});
                __block_comment_blocks.back().start =
                    (size_t)(YYMARKER-start);
                __block_comment_blocks.back().start_line = line;
                in_block_comment = true; goto loop_continue;
             }
            [*][/] {
                if(in_block_comment){
                    in_block_comment = false;
                    __block_comment_blocks.back().length =
                        (size_t)(YYCURSOR-start)-__block_comment_blocks.back().start;
                    __block_comment_blocks.back().end_line = line;
                }
                goto loop_continue;
             }
             "R"["] [^\x00\t\n\r \\(]* "(" {
                ERR VV("-------------Nwe Block--------") ENDL;
                if(in_block_comment||in_line_comment||in_string)
                    goto loop_continue;
                __string_blocks.emplace_back(PreCompiledBlock{});
                __string_blocks.back().start = (size_t)(YYMARKER-start);
                __string_blocks.back().start_line = line;
                in_name_force_string = true;
                goto loop_continue;
             }
            ["] {
                SHOW_STATE(" Start string");
                if(in_translation_unit){
                    in_translation_unit = false;
                    ERR VV("End translation unit") ENDL;
                    goto loop_continue;
                }
                if(in_block_comment||in_line_comment||in_macro_define)
                    goto loop_continue;
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
                goto loop_continue;
         }
            "\\" {  in_translation_unit = !in_translation_unit; goto
         loop_continue; }
            "//" {
                if(in_block_comment||in_line_comment||in_string||in_macro_define)
                    goto loop_continue; in_line_comment = true;
                __line_comment_blocks.emplace_back(PreCompiledBlock{});
                __line_comment_blocks.back().start =
                    (size_t)(YYMARKER-start);
                __line_comment_blocks.back().start_line = line;
                goto loop_continue;
             }
            [ \v\t\r]+  { goto loop_continue; }
            [\x00\n] {
                line++;
                if(in_translation_unit){
                    in_translation_unit = false;
                    goto end_line; }
                if(in_condition_line) {
                    condition_block_stack.top().blocks.back().condition_length =
                        (YYCURSOR-start)-condition_block_stack.top().blocks.back().start;
                    in_condition_line = false;
                }
                if(in_condition_endif_line) {
                    in_condition_endif_line = false;
                    OUT VV("End condition block") ENDL;
                    auto b = condition_block_stack.top();
                    b.end_line = line-1;
                    b.length = last_cursor-start-b.start;
                    condition_block_stack.pop();
                    if(condition_block_stack.empty()){
                        __condition_blocks.push_back(b);
                    }else{
                        if(condition_block_stack.top().blocks.size()){
                           condition_block_stack.top().blocks.back().sub_blocks.push_back(b);
                        }
                    }
                }
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
                end_line:
                if(*(YYCURSOR-1) == 0)
                    break;
                goto loop_continue;
             }
            *      { goto loop_continue; }
        */
    loop_continue:
        last_cursor = YYCURSOR;
    }
}
