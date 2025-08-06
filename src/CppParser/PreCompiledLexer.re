// re2c --lang c++
#include <assert.h>

#include "CppParser/CompileUnit.h"
#include "CppParser/MacroExpandMacroHelper.h"
#include "CppParser/PreCompiledLexer.h"
#undef __xcdoc_debug__
#include "utils/public.h"

/*!include:re2c "def.re" */

#define IMPL_HANDLE(code)        \
    template <>                  \
    PreCompiledLexer::NextAction \
    PreCompiledLexer::__handle<PreCompiledLexer::TokenCode::code>()
#define FILTER_COMMENT                                                  \
    if (state_.block_comment || state_.line_comment || state_.string || \
        state_.condition_line || state_.condition_endif_line ||         \
        state_.macro_function_call)                                     \
        return NextAction::Continue;
IMPL_HANDLE(If) {
    FILTER_COMMENT
    state_.condition_line = true;
    ConditonBlock condition_block{};
    condition_block.start = last_cursor_;
    condition_block.start_line = state_.line;
    ConditonItemBlock condition_item_block{};
    condition_item_block.start = last_cursor_;
    condition_item_block.start_line = state_.line;
    condition_block.blocks.push_back(condition_item_block);
    state_.condition_block_stack.push(condition_block);
    return NextAction::Continue;
}
IMPL_HANDLE(Ifdef) {
    FILTER_COMMENT
    state_.condition_line = true;
    ConditonBlock condition_block{};
    condition_block.start = last_cursor_;
    condition_block.start_line = state_.line;
    ConditonItemBlock condition_item_block{};
    condition_item_block.start = last_cursor_;
    condition_item_block.start_line = state_.line;
    condition_block.blocks.push_back(condition_item_block);
    state_.condition_block_stack.push(condition_block);
    return NextAction::Continue;
}
IMPL_HANDLE(Elif) {
    FILTER_COMMENT
    state_.condition_line = true;
    state_.condition_block_stack.top().blocks.back().length =
        last_cursor_ - state_.condition_block_stack.top().blocks.back().start;
    state_.condition_block_stack.top().blocks.back().end_line = state_.line - 1;
    ConditonItemBlock condition_item_block{};
    condition_item_block.start = last_cursor_;
    condition_item_block.start_line = state_.line;
    state_.condition_block_stack.top().blocks.push_back(condition_item_block);
    return NextAction::Continue;
}
IMPL_HANDLE(Elifdef) {
    FILTER_COMMENT
    state_.condition_line = true;
    state_.condition_block_stack.top().blocks.back().length =
        last_cursor_ - state_.condition_block_stack.top().blocks.back().start;
    state_.condition_block_stack.top().blocks.back().end_line = state_.line - 1;
    ConditonItemBlock condition_item_block{};
    condition_item_block.start = last_cursor_;
    condition_item_block.start_line = state_.line;
    state_.condition_block_stack.top().blocks.push_back(condition_item_block);
    return NextAction::Continue;
}
IMPL_HANDLE(Else) {
    FILTER_COMMENT
    state_.condition_line = true;
    state_.condition_block_stack.top().blocks.back().length =
        last_cursor_ - state_.condition_block_stack.top().blocks.back().start;
    state_.condition_block_stack.top().blocks.back().end_line = state_.line - 1;
    ConditonItemBlock condition_item_block{};
    condition_item_block.start = last_cursor_;
    condition_item_block.start_line = state_.line;
    state_.condition_block_stack.top().blocks.push_back(condition_item_block);
    return NextAction::Continue;
}
IMPL_HANDLE(Endif) {
    FILTER_COMMENT
    if (!state_.condition_block_stack.empty()) {
        state_.condition_block_stack.top().blocks.back().length =
            last_cursor_ -
            state_.condition_block_stack.top().blocks.back().start;
        state_.condition_block_stack.top().blocks.back().end_line =
            state_.line - 1;
    }
    state_.condition_endif_line = true;
    return NextAction::Continue;
}
IMPL_HANDLE(Include) {
    FILTER_COMMENT
    state_.include_block = true;
    include_blocks_.emplace_back(IncludeBlock{});
    include_blocks_.back().start = last_cursor_;
    include_blocks_.back().start_line = state_.line;
    include_blocks_.back().end_line = state_.line;
    const char *en = nullptr;
    if (YYCURSOR < limit_) {
        if (*(YYCURSOR - 1) == '"')
            en = std::find(YYCURSOR, limit_, '"');
        else if (*(YYCURSOR - 1) == '<')
            en = std::find(YYCURSOR, limit_, '>');
        else
            assert(false && "Invalid include path");
        if (en == limit_) {
            ERR VV("Invalid include path") ENDL;
            return NextAction::Break;
        }
        include_blocks_.back().include_path = std::string(YYCURSOR, en);
        if (compile_unit_) {
            auto p = compile_unit_->attach_lexer(
                include_blocks_.back().include_path);
            if (p) include_lexers_.push_back(p);
        }
        include_blocks_.back().length =
            (size_t)(en - include_blocks_.back().start);
        YYCURSOR = en + 1;
        state_.include_block = false;
    } else {
        ERR VV("Invalid include path") ENDL;
        return NextAction::Break;
    }
    return NextAction::Continue;
}
IMPL_HANDLE(Define) {
    FILTER_COMMENT
    ERR VV("Start macro define") ENDL;
    state_.macro_define = true;
    macro_define_blocks_.emplace_back(MacroDefineBlock{});
    macro_define_blocks_.back().start = last_cursor_;
    macro_define_blocks_.back().ident_start = (size_t)(YYCURSOR - start_);
    macro_define_blocks_.back().start_line = state_.line;
    return NextAction::Continue;
}
IMPL_HANDLE(Left_parenthesis) {
    if (state_.block_comment || state_.line_comment || state_.condition_line ||
        state_.condition_endif_line || state_.string)
        return NextAction::Continue;
    else if (state_.macro_define) {
        if (state_.last_token_code == TokenCode::Ident &&
            state_.last_ident_ptr ==
                start_ + macro_define_blocks_.back().ident_start) {
            macro_define_blocks_.back().is_function = true;
            state_.macro_param_define = true;
        }
        return NextAction::Continue;
    } else if (state_.macro_function_call) {
        if (++state_.parenthesis_count == 1) {
            state_.last_param_cursor = YYCURSOR;
            state_.last_param_line = state_.line;
        }
        return NextAction::Continue;
    }
    return NextAction::Break;
}
IMPL_HANDLE(Right_parenthesis) {
    if (state_.block_comment || state_.line_comment || state_.condition_line ||
        state_.condition_endif_line || state_.string)
        return NextAction::Continue;
    else if (state_.macro_define) {
        if (state_.macro_param_define) {
            state_.macro_param_define = false;
            macro_define_blocks_.back().body_start =
                (size_t)(YYCURSOR - macro_define_blocks_.back().start);
        }
        return NextAction::Continue;
    } else if (state_.macro_function_call) {
        if (--state_.parenthesis_count == 0) {
            if (state_.last_param_cursor != last_cursor_)
                macro_idents_.back().real_params.emplace_back(
                    state_.last_param_cursor,
                    YYCURSOR - state_.last_param_cursor - 1,
                    state_.last_param_line, state_.line);
            state_.macro_function_call = false;
            if (MacroExpandMacroHelper(this).parser(macro_idents_.back())) {
                return NextAction::ReturnPreCorsur;
            }
        }
        return NextAction::Continue;
    }
    return NextAction::Break;
}
IMPL_HANDLE(Comma) {
    if (state_.block_comment || state_.line_comment || state_.string ||
        state_.condition_line || state_.condition_endif_line ||
        state_.macro_define)
        return NextAction::Continue;
    else if (state_.macro_function_call) {
        if (state_.parenthesis_count == 1) {
            macro_idents_.back().real_params.emplace_back(
                state_.last_param_cursor,
                last_cursor_ - state_.last_param_cursor, state_.last_param_line,
                state_.line);
            state_.last_param_cursor = YYCURSOR;
            state_.last_param_line = state_.line;
        }
        return NextAction::Continue;
    }
    return NextAction::Break;
}
IMPL_HANDLE(Double_hash) {
    FILTER_COMMENT
    if (state_.macro_define) {
        state_.last_param_cursor = last_cursor_;
        state_.cur_macro_param_ref_type = MacroParamRefType::Concat;
        return NextAction::Continue;
    }
    return NextAction::Break;
}
IMPL_HANDLE(Hash) {
    FILTER_COMMENT
    if (state_.macro_define) {
        state_.last_param_cursor = last_cursor_;
        state_.cur_macro_param_ref_type = MacroParamRefType::ToString;
        return NextAction::Continue;
    }
    return NextAction::Break;
}
IMPL_HANDLE(Ident_before_Double_hash) {
    FILTER_COMMENT
    if (state_.macro_define) {
        auto _pos = std::min(content_->find("#", last_cursor_ - start_),
                             content_->find(" ", last_cursor_ - start_));
        std::string_view ident(last_cursor_, _pos - (last_cursor_ - start_));
        if (auto it = std::find_if(macro_define_blocks_.back().params.begin(),
                                   macro_define_blocks_.back().params.end(),
                                   [&](const auto &b) {
                                       return std::string_view(
                                                  +b.start, b.length) == ident;
                                   });
            it != macro_define_blocks_.back().params.end()) {
            OUT VV("find: ") SV(mm, ident) ENDL;
            macro_define_blocks_.back().params_refs.emplace_back(
                last_cursor_, YYCURSOR - last_cursor_,
                it - macro_define_blocks_.back().params.begin(),
                MacroParamRefType::Concat);
            state_.cur_macro_param_ref_type = MacroParamRefType::Normal;
        } else {
            YYCURSOR = start_ + _pos;
        }
        return NextAction::Continue;
    }
    return NextAction::Break;
}
IMPL_HANDLE(Ident) {
    FILTER_COMMENT
    auto ident = std::string_view(last_cursor_, YYCURSOR - last_cursor_);
    state_.last_ident_ptr = last_cursor_;
    if (state_.macro_define) {
        if (macro_define_blocks_.back().ident_length == 0) {
            macro_define_blocks_.back().ident_length =
                (size_t)(YYCURSOR - start_) -
                macro_define_blocks_.back().ident_start;
            macro_define_map_[std::string_view(
                start_ + macro_define_blocks_.back().ident_start,
                macro_define_blocks_.back().ident_length)] =
                macro_define_blocks_.size() - 1;
            macro_define_blocks_.back().body_start =
                (size_t)(YYCURSOR - start_);
        } else if (state_.macro_param_define) {
            macro_define_blocks_.back().params.emplace_back(
                last_cursor_, YYCURSOR - last_cursor_);
        } else if (auto it = std::find_if(
                       macro_define_blocks_.back().params.begin(),
                       macro_define_blocks_.back().params.end(),
                       [&](const auto &b) {
                           return std::string_view(b.start, b.length) == ident;
                       });
                   it != macro_define_blocks_.back().params.end()) {
            OUT VV("find: ") SV(mm, ident) ENDL;
            if (state_.cur_macro_param_ref_type == MacroParamRefType::Normal) {
                macro_define_blocks_.back().params_refs.emplace_back(
                    last_cursor_, YYCURSOR - last_cursor_,
                    it - macro_define_blocks_.back().params.begin(),
                    state_.cur_macro_param_ref_type);
            } else
                macro_define_blocks_.back().params_refs.emplace_back(
                    state_.last_param_cursor,
                    YYCURSOR - state_.last_param_cursor,
                    it - macro_define_blocks_.back().params.begin(),
                    state_.cur_macro_param_ref_type);
            state_.cur_macro_param_ref_type = MacroParamRefType::Normal;
        }
        return NextAction::Continue;
    } else if (auto it = macro_define_map_.find(ident);
               it != macro_define_map_.end()) {
        macro_idents_.emplace_back(last_cursor_, YYCURSOR - last_cursor_,
                                   state_.line, it->second);
        auto &block = macro_define_blocks_[it->second];
        if (block.is_function) {
            state_.macro_function_call = true;
            return NextAction::Continue;
        } else {
            if (MacroExpandMacroHelper(this).parser(macro_idents_.back())) {
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
    block_comment_blocks_.emplace_back(PreCompiledBlock{});
    block_comment_blocks_.back().start = last_cursor_;
    block_comment_blocks_.back().start_line = state_.line;
    state_.block_comment = true;
    return NextAction::Continue;
}
IMPL_HANDLE(Block_comment_end) {
    if (state_.block_comment) {
        state_.block_comment = false;
        block_comment_blocks_.back().length =
            (size_t)(YYCURSOR - block_comment_blocks_.back().start);
        block_comment_blocks_.back().end_line = state_.line;
    }
    return NextAction::Continue;
}
IMPL_HANDLE(Double_quotation_marks) {
    if (state_.translation_unit) {
        state_.translation_unit = false;
        ERR VV("End translation unit") ENDL;
        return NextAction::Continue;
    }
    if (state_.block_comment || state_.line_comment || state_.macro_define)
        return NextAction::Continue;
    state_.string = !state_.string;
    if (state_.string) {
        string_blocks_.emplace_back(PreCompiledBlock{});
        string_blocks_.back().start = last_cursor_;
        string_blocks_.back().start_line = state_.line;
    } else {
        string_blocks_.back().length =
            (size_t)(YYCURSOR - string_blocks_.back().start);
        string_blocks_.back().end_line = state_.line;
    }
    return NextAction::Continue;
}
IMPL_HANDLE(Native_string) {
    ERR VV("-------------Nwe Block--------") ENDL;
    FILTER_COMMENT
    string_blocks_.emplace_back(PreCompiledBlock{});
    string_blocks_.back().start = last_cursor_;
    string_blocks_.back().start_line = state_.line;
    state_.name_force_string = true;
    auto &ss = string_blocks_.back();
    auto cpos = ss.start - start_;
    auto ns =
        ")" +
        (content_->substr(cpos + 1, content_->find('(', cpos) - cpos - 1)) +
        "\"";
    auto pos = content_->find(ns, cpos) + ns.size();
    state_.line += std::count(ss.start, start_ + pos, L'\n');

    OUT NV(ns) NV(state_.line) ENDL;
    OUT NV(pos) ENDL;
    // OUT NV(__content->find(ns,  ss.start)) ENDL;
    OUT NV(__content->at(pos - 1)) ENDL;
    OUT NV(__content->at(pos)) ENDL;
    OUT NV(__content[pos + 1]) ENDL;
    YYCURSOR = start_ + pos;
    OUT NV(YYCURSOR) ENDL;
    ss.length = pos - cpos;
    state_.name_force_string = false;
    if (state_.macro_define) {
        string_blocks_.pop_back();
    }
    return NextAction::Continue;
}
IMPL_HANDLE(Line_comment) {
    if (state_.block_comment || state_.line_comment || state_.string ||
        state_.macro_param_define)
        return NextAction::Continue;
    if (state_.macro_define) {
        ERR VV("End macro define") ENDL;
        macro_define_blocks_.back().length =
            (size_t)(YYCURSOR - macro_define_blocks_.back().start) - 2;
        macro_define_blocks_.back().end_line = state_.line;
        state_.macro_define = false;
    }
    state_.line_comment = true;
    line_comment_blocks_.emplace_back(PreCompiledBlock{});
    line_comment_blocks_.back().start = last_cursor_;
    line_comment_blocks_.back().start_line = state_.line;
    return NextAction::Continue;
}
IMPL_HANDLE(Backslash) {
    state_.translation_unit = !state_.translation_unit;
    return NextAction::Continue;
}
IMPL_HANDLE(Eof) {
    state_.line++;
    line_index_.push_back(YYCURSOR - start_);
    if (state_.translation_unit) {
        state_.translation_unit = false;
        goto end_line;
    }
    if (!(state_.block_comment || state_.line_comment ||
          state_.condition_line || state_.condition_endif_line ||
          state_.string || state_.macro_define))
        return NextAction::Break;

    if (state_.condition_line) {
        state_.condition_block_stack.top().blocks.back().condition_length =
            YYCURSOR - state_.condition_block_stack.top().blocks.back().start;
        state_.condition_line = false;
    }
    if (state_.condition_endif_line) {
        state_.condition_endif_line = false;
        OUT VV("End condition block") ENDL;
        auto b = state_.condition_block_stack.top();
        b.end_line = state_.line - 1;
        b.length = last_cursor_ - b.start;
        state_.condition_block_stack.pop();
        if (state_.condition_block_stack.empty()) {
            condition_blocks_.push_back(b);
        } else {
            if (state_.condition_block_stack.top().blocks.size()) {
                state_.condition_block_stack.top()
                    .blocks.back()
                    .sub_blocks.push_back(b);
            }
        }
    }
    if (state_.string) {
        state_.string = false;
    }
    if (state_.line_comment) {
        state_.line_comment = false;
        line_comment_blocks_.back().length =
            YYCURSOR - line_comment_blocks_.back().start;
        line_comment_blocks_.back().end_line = state_.line - 1;
    }
    if (state_.macro_define) {
        ERR VV("End macro define") ENDL;
        macro_define_blocks_.back().length =
            YYCURSOR - macro_define_blocks_.back().start - 1;
        macro_define_blocks_.back().end_line = state_.line - 1;
        state_.macro_define = false;
    }
end_line:
    if (*(YYCURSOR - 1) == 0) {
        --YYCURSOR;
        return NextAction::Break;
    }
    return NextAction::Continue;
}
IMPL_HANDLE(Other) {
    FILTER_COMMENT
    if (state_.condition_line || state_.condition_endif_line ||
        state_.macro_define)
        return NextAction::Continue;
    return NextAction::Break;
}

PreCompiledLexer::PreCompiledLexer(const std::string *content, CompileUnit *c)
    : content_(content),
      start_(content->c_str()),
      YYCURSOR(content->c_str()),
      pre_cursor_(YYCURSOR),
      YYMARKER(YYCURSOR),
      last_cursor_(YYCURSOR),
      compile_unit_(c),
      limit_(YYCURSOR + content->size()) {
    // parse();
}
const char *PreCompiledLexer::next() {
    using namespace std;
    if (state_.macro_expand_queue.size()) {
        auto &front = state_.macro_expand_queue.front();
        if (pre_cursor_ < front.limit) {
            return pre_cursor_++;
        } else {
            OUT NV(front.next_expanded_start - start) ENDL;
            pre_cursor_ = last_cursor_ = YYCURSOR = front.next_expanded_start;
            state_.macro_expand_queue.pop();
            if (state_.macro_expand_queue.size()) {
                return pre_cursor_++;
            }
        }
    } else if (pre_cursor_ < YYCURSOR) {
        return pre_cursor_++;
    }
    for (;;) {
#define HANDEL(code)                              \
    do {                                          \
        auto tmp = __handle<TokenCode::code>();   \
        state_.last_token_code = TokenCode::code; \
        switch (tmp) {                            \
            case NextAction::Continue:            \
                goto loop_continue;               \
            case NextAction::Break:               \
                goto loop_break;                  \
            case NextAction::ReturnPreCorsur:     \
                return pre_cursor_++;             \
        }                                         \
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
        last_cursor_ = YYCURSOR;
    }
loop_break:
    pre_cursor_ = last_cursor_;
    last_cursor_ = YYCURSOR;
    return pre_cursor_++;
}
std::pair<size_t, size_t> PreCompiledLexer::line_and_column(size_t pos) const {
    size_t line_num = 1;
    size_t column_num = 0;
    for (size_t i = 0; i < pos && i < line_index_.size(); ++i) {
        if (line_index_[i] <= pos) {
            line_num++;
            column_num = pos - line_index_[i];
        }
    }
    return {line_num, column_num};
}
const string_slice_view &PreCompiledLexer::source() {
    const char *_next;
    while (*(_next = next()) != 0) {
        source_.push_char(_next);
    }
    return source_;
}
