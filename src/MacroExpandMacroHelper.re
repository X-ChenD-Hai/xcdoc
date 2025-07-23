// re2c --lang c
#include "./MacroExpandMacroHelper.h"
#include "./utils.h"
/*!include:re2c "def.re" */

bool MacroExpandMacroHelper::parser(PreCompiledLexer::Ident &ident) {
    auto &macro = lexer->__macro_define_blocks[ident.macro_id];
    if (macro.start + macro.length == macro.body_start) return false;

    string_slice_view str;
    auto _last_end = lexer->start + macro.body_start;
    for (auto &ref : macro.params_refs) {
        str.push(_last_end, lexer->start + ref.start);
        _last_end = lexer->start + ref.start + ref.length;
        if (ref.shape_param_id < ident.real_params.size()) {
            auto &real = ident.real_params[ref.shape_param_id];
            if (ref.type == PreCompiledLexer::MacroParamRefType::ToString) {
                if (ref.type == PreCompiledLexer::MacroParamRefType::ToString)
                    str.push(handle_str_real(
                        {lexer->start + real.start, real.length}));
                else
                    str.push(
                        handle_real({lexer->start + real.start, real.length}));
            } else {
                str.push(lexer->start + real.start, real.length);
            }
        }
    }
    str.push(_last_end, lexer->start + macro.start + macro.length);
    OUT NV(str) ENDL;

    do {
        expand_macro(&str);
        last_cursor = YYMARKER = YYCURSOR = str.begin();
        for (;;) {
            /*!re2c
                re2c:define:YYCTYPE = char;
                re2c:yyfill:enable = 0;
                str {
                    goto loop_continue;
                }
                "(" whitespace {
                    handle_left();
                    goto loop_continue;
                }
                whitespace ")" {
                    handle_right();
                    goto loop_continue;
                }
                whitespace "," whitespace {
                    handle_comma();
                    goto loop_continue;
                }
                ident {
                    handle_ident();
                    goto loop_continue;
                }
                [\x00]{
                    break;
                }
                * {
                    goto loop_continue;
                }
             */
        loop_continue:
            last_cursor = YYCURSOR;
        }
        OUT VV("Expanding ----") ENDL;
        for (auto idt : macro_idents) {
            OUT SV(idt, string_slice_view(idt.start, idt.end)) ENDL;
            for (size_t i = 0; i < idt.real_params.size(); ++i) {
                auto &real = idt.real_params[i];
                OUT VV("\t") SV(id, i)
                    SV(real, string_slice_view(real.start, real.end)) ENDL;
            }
        }
    } while (macro_idents.size());
    auto &q = lexer->__state.macro_expand_queue;
    auto &sq = str.string_refs();
    auto _last_cursor = lexer->YYCURSOR;
    for (size_t i = 1; i < sq.size(); ++i) {
        auto &r = sq[i - 1];
        auto &nr = sq[i];
        q.emplace(r.str + r.len, nr.str);
    }
    if (sq.size()) {
        lexer->pre_cursor = sq.front().str;
        auto &er = sq.back();
        q.emplace(er.str + er.len, lexer->YYCURSOR);
    }
    return true;
}
void MacroExpandMacroHelper::handle_ident() {
    if (__state.macro_parma) return;
    auto ident = std::string(last_cursor, YYCURSOR);
    if (auto it = lexer->macro_define_map().find(ident);
        it != lexer->macro_define_map().end()) {
        macro_idents.emplace_back(last_cursor, YYCURSOR, it->second,
                                  std::vector<Parma>{});
        if (lexer->__macro_define_blocks[it->second].is_function) {
            __state.macro_parma = true;
        }
    }
}
void MacroExpandMacroHelper::handle_right() {
    if (__state.macro_parma) {
        if (--__state.parenthesis_count == 0) {
            macro_idents.back().real_params.emplace_back(
                __state.last_param_start, last_cursor);
            macro_idents.back().end = YYCURSOR;
            __state.macro_parma = false;
        }
    }
}
void MacroExpandMacroHelper::handle_left() {
    if (__state.macro_parma) {
        if (++__state.parenthesis_count == 1) {
            __state.last_param_start = YYCURSOR;
        }
    }
}
void MacroExpandMacroHelper::handle_comma() {
    if (__state.parenthesis_count == 1) {
        macro_idents.back().real_params.emplace_back(__state.last_param_start,
                                                     last_cursor);
        __state.last_param_start = YYCURSOR;
    }
}
void MacroExpandMacroHelper::expand_macro(string_slice_view *str) {
    if (macro_idents.empty()) return;
    string_slice_view res;
    auto last_start = str->begin();
    for (auto &ident : macro_idents) {
        res.push(last_start, ident.start);
        OUT NV(res) ENDL;
        last_start = ident.end;
        auto &macro = lexer->__macro_define_blocks[ident.macro_id];
        if (macro.start + macro.length == macro.body_start) continue;
        auto _last_start = lexer->start + macro.body_start;
        for (auto &ref : macro.params_refs) {
            res.push(_last_start, lexer->start + ref.start);
            OUT NV(res) ENDL;
            _last_start = lexer->start + ref.start + ref.length;
            OUT NV(ref.shape_param_id) NV(ident.real_params.size()) ENDL;
            if (ref.shape_param_id < ident.real_params.size()) {
                auto &real = ident.real_params[ref.shape_param_id];
                OUT SV(real, string_slice_view(real.start, real.end)) ENDL;
                if (ref.type == PreCompiledLexer::MacroParamRefType::ToString)
                    res.push(handle_str_real({real.start, real.end}));
                else
                    res.push(handle_real({real.start, real.end}));
                OUT NV(res) ENDL;
            }
        }
        res.push(_last_start, lexer->start + macro.start + macro.length);
        OUT NV(res) ENDL;
    }
    res.push(last_start, str->end());
    OUT SV(res - str, *str) ENDL;
    OUT SV(res, res) ENDL;
    *str = res;
    macro_idents.clear();
}
string_slice_view MacroExpandMacroHelper::handle_real(
    const string_slice_view &p) {
    last_cursor = YYMARKER = YYCURSOR = p.begin();
    string_slice_view::iterator ls = p.begin();
    static auto _S = " ";
    string_slice_view s;
    for (;;) {
    /*!re2c
        * {
            last_cursor = YYCURSOR;
            continue;
        }
        (str)+ {
            last_cursor = YYCURSOR;
            continue;
        }
        (line_comment|block_comment|whitespace_with_line)+ {
            s.push(ls, last_cursor);
            s.push(_S, 1);
            last_cursor = YYCURSOR;
            ls = YYCURSOR;
            continue;
        }
        [\x00] {
            if(ls!=last_cursor){
                s.push(ls, last_cursor);
                s.push(_S, 1);}
            break;
        }
     */}
    s.pop_back();
    return s;
}

string_slice_view MacroExpandMacroHelper::handle_str_real(
    const string_slice_view &p) {
    last_cursor = YYMARKER = YYCURSOR = p.begin();
    string_slice_view::iterator ls = p.begin();
    static auto _S = "\"";
    string_slice_view s(_S, 1);
    s.push(handle_real(p));
    s.push(_S, 1);
    return s;
};
