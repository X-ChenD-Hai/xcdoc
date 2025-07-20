#pragma once
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

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
    };
    enum class MacroParamRefType {
        Normal,
        Concat,
        ToString,
    };

    struct MacroParamRef {
        size_t start{0};
        size_t length{0};
        MacroParamRefType type{MacroParamRefType::Normal};
    };
    struct MacroParam {
        size_t start{0};
        size_t length{0};
        std::vector<MacroParamRef> refs{};
    };
    struct MacroDefineBlock : public PreCompiledBlock {
        size_t ident_start{0};
        size_t ident_length{0};
        size_t body_start{0};
        bool is_function{false};
        std::vector<MacroParam> params{};
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
        Two_hash,
        Ident,
        Eof
    };

   private:
    template <TokenCode code>
    NextAction __handle();

   private:
    const std::string *__content;
    std::unordered_map<std::string, size_t> __macro_define_map;
    std::vector<IncludeBlock> __include_blocks;
    std::vector<MacroDefineBlock> __macro_define_blocks;
    std::vector<PreCompiledBlock> __string_blocks;
    std::vector<PreCompiledBlock> __line_comment_blocks;
    std::vector<PreCompiledBlock> __block_comment_blocks;
    std::vector<Ident> __macro_idents;
    std::vector<ConditonBlock> __condition_blocks;

   private:
    const char *start;
    const char *YYCURSOR;
    const char *YYMARKER = YYCURSOR;
    const char *last_cursor = YYCURSOR;
    const char *limit = YYCURSOR + __content->size();
    const char *pre_cursor;
    State __state;
    MacroParamRefType cur_macro_param_ref_type = MacroParamRefType::Normal;
    const char *__macro_end{nullptr};
    std::stack<ConditonBlock> condition_block_stack;
    std::vector<size_t> __line_index{0};
    std::stack<std::pair<const char *, const char *>> macro_expansion_stack;
    size_t line = 1;


   public:
    PreCompiledLexer(const std::string *content);

    // void parse();
    const char *next();
    inline std::pair<size_t, size_t> line_and_column(size_t pos) const {
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
    inline const std::unordered_map<std::string, size_t> &macro_define_map()
        const {
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