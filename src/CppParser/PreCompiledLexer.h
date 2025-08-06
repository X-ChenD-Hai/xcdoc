#pragma once
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/string_slice_view.h"

class CompileUnit;

class PreCompiledLexer {
    friend class MacroExpandMacroHelper;

   public:
    struct PreCompiledBlock {
        const char *start{nullptr};
        size_t length{0};
        size_t start_line{0};
        size_t end_line{0};
    };
    struct Ident {
        const char *start{nullptr};
        size_t length{0};
        size_t line{0};
        size_t macro_id{0};
        std::vector<PreCompiledBlock> real_params;
    };
    enum class MacroParamRefType {
        Normal,
        Concat,
        ToString,
    };

    struct MacroParamRef {
        const char *start{nullptr};
        size_t length{0};
        size_t shape_param_id{0};
        MacroParamRefType type{MacroParamRefType::Normal};
    };
    struct MacroParam {
        const char *start{nullptr};
        size_t length{0};
    };
    struct MacroDefineBlock : public PreCompiledBlock {
        size_t ident_start{0};
        size_t ident_length{0};
        size_t body_start{0};
        bool is_function{false};
        std::vector<MacroParam> params{};
        std::vector<MacroParamRef> params_refs{};
    };
    struct IncludeBlock : public PreCompiledBlock {
        std::string include_path;
    };
    struct ConditonBlock;
    struct ConditonItemBlock : public PreCompiledBlock {
        size_t condition_length{0};
        std::vector<ConditonBlock> sub_blocks{};
    };
    struct ConditonBlock : public PreCompiledBlock {
        std::vector<ConditonItemBlock> blocks{};
    };

   private:
    struct ExpandedState {
        const char *limit;
        const char *next_expanded_start;
    };
    enum class TokenCode;
    struct State {
        bool include_block : 1 = false;
        bool macro_define : 1 = false;
        bool line_comment : 1 = false;
        bool block_comment : 1 = false;
        bool string : 1 = false;
        bool translation_unit : 1 = false;
        bool name_force_string : 1 = false;
        bool condition_line : 1 = false;
        bool condition_endif_line : 1 = false;
        bool macro_param_define : 1 = false;
        bool macro_function_call : 1 = false;
        TokenCode last_token_code = TokenCode::Other;
        const char *last_ident_ptr{nullptr};
        size_t line = 1;
        size_t parenthesis_count = 0;
        size_t last_param_line = 0;
        const char *last_param_cursor{};
        MacroParamRefType cur_macro_param_ref_type = MacroParamRefType::Normal;
        std::stack<ConditonBlock> condition_block_stack;
        std::queue<ExpandedState> macro_expand_queue;
    };
    enum class NextAction { Continue, Break, ReturnPreCorsur };
    enum class TokenCode {
        Other,
        Whitespace,
        If,
        Ifdef,
        Elif,
        Elifdef,
        Else,
        Endif,
        Include,
        Define,
        Left_parenthesis,
        Right_parenthesis,
        Block_comment_start,
        Block_comment_end,
        Backslash,
        Line_comment,
        Double_quotation_marks,
        Native_string,
        Hash,
        Double_hash,
        Ident_before_Double_hash,
        Ident,
        Comma,
        Eof
    };

   private:
    template <TokenCode code>
    NextAction __handle();

   private:
    const char *start_;
    const char *YYCURSOR;
    const char *YYMARKER = YYCURSOR;
    const char *last_cursor_ = YYCURSOR;
    const char *limit_ = YYCURSOR + content_->size();
    const char *pre_cursor_;
    const std::string *content_;
    CompileUnit *compile_unit_{nullptr};
    std::vector<PreCompiledLexer *> include_lexers_;
    string_slice_view source_;
    std::unordered_map<std::string_view, size_t> macro_define_map_;
    std::vector<IncludeBlock> include_blocks_;
    std::vector<MacroDefineBlock> macro_define_blocks_;
    std::vector<PreCompiledBlock> string_blocks_;
    std::vector<PreCompiledBlock> line_comment_blocks_;
    std::vector<PreCompiledBlock> block_comment_blocks_;
    std::vector<Ident> macro_idents_;
    std::vector<ConditonBlock> condition_blocks_;
    std::vector<size_t> line_index_{0};
    State state_;

   public:
    PreCompiledLexer(const std::string *content,
                     CompileUnit *compile_unit = nullptr);

    // void parse();
    const char *next();
    std::pair<size_t, size_t> line_and_column(size_t pos) const;
    inline const std::vector<IncludeBlock> &include_blocks() const {
        return include_blocks_;
    }
    inline const std::vector<MacroDefineBlock> &macro_define_blocks() const {
        return macro_define_blocks_;
    }
    inline const std::vector<PreCompiledBlock> &string_blocks() const {
        return string_blocks_;
    }
    inline const std::vector<PreCompiledBlock> &line_comment_blocks() const {
        return line_comment_blocks_;
    }
    inline const std::vector<PreCompiledBlock> &block_comment_blocks() const {
        return block_comment_blocks_;
    }
    inline const std::unordered_map<std::string_view, size_t> &
    macro_define_map() const {
        return macro_define_map_;
    }
    inline const std::vector<Ident> &macro_idents() const {
        return macro_idents_;
    }
    inline const std::vector<ConditonBlock> &condition_blocks() const {
        return condition_blocks_;
    }
    inline const std::string &content() const { return *content_; }

    inline size_t pos(const char *ptr) const {
        auto tmp = ptr - start_;
        if (tmp < 0 || tmp > content_->size()) {
            return content_->size();
        }
        return tmp;
    }
    const string_slice_view &source();
    const string_slice_view &source() const { return source_; };
};