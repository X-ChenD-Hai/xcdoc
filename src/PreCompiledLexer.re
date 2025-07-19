// re2c --lang c++
#include <assert.h>

#include "PreCompiledLexer.h"
#include "utils.h"
/*!include:re2c "def.re" */

PreCompiledLexer::PreCompiledLexer(const std::string *content)
    : __content(content),
      start(content->c_str()),
      YYCURSOR(content->c_str()),
      pre_cursor(YYCURSOR),
      YYMARKER(YYCURSOR),
      last_cursor(YYCURSOR),
      limit(YYCURSOR + content->size()) {
    // parse();
}

const char *PreCompiledLexer::next() {
    using namespace std;
    if (__macro_end) {
        if (pre_cursor < __macro_end) {
            return pre_cursor++;
        } else {
            __macro_end = nullptr;
            YYCURSOR = macro_expansion_stack.top().first;
            last_cursor = macro_expansion_stack.top().second;
            macro_expansion_stack.pop();
        }
    } else if (pre_cursor < YYCURSOR) {
        return pre_cursor++;
    }
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
            const char *en = nullptr;
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
                __include_blocks.back().include_path = string(YYCURSOR, en);
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
            auto ns = ")" +
                      (__content->substr(
                          ss.start + 1,
                          __content->find('(', ss.start) - ss.start - 1)) +
                      "\"";
            auto pos = __content->find(ns, ss.start) + ns.size();
            line += std::count(start + ss.start, start + pos, L'\n');

            OUT NV(ns) NV(line) ENDL;
            OUT NV(pos) ENDL;
            OUT NV(__content->find(ns, ss.start)) ENDL;
            OUT NV(__content->at(pos - 1)) ENDL;
            OUT NV(__content->at(pos)) ENDL;
            OUT NV(__content[pos + 1]) ENDL;
            YYCURSOR = start + pos;
            OUT NV(YYCURSOR) ENDL;
            ss.length = pos - ss.start;
            in_name_force_string = false;
            if (in_macro_define) {
                __string_blocks.pop_back();
            }
            goto loop_continue;
        }

        /*!re2c
            re2c:define:YYCTYPE = char;
            re2c:yyfill:enable = 0;
            whitespace = [ \t]*;
            ident     = ident_;
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
                    (size_t)(last_cursor-start);
                __macro_define_blocks.back().ident_start =
                    (size_t)(YYCURSOR-start);
                __macro_define_blocks.back().start_line = line;
                goto loop_continue; }
            "(" {
                if(in_block_comment||
                    in_line_comment||
                    in_condition_line||
                    in_condition_endif_line||
                    in_string)
                    goto loop_continue;
                else if(in_macro_define){
                    __macro_define_blocks.back().is_function = true;
                    in_macro_param_define = true;
                    goto loop_continue;
                }
                break;
            }
            whitespace "##" whitespace {
                if(in_block_comment||
                    in_line_comment||
                    in_condition_line||
                    in_condition_endif_line||
                    in_string)
                    goto loop_continue;
                else if(in_macro_define){
                    cur_macro_param_ref_type = MacroParamRefType::Concat;
                    goto loop_continue;
                }
                break;
            }
            whitespace "#" whitespace {
                if(in_block_comment||
                    in_line_comment||
                    in_condition_line||
                    in_condition_endif_line||
                    in_string)
                    goto loop_continue;
                else if(in_macro_define){
                    cur_macro_param_ref_type = MacroParamRefType::ToString;
                    goto loop_continue;
                }
                break;
            }
            ")" {
                if(in_block_comment||
                    in_line_comment||
                    in_condition_line||
                    in_condition_endif_line||
                    in_string)
                    goto loop_continue;
                else if(in_macro_define){
                    in_macro_param_define = false;
                    goto loop_continue;
                }
                break;
            }
            ident {
                if(in_block_comment||in_line_comment||in_string) goto
                    loop_continue;
                    auto
         ident=__content->substr(last_cursor-start,YYCURSOR-last_cursor);
                if(in_macro_define){
                    if(__macro_define_blocks.back().ident_length==0){
                        __macro_define_blocks.back().ident_length =
                            (size_t)(YYCURSOR-start)-__macro_define_blocks.back().ident_start;
                        __macro_define_map[__content->substr(__macro_define_blocks.back().ident_start,
                            __macro_define_blocks.back().ident_length)]=
                            __macro_define_blocks.size()-1;
                        __macro_define_blocks.back().body_start =
                            (size_t)(YYCURSOR-start);
                    }else if(in_macro_param_define){
                        __macro_define_blocks.back().params.emplace_back(
                            last_cursor-start,
                            YYCURSOR-last_cursor,
                            std::vector<MacroParamRef>{}
                        );
                    }else if(auto it=std::find_if(
                                                __macro_define_blocks.back().params.begin(),
                                                __macro_define_blocks.back().params.end(),
                                            [&](const auto& b){
                                                OUT VV("compare: ")
         SV(mm,__content->substr(b.start,b.length)) NV(ident) ENDL; return
                                                __content->substr(b.start,b.length)==ident;
                                            });
                                        it!=__macro_define_blocks.back().params.end()){
                        OUT VV("find: ") SV(mm,ident) ENDL;
                        it->refs.emplace_back(
                            last_cursor-start,
                            YYCURSOR-last_cursor,
                            cur_macro_param_ref_type);
                            cur_macro_param_ref_type =
         MacroParamRefType::Normal;
                    }
                }else if(auto it=__macro_define_map.find(ident);
                        it!=__macro_define_map.end()){
                        __macro_idents.emplace_back(
                            last_cursor-start,
                            YYCURSOR-last_cursor,
                            line,
                            it->second
                        );
                        last_cursor = YYCURSOR;
                        macro_expansion_stack.emplace(
                            YYCURSOR,
                            last_cursor
                        );
                        pre_cursor = start+
                            __macro_define_blocks[it->second].body_start;
                        __macro_end = start +
         __macro_define_blocks[it->second].start+
                            __macro_define_blocks[it->second].length;
                        return pre_cursor++;
                }else if(!(in_block_comment|| in_line_comment||
         in_condition_line|| in_condition_endif_line|| in_string)) break;

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
            "\\" {
                in_translation_unit = !in_translation_unit; goto
         loop_continue; }
            "//" {
                if(in_block_comment||in_line_comment||in_string||in_macro_define)
                    goto loop_continue;
                in_line_comment = true;
                __line_comment_blocks.emplace_back(PreCompiledBlock{});
                __line_comment_blocks.back().start =
                    (size_t)(YYMARKER-start);
                __line_comment_blocks.back().start_line = line;
                goto loop_continue;
             }
            [ \v\t\r]+  {
                if(!(in_block_comment||
                    in_line_comment||
                    in_condition_line||
                    in_condition_endif_line||
                    in_string||in_macro_define))
                    break;
                goto loop_continue; }
            [\x00\n] {
                line++;
                if(in_translation_unit){
                    in_translation_unit = false;
                    goto end_line; }
                if(!(in_block_comment||
                    in_line_comment||
                    in_condition_line||
                    in_condition_endif_line||
                    in_string||in_macro_define))
                    break;

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
                        (size_t)(YYCURSOR-start)-__macro_define_blocks.back().start-1;
                    __macro_define_blocks.back().end_line = line-1;
                    in_macro_define = false;
                }
                end_line:
                if(*(YYCURSOR-1) == 0)
                    break;
                goto loop_continue;
             }
            *      {
                if(in_block_comment||
                    in_line_comment||
                    in_condition_line||
                    in_condition_endif_line||
                    in_string||
                    in_macro_define)
                    goto loop_continue;
                break;
            }
        */
    loop_continue:
        last_cursor = YYCURSOR;
    }
    pre_cursor = last_cursor;
    last_cursor = YYCURSOR;
    return pre_cursor++;
}
void PreCompiledLexer::__parse_define_block() {
    auto &block = __macro_define_blocks.back();
}
