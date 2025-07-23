// re2c --lang c
#include "./MacroExpandMacroHelper.h"
#include "./utils.h"
/*!include:re2c "def.re" */

bool MacroExpandMacroHelper::parser(PreCompiledLexer::Ident &ident) {
    static auto __S = " \"";
    auto &macro = lexer->__macro_define_blocks[ident.macro_id];
    if (macro.start + macro.length == macro.body_start) return false;

    string_slice_view str;
    auto _last_end = lexer->start + macro.body_start;
    for (auto &ref : macro.params_refs) {
        auto &real = ident.real_params[ref.shape_param_id];
        str.push(_last_end, lexer->start + ref.start);
        if (ref.type == PreCompiledLexer::MacroParamRefType::ToString) {
            str.push(__S + 1, 1);
            string_slice_view ss(lexer->start + real.start, real.length);
            handle_str_real(&ss);
            str.push(ss);
            str.push(__S + 1, 1);
        } else {
            str.push(lexer->start + real.start, real.length);
        }
        _last_end = lexer->start + ref.start + ref.length;
    }
    str.push(_last_end, lexer->start + macro.start + macro.length);
    OUT NV(str) ENDL;

    last_cursor = YYMARKER = YYCURSOR = str.begin();
    do {
        expand_macro(str);
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
                    goto warp_end;
                }
                * {
                    goto loop_continue;
                }
             */
        loop_continue:
            last_cursor = YYCURSOR;
        }
    warp_end:
    } while (macro_idents.size());
    expand_macro(str);
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
        if (--__state.quote_count == 0) {
            macro_idents.back().real_parms.emplace_back(
                __state.last_param_start, last_cursor);
            __state.macro_parma = false;
        }
    }
}
void MacroExpandMacroHelper::handle_left() {
    if (__state.macro_parma) {
        if (++__state.quote_count == 1) {
            __state.last_param_start = YYCURSOR;
            __state.last_param_start = last_cursor;
        }
    }
}
void MacroExpandMacroHelper::handle_comma() {
    if (__state.quote_count == 1) {
        macro_idents.back().real_parms.emplace_back(__state.last_param_start,
                                                    last_cursor);
    }
}
void MacroExpandMacroHelper::expand_macro(string_slice_view &str) {
    // TODO
    macro_idents.clear();
}
void MacroExpandMacroHelper::handle_str_real(string_slice_view *p) {
    last_cursor = YYMARKER = YYCURSOR = p->begin();
    string_slice_view::iterator ls = p->begin();
    string_slice_view s;
    static auto _S = " ";
    for (;;) {
    /*!re2c
        * {
            last_cursor = YYCURSOR;
            continue;
        }
        ([\n \t\v]|[\\][\n])+ {
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
    *p = s;
};
