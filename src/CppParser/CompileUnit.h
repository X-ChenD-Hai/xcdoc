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
    str_list includes_path_;
    macro_list ext_macros_;
    std::string source_file_;
    std::string source_;
    str_list output_files_;
    std::string command_;
    std::unique_ptr<PreCompiledLexer> pre_lexer_;
    str_ptr_list include_files_;
    attached_lexers_t attached_lexers_;

   private:
    PreCompiledLexer* _attach_lexer(const std::filesystem::path& file_name,
                                    const std::string& key);

   public:
    CompileUnit(const std::string& command);
    PreCompiledLexer* attach_lexer(const std::string& file_name,
                                   bool is_absolute_path = false);
    const str_list& includes_path() const { return includes_path_; }
    const macro_list& ext_macros() const { return ext_macros_; }
    const std::string& source_file() const { return source_file_; }
    const std::string& source() const { return source_; }
    const str_list& output_files() const { return output_files_; }
    const std::string& command() const { return command_; }
    const PreCompiledLexer* pre_lexer() const { return pre_lexer_.get(); }
    const str_ptr_list& include_files() const { return include_files_; }
    const attached_lexers_t& attached_lexers() const {
        return attached_lexers_;
    }
};