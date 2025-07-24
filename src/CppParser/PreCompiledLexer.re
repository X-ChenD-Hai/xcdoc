// re2c --lang c++
#include <assert.h>

#include "CppParser/MacroExpandMacroHelper.h"
#include "CppParser/PreCompiledLexer.h"
#include "utils/public.h"

/*!include:re2c "def.re" */

#define IMPL_HANDLE(code)        \
    template <>                  \
    PreCompiledLexer::NextAction \
    PreCompiledLexer::__handle<PreCompiledLexer::TokenCode::code>()
#define FILTER_COMMENT                                                     \
    if (__state.block_comment || __state.line_comment || __state.string || \
        __state.condition_line || __state.condition_endif_line ||          \
        __state.macro_function_call)                                       \
        return NextAction::Continue;
IMPL_HANDLE(If) {
    FILTER_COMMENT
    __state.condition_line = true;
    ConditonBlock condition_block{};
    condition_block.start = last_cursor;
    condition_block.start_line = __state.line;
    ConditonItemBlock condition_item_block{};
    condition_item_block.start = last_cursor;
    condition_item_block.start_line = __state.line;
    condition_block.blocks.push_back(condition_item_block);
    __state.condition_block_stack.push(condition_block);
    return NextAction::Continue;
}
IMPL_HANDLE(Ifdef) {
    FILTER_COMMENT
    __state.condition_line = true;
    ConditonBlock condition_block{};
    condition_block.start = last_cursor;
    condition_block.start_line = __state.line;
    ConditonItemBlock condition_item_block{};
    condition_item_block.start = last_cursor;
    condition_item_block.start_line = __state.line;
    condition_block.blocks.push_back(condition_item_block);
    __state.condition_block_stack.push(condition_block);
    return NextAction::Continue;
}
IMPL_HANDLE(Elif) {
    FILTER_COMMENT
    __state.condition_line = true;
    __state.condition_block_stack.top().blocks.back().length =
        last_cursor - __state.condition_block_stack.top().blocks.back().start;
    __state.condition_block_stack.top().blocks.back().end_line =
        __state.line - 1;
    ConditonItemBlock condition_item_block{};
    condition_item_block.start = last_cursor;
    condition_item_block.start_line = __state.line;
    __state.condition_block_stack.top().blocks.push_back(condition_item_block);
    return NextAction::Continue;
}
IMPL_HANDLE(Elifdef) {
    FILTER_COMMENT
    __state.condition_line = true;
    __state.condition_block_stack.top().blocks.back().length =
        last_cursor - __state.condition_block_stack.top().blocks.back().start;
    __state.condition_block_stack.top().blocks.back().end_line =
        __state.line - 1;
    ConditonItemBlock condition_item_block{};
    condition_item_block.start = last_cursor;
    condition_item_block.start_line = __state.line;
    __state.condition_block_stack.top().blocks.push_back(condition_item_block);
    return NextAction::Continue;
}
IMPL_HANDLE(Else) {
    FILTER_COMMENT
    __state.condition_line = true;
    __state.condition_block_stack.top().blocks.back().length =
        last_cursor - __state.condition_block_stack.top().blocks.back().start;
    __state.condition_block_stack.top().blocks.back().end_line =
        __state.line - 1;
    ConditonItemBlock condition_item_block{};
    condition_item_block.start = last_cursor;
    condition_item_block.start_line = __state.line;
    __state.condition_block_stack.top().blocks.push_back(condition_item_block);
    return NextAction::Continue;
}
IMPL_HANDLE(Endif) {
    FILTER_COMMENT
    if (!__state.condition_block_stack.empty()) {
        __state.condition_block_stack.top().blocks.back().length =
            last_cursor -
            __state.condition_block_stack.top().blocks.back().start;
        __state.condition_block_stack.top().blocks.back().end_line =
            __state.line - 1;
    }
    __state.condition_endif_line = true;
    return NextAction::Continue;
}
IMPL_HANDLE(Include) {
    FILTER_COMMENT
    __state.include_block = true;
    __include_blocks.emplace_back(IncludeBlock{});
    __include_blocks.back().start = last_cursor;
    __include_blocks.back().start_line = __state.line;
    __include_blocks.back().end_line = __state.line;
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
            return NextAction::Break;
        }
        __include_blocks.back().include_path = std::string(YYCURSOR, en);
        __include_blocks.back().length =
            (size_t)(en - __include_blocks.back().start);
        YYCURSOR = en + 1;
        __state.include_block = false;
    } else {
        ERR VV("Invalid include path") ENDL;
        return NextAction::Break;
    }
    return NextAction::Continue;
}
IMPL_HANDLE(Define) {
    FILTER_COMMENT
    ERR VV("Start macro define") ENDL;
    __state.macro_define = true;
    __macro_define_blocks.emplace_back(MacroDefineBlock{});
    __macro_define_blocks.back().start = last_cursor;
    __macro_define_blocks.back().ident_start = (size_t)(YYCURSOR - start);
    __macro_define_blocks.back().start_line = __state.line;
    return NextAction::Continue;
}
IMPL_HANDLE(Left_parenthesis) {
    if (__state.block_comment || __state.line_comment ||
        __state.condition_line || __state.condition_endif_line ||
        __state.string)
        return NextAction::Continue;
    else if (__state.macro_define) {
        if (__state.last_token_code == TokenCode::Ident &&
            __state.last_ident_ptr ==
                start + __macro_define_blocks.back().ident_start) {
            __macro_define_blocks.back().is_function = true;
            __state.macro_param_define = true;
        }
        return NextAction::Continue;
    } else if (__state.macro_function_call) {
        if (++__state.parenthesis_count == 1) {
            __state.last_param_cursor = YYCURSOR;
            __state.last_param_line = __state.line;
        }
        return NextAction::Continue;
    }
    return NextAction::Break;
}
IMPL_HANDLE(Right_parenthesis) {
    if (__state.block_comment || __state.line_comment ||
        __state.condition_line || __state.condition_endif_line ||
        __state.string)
        return NextAction::Continue;
    else if (__state.macro_define) {
        if (__state.macro_param_define) {
            __state.macro_param_define = false;
            __macro_define_blocks.back().body_start =
                (size_t)(YYCURSOR - __macro_define_blocks.back().start);
        }
        return NextAction::Continue;
    } else if (__state.macro_function_call) {
        if (--__state.parenthesis_count == 0) {
            if (__state.last_param_cursor != last_cursor)
                __macro_idents.back().real_params.emplace_back(
                    __state.last_param_cursor,
                    YYCURSOR - __state.last_param_cursor - 1,
                    __state.last_param_line, __state.line);
            __state.macro_function_call = false;
            if (MacroExpandMacroHelper(this).parser(__macro_idents.back())) {
                return NextAction::ReturnPreCorsur;
            }
        }
        return NextAction::Continue;
    }
    return NextAction::Break;
}
IMPL_HANDLE(Comma) {
    if (__state.block_comment || __state.line_comment || __state.string ||
        __state.condition_line || __state.condition_endif_line ||
        __state.macro_define)
        return NextAction::Continue;
    else if (__state.macro_function_call) {
        if (__state.parenthesis_count == 1) {
            __macro_idents.back().real_params.emplace_back(
                __state.last_param_cursor,
                last_cursor - __state.last_param_cursor,
                __state.last_param_line, __state.line);
            __state.last_param_cursor = YYCURSOR;
            __state.last_param_line = __state.line;
        }
        return NextAction::Continue;
    }
    return NextAction::Break;
}
IMPL_HANDLE(Double_hash) {
    FILTER_COMMENT
    if (__state.macro_define) {
        __state.last_param_cursor = last_cursor;
        __state.cur_macro_param_ref_type = MacroParamRefType::Concat;
        return NextAction::Continue;
    }
    return NextAction::Break;
}
IMPL_HANDLE(Hash) {
    FILTER_COMMENT
    if (__state.macro_define) {
        __state.last_param_cursor = last_cursor;
        __state.cur_macro_param_ref_type = MacroParamRefType::ToString;
        return NextAction::Continue;
    }
    return NextAction::Break;
}
IMPL_HANDLE(Ident_before_Double_hash) {
    FILTER_COMMENT
    if (__state.macro_define) {
        auto _pos = std::min(__content->find("#", last_cursor - start),
                             __content->find(" ", last_cursor - start));
        std::string_view ident(last_cursor, _pos - (last_cursor - start));
        if (auto it = std::find_if(__macro_define_blocks.back().params.begin(),
                                   __macro_define_blocks.back().params.end(),
                                   [&](const auto &b) {
                                       return std::string_view(
                                                  +b.start, b.length) == ident;
                                   });
            it != __macro_define_blocks.back().params.end()) {
            OUT VV("find: ") SV(mm, ident) ENDL;
            __macro_define_blocks.back().params_refs.emplace_back(
                last_cursor, YYCURSOR - last_cursor,
                it - __macro_define_blocks.back().params.begin(),
                MacroParamRefType::Concat);
            __state.cur_macro_param_ref_type = MacroParamRefType::Normal;
        } else {
            YYCURSOR = start + _pos;
        }
        return NextAction::Continue;
    }
    return NextAction::Break;
}
IMPL_HANDLE(Ident) {
    FILTER_COMMENT
    auto ident = std::string_view(last_cursor, YYCURSOR - last_cursor);
    __state.last_ident_ptr = last_cursor;
    if (__state.macro_define) {
        if (__macro_define_blocks.back().ident_length == 0) {
            __macro_define_blocks.back().ident_length =
                (size_t)(YYCURSOR - start) -
                __macro_define_blocks.back().ident_start;
            __macro_define_map[std::string_view(
                start + __macro_define_blocks.back().ident_start,
                __macro_define_blocks.back().ident_length)] =
                __macro_define_blocks.size() - 1;
            __macro_define_blocks.back().body_start =
                (size_t)(YYCURSOR - start);
        } else if (__state.macro_param_define) {
            __macro_define_blocks.back().params.emplace_back(
                last_cursor, YYCURSOR - last_cursor);
        } else if (auto it = std::find_if(
                       __macro_define_blocks.back().params.begin(),
                       __macro_define_blocks.back().params.end(),
                       [&](const auto &b) {
                           return std::string_view(b.start, b.length) == ident;
                       });
                   it != __macro_define_blocks.back().params.end()) {
            OUT VV("find: ") SV(mm, ident) ENDL;
            if (__state.cur_macro_param_ref_type == MacroParamRefType::Normal) {
                __macro_define_blocks.back().params_refs.emplace_back(
                    last_cursor, YYCURSOR - last_cursor,
                    it - __macro_define_blocks.back().params.begin(),
                    __state.cur_macro_param_ref_type);
            } else
                __macro_define_blocks.back().params_refs.emplace_back(
                    __state.last_param_cursor,
                    YYCURSOR - __state.last_param_cursor,
                    it - __macro_define_blocks.back().params.begin(),
                    __state.cur_macro_param_ref_type);
            __state.cur_macro_param_ref_type = MacroParamRefType::Normal;
        }
        return NextAction::Continue;
    } else if (auto it = __macro_define_map.find(ident);
               it != __macro_define_map.end()) {
        __macro_idents.emplace_back(last_cursor, YYCURSOR - last_cursor,
                                    __state.line, it->second);
        auto &block = __macro_define_blocks[it->second];
        if (block.is_function) {
            __state.macro_function_call = true;
            return NextAction::Continue;
        } else {
            if (MacroExpandMacroHelper(this).parser(__macro_idents.back())) {
                return NextAction::ReturnPreCorsur;
            }
            return NextAction::Continue;
        }
    }
    FILTER_COMMENT
    return NextAction::Break;
}
IMPL_HANDLE(Block_comment_start) {
    FILTER_COMMENT
    __block_comment_blocks.emplace_back(PreCompiledBlock{});
    __block_comment_blocks.back().start = last_cursor;
    __block_comment_blocks.back().start_line = __state.line;
    __state.block_comment = true;
    return NextAction::Continue;
}
IMPL_HANDLE(Block_comment_end) {
    if (__state.block_comment) {
        __state.block_comment = false;
        __block_comment_blocks.back().length =
            (size_t)(YYCURSOR - __block_comment_blocks.back().start);
        __block_comment_blocks.back().end_line = __state.line;
    }
    return NextAction::Continue;
}
IMPL_HANDLE(Double_quotation_marks) {
    if (__state.translation_unit) {
        __state.translation_unit = false;
        ERR VV("End translation unit") ENDL;
        return NextAction::Continue;
    }
    if (__state.block_comment || __state.line_comment || __state.macro_define)
        return NextAction::Continue;
    __state.string = !__state.string;
    if (__state.string) {
        __string_blocks.emplace_back(PreCompiledBlock{});
        __string_blocks.back().start = last_cursor;
        __string_blocks.back().start_line = __state.line;
    } else {
        __string_blocks.back().length =
            (size_t)(YYCURSOR - __string_blocks.back().start);
        __string_blocks.back().end_line = __state.line;
    }
    return NextAction::Continue;
}
IMPL_HANDLE(Native_string) {
    ERR VV("-------------Nwe Block--------") ENDL;
    FILTER_COMMENT
    __string_blocks.emplace_back(PreCompiledBlock{});
    __string_blocks.back().start = last_cursor;
    __string_blocks.back().start_line = __state.line;
    __state.name_force_string = true;
    auto &ss = __string_blocks.back();
    auto cpos = ss.start - start;
    auto ns =
        ")" +
        (__content->substr(cpos + 1, __content->find('(', cpos) - cpos - 1)) +
        "\"";
    auto pos = __content->find(ns, cpos) + ns.size();
    __state.line += std::count(ss.start, start + pos, L'\n');

    OUT NV(ns) NV(__state.line) ENDL;
    OUT NV(pos) ENDL;
    // OUT NV(__content->find(ns,  ss.start)) ENDL;
    OUT NV(__content->at(pos - 1)) ENDL;
    OUT NV(__content->at(pos)) ENDL;
    OUT NV(__content[pos + 1]) ENDL;
    YYCURSOR = start + pos;
    OUT NV(YYCURSOR) ENDL;
    ss.length = pos - cpos;
    __state.name_force_string = false;
    if (__state.macro_define) {
        __string_blocks.pop_back();
    }
    return NextAction::Continue;
}
IMPL_HANDLE(Line_comment) {
    if (__state.block_comment || __state.line_comment || __state.string ||
        __state.macro_param_define)
        return NextAction::Continue;
    if (__state.macro_define) {
        ERR VV("End macro define") ENDL;
        __macro_define_blocks.back().length =
            (size_t)(YYCURSOR - __macro_define_blocks.back().start) - 2;
        __macro_define_blocks.back().end_line = __state.line;
        __state.macro_define = false;
    }
    __state.line_comment = true;
    __line_comment_blocks.emplace_back(PreCompiledBlock{});
    __line_comment_blocks.back().start = last_cursor;
    __line_comment_blocks.back().start_line = __state.line;
    return NextAction::Continue;
}
IMPL_HANDLE(Backslash) {
    __state.translation_unit = !__state.translation_unit;
    return NextAction::Continue;
}
IMPL_HANDLE(Eof) {
    __state.line++;
    __line_index.push_back(YYCURSOR - start);
    if (__state.translation_unit) {
        __state.translation_unit = false;
        goto end_line;
    }
    if (!(__state.block_comment || __state.line_comment ||
          __state.condition_line || __state.condition_endif_line ||
          __state.string || __state.macro_define))
        return NextAction::Break;

    if (__state.condition_line) {
        __state.condition_block_stack.top().blocks.back().condition_length =
            YYCURSOR - __state.condition_block_stack.top().blocks.back().start;
        __state.condition_line = false;
    }
    if (__state.condition_endif_line) {
        __state.condition_endif_line = false;
        OUT VV("End condition block") ENDL;
        auto b = __state.condition_block_stack.top();
        b.end_line = __state.line - 1;
        b.length = last_cursor - b.start;
        __state.condition_block_stack.pop();
        if (__state.condition_block_stack.empty()) {
            __condition_blocks.push_back(b);
        } else {
            if (__state.condition_block_stack.top().blocks.size()) {
                __state.condition_block_stack.top()
                    .blocks.back()
                    .sub_blocks.push_back(b);
            }
        }
    }
    if (__state.string) {
        __state.string = false;
    }
    if (__state.line_comment) {
        __state.line_comment = false;
        __line_comment_blocks.back().length =
            YYCURSOR - __line_comment_blocks.back().start;
        __line_comment_blocks.back().end_line = __state.line - 1;
    }
    if (__state.macro_define) {
        ERR VV("End macro define") ENDL;
        __macro_define_blocks.back().length =
            YYCURSOR - __macro_define_blocks.back().start - 1;
        __macro_define_blocks.back().end_line = __state.line - 1;
        __state.macro_define = false;
    }
end_line:
    if (*(YYCURSOR - 1) == 0) return NextAction::Break;
    return NextAction::Continue;
}
IMPL_HANDLE(Other) {
    FILTER_COMMENT
    if (__state.condition_line || __state.condition_endif_line ||
        __state.macro_define)
        return NextAction::Continue;
    return NextAction::Break;
}

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
    if (__state.macro_expand_queue.size()) {
        auto &front = __state.macro_expand_queue.front();
        if (pre_cursor < front.limit) {
            return pre_cursor++;
        } else {
            OUT NV(front.next_expanded_start - start) ENDL;
            pre_cursor = last_cursor = YYCURSOR = front.next_expanded_start;
            __state.macro_expand_queue.pop();
            if (__state.macro_expand_queue.size()) {
                return pre_cursor++;
            }
        }
    } else if (pre_cursor < YYCURSOR) {
        return pre_cursor++;
    }
    for (;;) {
#define HANDEL(code)                               \
    do {                                           \
        auto tmp = __handle<TokenCode::code>();    \
        __state.last_token_code = TokenCode::code; \
        switch (tmp) {                             \
            case NextAction::Continue:             \
                goto loop_continue;                \
            case NextAction::Break:                \
                goto loop_break;                   \
            case NextAction::ReturnPreCorsur:      \
                return pre_cursor++;               \
        }                                          \
    } while (1)
        /*!re2c
            re2c:define:YYCTYPE = char;
            re2c:yyfill:enable = 0;
            "#" whitespace "if"{
                HANDEL(If);
            }
            "#" whitespace "ifdef"{
                HANDEL(Ifdef);
            }
            "#" whitespace "elif"{
                HANDEL(Elif);
            }
            "#" whitespace "elifdef"{
                HANDEL(Elifdef);
            }
            "#" whitespace "else"{
                HANDEL(Else);
            }
            "#" whitespace "endif"{
                HANDEL(Endif);
            }
            "#" whitespace "include" whitespace [<"] {
                HANDEL(Include);
            }
            "#" whitespace "define" [ \t]+  {
                HANDEL(Define);
            }
            whitespace "##" whitespace {
                HANDEL(Double_hash);
            }
            "#" whitespace {
                HANDEL(Hash);
            }
            "(" whitespace {
                HANDEL(Left_parenthesis);
            }
            whitespace ")" {
               HANDEL(Right_parenthesis);
            }
            whitespace "," whitespace {
                HANDEL(Comma);
            }
            ident {
                HANDEL(Ident);
            }
            ident whitespace "##" whitespace {
                HANDEL(Ident_before_Double_hash);
            }
            [/][*] {
                HANDEL(Block_comment_start);
             }
            [*][/] {
                HANDEL(Block_comment_end);
             }
             "R"["] [^\x00\t\n\r \\(]* "(" {
                HANDEL(Native_string);
             }
            ["] {
                HANDEL(Double_quotation_marks);
            }
            "\\" {
                HANDEL(Backslash);
                 }
            "//" {
                HANDEL(Line_comment);
             }
            [\x00\n] {
                HANDEL(Eof);
             }
            *      {
                HANDEL(Other);
            }
        */
    loop_continue:
        last_cursor = YYCURSOR;
    }
loop_break:
    pre_cursor = last_cursor;
    last_cursor = YYCURSOR;
    return pre_cursor++;
}
std::pair<size_t, size_t> PreCompiledLexer::line_and_column(size_t pos) const {
    size_t line_num = 1;
    size_t column_num = 0;
    for (size_t i = 0; i < pos && i < __line_index.size(); ++i) {
        if (__line_index[i] <= pos) {
            line_num++;
            column_num = pos - __line_index[i];
        }
    }
    return {line_num, column_num};
}
