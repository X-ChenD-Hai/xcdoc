#pragma once
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "./string_slice_view.h"

class PreCompiledLexer {
   public:
    struct PreCompiledBlock {
        size_t start{0};
        size_t length{0};
        size_t start_line{0};
        size_t end_line{0};
    };
    struct Ident {
        size_t start{0};
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
        size_t start{0};
        size_t length{0};
        size_t shape_param_id{0};
        MacroParamRefType type{MacroParamRefType::Normal};
    };
    struct MacroParam {
        size_t start{0};
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
    void __expand_macro();

   private:
    const std::string *__content;
    std::unordered_map<std::string_view, size_t> __macro_define_map;
    std::vector<IncludeBlock> __include_blocks;
    std::vector<MacroDefineBlock> __macro_define_blocks;
    std::vector<PreCompiledBlock> __string_blocks;
    std::vector<PreCompiledBlock> __line_comment_blocks;
    std::vector<PreCompiledBlock> __block_comment_blocks;
    std::vector<Ident> __macro_idents;
    std::vector<ConditonBlock> __condition_blocks;
    std::vector<size_t> __line_index{0};

   private:
    const char *start;
    const char *YYCURSOR;
    const char *YYMARKER = YYCURSOR;
    const char *last_cursor = YYCURSOR;
    const char *limit = YYCURSOR + __content->size();
    const char *pre_cursor;
    State __state;

   public:
    PreCompiledLexer(const std::string *content);

    // void parse();
    const char *next();
    std::pair<size_t, size_t> line_and_column(size_t pos) const;
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
    inline const std::unordered_map<std::string_view, size_t> &
    macro_define_map() const {
        return __macro_define_map;
    }
    inline const std::vector<Ident> &macro_idents() const {
        return __macro_idents;
    }
    inline const std::vector<ConditonBlock> &condition_blocks() const {
        return __condition_blocks;
    }
    inline const std::string &content() const { return *__content; }
};