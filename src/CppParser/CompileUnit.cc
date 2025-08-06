#include "./CompileUnit.h"

#include <utils/public.h>

#include <filesystem>
#include <nlohmann/json.hpp>

#include "./CommandParser.h"
#include "PreCompiledLexer.h"

using json = nlohmann::json;
CompileUnit::CompileUnit(const std::string& command) : command_(command) {
    auto parser = CommandParser::parse(command_);
    OUT SV(command, command_) ENDL;
    namespace fs = std::filesystem;
    if (parser) {
        source_file_ = parser->input_files().front();
        includes_path_.push_back(fs::path(source_file_).parent_path().string());
        includes_path_.append_range(parser->include_paths());
        ext_macros_ = parser->macros();
        output_files_ = parser->output_files();
        source_ = utils::read_file(source_file_);
        pre_lexer_ = std::make_unique<PreCompiledLexer>(&source_, this);
        for (auto i : includes_path_) {
            OUT SV(include_path, i) ENDL;
        }
        OUT NV(source_file_) ENDL;
        pre_lexer_->source();
    }
}
PreCompiledLexer* CompileUnit::attach_lexer(const std::string& file_name,
                                            bool is_absolute_path) {
    namespace fs = std::filesystem;
    auto it = attached_lexers_.find(file_name);

    OUT SV(search_file, file_name) ENDL;
    if (it == attached_lexers_.end()) {
        if (is_absolute_path) {
            OUT VV("\tabsolute path") ENDL;
            if (auto l = _attach_lexer(fs::path(file_name), file_name); l) {
                OUT VV("\tfound") ENDL;
                l->source();
                return l;
            }
            return nullptr;
        }
        for (auto& ps : includes_path_) {
            OUT SV(\tsearch_on, fs::path(ps) / file_name) ENDL;
            if (auto l = _attach_lexer(fs::path(ps) / file_name, file_name);
                l) {
                OUT VV("\tfound") ENDL;
                l->source();
                return l;
            }
        }
        OUT VV("\tnot found") ENDL;
    } else {
        OUT VV("\talready attached") ENDL;
        return it->second.get();
    }
    return nullptr;
}
PreCompiledLexer* CompileUnit::_attach_lexer(const std::filesystem::path& ps,
                                             const std::string& key) {
    namespace fs = std::filesystem;
    if (fs::is_regular_file(ps)) {
        auto content = new std::string(utils::read_file((ps.string())));
        include_files_.emplace_back(content);
        auto lexer = std::make_unique<PreCompiledLexer>(content, this);
        return attached_lexers_.emplace(key, std::move(lexer))
            .first->second.get();
    }
    return nullptr;
};