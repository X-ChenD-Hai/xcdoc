#pragma once
#include "CppParser/PreCompiledLexer.h"
#include "utils/string_slice_view.h"

class PreCompiledLexer;
class MacroExpandMacroHelper {
   private:
    struct Parma {
        string_slice_view::iterator start;
        string_slice_view::iterator end;
    };
    struct Ident {
        string_slice_view::iterator start;
        string_slice_view::iterator end;
        size_t macro_id;
        std::vector<Parma> real_params;
    };
    PreCompiledLexer *lexer_;
    string_slice_view::iterator YYCURSOR;
    string_slice_view::iterator YYMARKER;
    string_slice_view::iterator last_cursor;
    std::vector<Ident> macro_idents;
    struct {
        bool macro_parma{false};
        long parenthesis_count = 0;
        string_slice_view::iterator last_param_start;
    } state_;

   public:
    MacroExpandMacroHelper(PreCompiledLexer *lexer) : lexer_(lexer) {}
    bool parser(PreCompiledLexer::Ident &ident);
    void handle_ident();
    void handle_right();
    void handle_left();
    void handle_comma();
    string_slice_view handle_str_real(const string_slice_view &p);
    string_slice_view handle_real(const string_slice_view &p);
    void expand_macro(string_slice_view *str);
};