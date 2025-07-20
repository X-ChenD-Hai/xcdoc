// re2c --lang c++
#include <assert.h>

#include "./PreCompiledLexer.h"
#include "utils.h"

/*!include:re2c "def.re" */

#define IMPL_HANDLE(code)        \
    template <>                  \
    PreCompiledLexer::NextAction \
    PreCompiledLexer::__handle<PreCompiledLexer::TokenCode::code>()
#define FILTER_COMMENT                                                   \
    if (__state.block_comment || __state.line_comment || __state.string) \
        return NextAction::Continue;
IMPL_HANDLE(If) {
    FILTER_COMMENT
    __state.condition_line = true;
    ConditonBlock condition_block{};
    condition_block.start = last_cursor - start;
    condition_block.start_line = line;
    ConditonItemBlock condition_item_block{};
    condition_item_block.start = last_cursor - start;
    condition_item_block.start_line = line;
    condition_block.blocks.push_back(condition_item_block);
    condition_block_stack.push(condition_block);
    return NextAction::Continue;
}
IMPL_HANDLE(Ifdef) {
    FILTER_COMMENT
    __state.condition_line = true;
    ConditonBlock condition_block{};
    condition_block.start = last_cursor - start;
    condition_block.start_line = line;
    ConditonItemBlock condition_item_block{};
    condition_item_block.start = last_cursor - start;
    condition_item_block.start_line = line;
    condition_block.blocks.push_back(condition_item_block);
    condition_block_stack.push(condition_block);
    return NextAction::Continue;
}
IMPL_HANDLE(Elif) {
    FILTER_COMMENT
    __state.condition_line = true;
    condition_block_stack.top().blocks.back().length =
        last_cursor - start - condition_block_stack.top().blocks.back().start;
    condition_block_stack.top().blocks.back().end_line = line - 1;
    ConditonItemBlock condition_item_block{};
    condition_item_block.start = last_cursor - start;
    condition_item_block.start_line = line;
    condition_block_stack.top().blocks.push_back(condition_item_block);
    return NextAction::Continue;
}
IMPL_HANDLE(Elifdef) {
    FILTER_COMMENT
    __state.condition_line = true;
    condition_block_stack.top().blocks.back().length =
        last_cursor - start - condition_block_stack.top().blocks.back().start;
    condition_block_stack.top().blocks.back().end_line = line - 1;
    ConditonItemBlock condition_item_block{};
    condition_item_block.start = last_cursor - start;
    condition_item_block.start_line = line;
    condition_block_stack.top().blocks.push_back(condition_item_block);
    return NextAction::Continue;
}
IMPL_HANDLE(Else) {
    FILTER_COMMENT
    __state.condition_line = true;
    condition_block_stack.top().blocks.back().length =
        last_cursor - start - condition_block_stack.top().blocks.back().start;
    condition_block_stack.top().blocks.back().end_line = line - 1;
    ConditonItemBlock condition_item_block{};
    condition_item_block.start = last_cursor - start;
    condition_item_block.start_line = line;
    condition_block_stack.top().blocks.push_back(condition_item_block);
    return NextAction::Continue;
}
IMPL_HANDLE(Endif) {
    FILTER_COMMENT
    if (!condition_block_stack.empty()) {
        condition_block_stack.top().blocks.back().length =
            last_cursor - start -
            condition_block_stack.top().blocks.back().start;
        condition_block_stack.top().blocks.back().end_line = line - 1;
    }
    __state.condition_endif_line = true;
    return NextAction::Continue;
}
IMPL_HANDLE(Include) {
    FILTER_COMMENT
    __state.include_block = true;
    __include_blocks.emplace_back(IncludeBlock{});
    __include_blocks.back().start = (size_t)(YYMARKER - start);
    __include_blocks.back().start_line = line;
    __include_blocks.back().end_line = line;
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
            (size_t)(en - start - __include_blocks.back().start);
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
    __macro_define_blocks.back().start = (size_t)(last_cursor - start);
    __macro_define_blocks.back().ident_start = (size_t)(YYCURSOR - start);
    __macro_define_blocks.back().start_line = line;
    return NextAction::Continue;
}
IMPL_HANDLE(Left_parenthesis) {
    if (__state.block_comment || __state.line_comment ||
        __state.condition_line || __state.condition_endif_line ||
        __state.string)
        return NextAction::Continue;
    else if (__state.macro_define) {
        __macro_define_blocks.back().is_function = true;
        __state.macro_param_define = true;
        return NextAction::Continue;
    }
    return NextAction::Break;
}
IMPL_HANDLE(Two_hash) {
    if (__state.block_comment || __state.line_comment ||
        __state.condition_line || __state.condition_endif_line ||
        __state.string)
        return NextAction::Continue;
    else if (__state.macro_define) {
        cur_macro_param_ref_type = MacroParamRefType::Concat;
        return NextAction::Continue;
    }
    return NextAction::Break;
}
IMPL_HANDLE(Hash) {
    if (__state.block_comment || __state.line_comment ||
        __state.condition_line || __state.condition_endif_line ||
        __state.string)
        return NextAction::Continue;
    else if (__state.macro_define) {
        cur_macro_param_ref_type = MacroParamRefType::ToString;
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
        __state.macro_param_define = false;
        return NextAction::Continue;
    }
    return NextAction::Break;
}
IMPL_HANDLE(Ident) {
    FILTER_COMMENT
    auto ident = __content->substr(last_cursor - start, YYCURSOR - last_cursor);
    if (__state.macro_define) {
        if (__macro_define_blocks.back().ident_length == 0) {
            __macro_define_blocks.back().ident_length =
                (size_t)(YYCURSOR - start) -
                __macro_define_blocks.back().ident_start;
            __macro_define_map[__content->substr(
                __macro_define_blocks.back().ident_start,
                __macro_define_blocks.back().ident_length)] =
                __macro_define_blocks.size() - 1;
            __macro_define_blocks.back().body_start =
                (size_t)(YYCURSOR - start);
        } else if (__state.macro_param_define) {
            __macro_define_blocks.back().params.emplace_back(
                last_cursor - start, YYCURSOR - last_cursor,
                std::vector<MacroParamRef>{});
        } else if (auto it = std::find_if(
                       __macro_define_blocks.back().params.begin(),
                       __macro_define_blocks.back().params.end(),
                       [&](const auto &b) {
                           OUT VV("compare: ")
                               SV(mm, __content->substr(b.start, b.length))
                                   NV(ident) ENDL;
                           return __content->substr(b.start, b.length) == ident;
                       });
                   it != __macro_define_blocks.back().params.end()) {
            OUT VV("find: ") SV(mm, ident) ENDL;
            it->refs.emplace_back(last_cursor - start, YYCURSOR - last_cursor,
                                  cur_macro_param_ref_type);
            cur_macro_param_ref_type = MacroParamRefType::Normal;
        }
    } else if (auto it = __macro_define_map.find(ident);
               it != __macro_define_map.end()) {
        __macro_idents.emplace_back(last_cursor - start, YYCURSOR - last_cursor,
                                    line, it->second);
        last_cursor = YYCURSOR;
        macro_expansion_stack.emplace(YYCURSOR, last_cursor);
        pre_cursor = start + __macro_define_blocks[it->second].body_start;
        __macro_end = start + __macro_define_blocks[it->second].start +
                      __macro_define_blocks[it->second].length;
        return NextAction::ReturnPreCorsur;
    } else if (!(__state.block_comment || __state.line_comment ||
                 __state.condition_line || __state.condition_endif_line ||
                 __state.string))
        return NextAction::Break;

    return NextAction::Continue;
}
IMPL_HANDLE(Block_comment_start) {
    FILTER_COMMENT
    __block_comment_blocks.emplace_back(PreCompiledBlock{});
    __block_comment_blocks.back().start = (size_t)(YYMARKER - start);
    __block_comment_blocks.back().start_line = line;
    __state.block_comment = true;
    return NextAction::Continue;
}
IMPL_HANDLE(Block_comment_end) {
    if (__state.block_comment) {
        __state.block_comment = false;
        __block_comment_blocks.back().length =
            (size_t)(YYCURSOR - start) - __block_comment_blocks.back().start;
        __block_comment_blocks.back().end_line = line;
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
        __string_blocks.back().start = (size_t)(YYMARKER - start);
        __string_blocks.back().start_line = line;
    } else {
        __string_blocks.back().length =
            (size_t)(YYCURSOR - start) - __string_blocks.back().start;
        __string_blocks.back().end_line = line;
    }
    return NextAction::Continue;
}
IMPL_HANDLE(Native_string) {
    ERR VV("-------------Nwe Block--------") ENDL;
    FILTER_COMMENT
    __string_blocks.emplace_back(PreCompiledBlock{});
    __string_blocks.back().start = (size_t)(YYMARKER - start);
    __string_blocks.back().start_line = line;
    __state.name_force_string = true;
    auto &ss = __string_blocks.back();
    auto ns = ")" +
              (__content->substr(ss.start + 1, __content->find('(', ss.start) -
                                                   ss.start - 1)) +
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
    __state.name_force_string = false;
    if (__state.macro_define) {
        __string_blocks.pop_back();
    }
    return NextAction::Continue;
}
IMPL_HANDLE(Line_comment) {
    if (__state.block_comment || __state.line_comment || __state.string ||
        __state.macro_define)
        return NextAction::Continue;
    __state.line_comment = true;
    __line_comment_blocks.emplace_back(PreCompiledBlock{});
    __line_comment_blocks.back().start = (size_t)(YYMARKER - start);
    __line_comment_blocks.back().start_line = line;
    return NextAction::Continue;
}
IMPL_HANDLE(Backslash) {
    __state.translation_unit = !__state.translation_unit;
    return NextAction::Continue;
}
IMPL_HANDLE(Eof) {
    line++;
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
        condition_block_stack.top().blocks.back().condition_length =
            (YYCURSOR - start) -
            condition_block_stack.top().blocks.back().start;
        __state.condition_line = false;
    }
    if (__state.condition_endif_line) {
        __state.condition_endif_line = false;
        OUT VV("End condition block") ENDL;
        auto b = condition_block_stack.top();
        b.end_line = line - 1;
        b.length = last_cursor - start - b.start;
        condition_block_stack.pop();
        if (condition_block_stack.empty()) {
            __condition_blocks.push_back(b);
        } else {
            if (condition_block_stack.top().blocks.size()) {
                condition_block_stack.top().blocks.back().sub_blocks.push_back(
                    b);
            }
        }
    }
    if (__state.string) {
        __state.string = false;
    }
    if (__state.line_comment) {
        __state.line_comment = false;
        __line_comment_blocks.back().length =
            (size_t)(YYCURSOR - start) - __line_comment_blocks.back().start;
        __line_comment_blocks.back().end_line = line - 1;
    }
    if (__state.macro_define) {
        ERR VV("End macro define") ENDL;
        __macro_define_blocks.back().length =
            (size_t)(YYCURSOR - start) - __macro_define_blocks.back().start - 1;
        __macro_define_blocks.back().end_line = line - 1;
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
    for (;;) {
#define HANDEL(code)                       \
    switch (__handle<TokenCode::code>()) { \
        case NextAction::Continue:         \
            goto loop_continue;            \
        case NextAction::Break:            \
            goto loop_break;               \
        case NextAction::ReturnPreCorsur:  \
            return pre_cursor++;           \
    }
        /*!re2c
            re2c:define:YYCTYPE = char;
            re2c:yyfill:enable = 0;
            whitespace = [ \t]*;
            ident     = ident_;
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
                HANDEL(Endif)
            }
            "#" whitespace "include" whitespace [<"] {
                HANDEL(Include)
            }
            "#" whitespace "define" [ \t]+  {
                HANDEL(Define)
            }
            "(" {
                HANDEL(Left_parenthesis)
            }
            whitespace "##" whitespace {
                HANDEL(Two_hash)
            }
            whitespace "#" whitespace {
                HANDEL(Hash)
            }
            ")" {
               HANDEL(Right_parenthesis)
            }
            ident {
                HANDEL(Ident)
            }
            [/][*] {
                HANDEL(Block_comment_start)
             }
            [*][/] {
                HANDEL(Block_comment_end)
             }
             "R"["] [^\x00\t\n\r \\(]* "(" {
                HANDEL(Native_string)
             }
            ["] {
                HANDEL(Double_quotation_marks)
            }
            "\\" {
                HANDEL(Backslash)
                 }
            "//" {
                HANDEL(Line_comment)
             }
            [\x00\n] {
                HANDEL(Eof)
             }
            *      {
                HANDEL(Other)
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
