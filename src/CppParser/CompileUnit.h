#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "CppParser/PreCompiledLexer.h"


class PreCompiledLexer;
class CompileUnit {
    using str_list = std::vector<std::string>;
    using str_ptr_list = std::vector<std::unique_ptr<std::string>>;
    using macro_list = std::vector<std::pair<std::string, std::string>>;
    using attached_lexers_t =
        std::unordered_map<std::string, std::unique_ptr<PreCompiledLexer>>;
    str_list __includes_path;
    macro_list __ext_macros;
    std::string __source_file;
    std::string __source;
    str_list __output_files;
    std::string __command;
    std::unique_ptr<PreCompiledLexer> __pre_lexer;
    str_ptr_list __include_files;
    attached_lexers_t __attached_lexers;

   private:
    PreCompiledLexer* _attach_lexer(const std::filesystem::path& file_name,
                                    const std::string& key);

   public:
    CompileUnit(const std::string& command);
    PreCompiledLexer* attach_lexer(const std::string& file_name,
                                   bool is_absolute_path = false);
    const str_list& includes_path() const { return __includes_path; }
    const macro_list& ext_macros() const { return __ext_macros; }
    const std::string& source_file() const { return __source_file; }
    const std::string& source() const { return __source; }
    const str_list& output_files() const { return __output_files; }
    const std::string& command() const { return __command; }
    const PreCompiledLexer* pre_lexer() const { return __pre_lexer.get(); }
    const str_ptr_list& include_files() const { return __include_files; }
    const attached_lexers_t& attached_lexers() const {
        return __attached_lexers;
    }
};