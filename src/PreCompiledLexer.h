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
    struct MacroDefineBlock : public PreCompiledBlock {
        size_t ident_start{0};
        size_t ident_length{0};
    };
    struct IncludeBlock : public PreCompiledBlock {
        std::wstring include_path;
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
    std::string path;
    std::wstring content;
    std::unordered_map<std::wstring, size_t> __macro_define_map;
    std::vector<IncludeBlock> __include_blocks;
    std::vector<MacroDefineBlock> __macro_define_blocks;
    std::vector<PreCompiledBlock> __string_blocks;
    std::vector<PreCompiledBlock> __line_comment_blocks;
    std::vector<PreCompiledBlock> __block_comment_blocks;
    std::vector<Ident> __macro_idents;
    std::vector<ConditonBlock> __condition_blocks;

   public:
    PreCompiledLexer(const std::string &path);

    void parse();
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
    inline const std::unordered_map<std::wstring, size_t> &macro_define_map()
        const {
        return __macro_define_map;
    }
    inline const std::vector<Ident> &macro_idents() const {
        return __macro_idents;
    }
    inline const std::vector<ConditonBlock> &condition_blocks() const {
        return __condition_blocks;
    }
};