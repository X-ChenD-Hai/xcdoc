#include <string>
#include <unordered_map>
#include <vector>

class PreCompiledLexer {
   public:
    struct PreCompiledBlock {
        size_t start;
        size_t length;
        size_t start_line;
        size_t end_line;
    };
    struct Ident {
        size_t start;
        size_t length;
        size_t line;
        size_t macro_id;
    };
    struct MacroDefineBlock : public PreCompiledBlock {
        size_t ident_start;
        size_t ident_length;
    };
    struct IncludeBlock : public PreCompiledBlock {
        std::wstring include_path;
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
};